#include "SharkSkeleton.h"


SharkBone* SharkSkeleton::gBone(string key)
{
	return bones.find(key)->second;
}

string SharkSkeleton::itoa(int key)
{
	char dec = (char) key%10;
	char tens = (char) (key/10)%10;
	char hunds = (char) (key/100)%10;
	char thous = (char) (key/1000)%10;
	char tents = (char) (key/10000)%10;
	return string(1,tents)+string(1,thous)+string(1,hunds)+string(1,tens)+string(1,dec);
}

SharkBone* SharkSkeleton::gBone(int key)  //bone getter where the key prefers to be an integer
{
	return gBone(itoa(key));
}

/*sets the bone, with its name as its key */
void SharkSkeleton::sBone(SharkBone* newBone)
{
	bones[newBone->gName()] = newBone;	
}

/*sets the angle on the armature. the key is the bone name. */
void SharkSkeleton::sAngle(string key, int angle)
{
	gBone(key)->changeAngle(angle);
}

/* Builds the skeleton from the CalShark shark model. This is the non-general build method
 * Given the initial Blender mesh, build the smart mesh and the bones that make up the skeleton. 
 * Needs to know, the number of segments that make up the shark, and the length of the segments*/
void SharkSkeleton::buildSkeleton(Mesh* mesh, int numSegments, float *segLength)
{
	MyMat multiplier;  //this is here just to multiply matrices toghether
	float start; //start and end determine where to slice the mesh into bones. 
	float end;
	const int rootNum = 4;   //number down the spine where the root is.

	//Mesh transformation to smart sharkMesh form. 
	start = mesh->lengthMax; 
	end = mesh->lengthMax;

	rootNode = itoa(rootNum);   //set the name of the root
	//build the individual bones	
	for(int i = 0; i < numSegments; i++)
	{
		SharkBone * newBone = new SharkBone(nmesh, i);
		end = start;
		start -= segLength[i];
		newBone->buildBone(mesh, start, end, multiplier);
		if(i == rootNum-1){     //the translation ahead of the root node will not line up w/o an additional translation	
			MyMat forwardtrans;
			forwardtrans.makeTranslate(Vector3f(newBone->gLength(), 0, 0));
			newBone->sJointTranslation(forwardtrans);
			newBone->boneLengthToTranslation(i < rootNum);
		}
		else {
			newBone->boneLengthToTranslation(i < rootNum);
		}
		totalLength += newBone->gLength();
		newBone->sName(itoa(i));
		sBone(newBone); //name the bone and put it into the map.
	}
	
	//create the heirarchy in the bones
	for(int i = rootNum; i > 0; i--)  //from head to root
	{
		gBone(i)->addChild(gBone(i-1));   
	}
	for(int i = bones.size()-1; i > rootNum; i--)
	{
		gBone(i-1)->addChild(gBone(i));
	}
	armTranslation.makeTranslate(Vector3f(gBone(rootNode)->boneLength, 0, 0));
	nmesh->newUpdateApproved = true;
}

/*copies user defined animation frames into the class. It needs the number of the current sequence, and the total number of frames. 
 * the array should be stored in row-majorness*/
void SharkSkeleton::buildAnimations(int totalFrames, GLfloat *segmentRot, int totalAngles)
{
	framesPerSequence = totalFrames;
	anglesPerFrame = totalAngles;
	for(int i = 0; i < totalFrames; i++)
	{
		buildAngles(&segmentRot[i*totalAngles], i, totalAngles);	
		curveAngles.push_back(0);
	}
}

//copies the angles into the simulation structure
bool SharkSkeleton::buildAngles( GLfloat segmentRot[], int sequenceNum, int totalAngles )
{
	bool ff = true;
	animatedAngles.push_back(vector<int>());
	//check to see if the sequence is empty, otherwise precondition has been violated
	if(animatedAngles[sequenceNum].size() == 0)
	{
		ff = false;
	}
	//copy over
	for(int i = 0; i < totalAngles; i++)
	{
		animatedAngles[sequenceNum].push_back(segmentRot[i]);
	}
	return ff;
}

// source http://animalcreativity.webs.com/109.pdf
float SharkSkeleton::deriveFrequency(float velocity)
{
	//Velocity = .25(length(3freq - 4))
	//therefore
	//f = (4(v+L))/(3L)
	
	return (4.0*( velocity+ TSEMI_LENGTH_M))/(3.0*TSEMI_LENGTH_M);	
}



/*main update method for the simulation 
* Rail angle is the angle provided from the world, showing the sharpness of the turn on the point of the rail the shark is at.*/
void SharkSkeleton::update(int dt, int railAngle, float velocity)
{
	elapsedTime += dt/1000.0;
	//velocity factors
	//swimFrequency = deriveFrequency(velocity); 
	//propellingAmplitude = Vector3f(0.0, .967(?) , 0.0).Interpolate(Vector3f(0.1, 5.0, 0), swimFrequency); //velocity ;  
				////amplitude increases with frequency until a max is reached at 5 beats per ssecond.
	//propellingAmplitude = Vector3f(0.1, 5.0, 0).Interpolate(Vector3f(.0, 0.967, 0), 
	//						(swimFrequency > 5.0 ? 5.0 : swimFrequency)).y;
	//propellingAmplitude = swimFrequency / velocityToAmp; // TODO amplitude scale

	//check if the recalculate flag is set
	if(nmesh->newUpdateApproved)
	{
		//figure out what the next keyframe's angles are going to be.  dt is converted from miliseconds to seconds
		calcNextAngles(railAngle);

		//change the bones
		setNewAngles();

		//apply and export the transformations to the SharkMesh
		applyTransformation();
	}
	//printf("amp: %f freq:%f  ang: %d \n", propellingAmplitude, swimFrequency, turningAngle);
}

/*pushes an angle down on the rail angle queue */
void SharkSkeleton::findRailCurve(int railAngle)
{
	curveAngles.insert(curveAngles.begin(), railAngle);
	curveAngles.erase(curveAngles.end()-1);
}

int SharkSkeleton::nextSegmentAngle(int prevSegmentAngle, int prevTimeAngle, int maxAngle)
{
	//An = Ka*sin(2pi*f*t - prev) + TA

	int TA = turningAngle;   //TA is the turning angle for this segment
	//TA = TA / gNumLocomotionBones();
	
	if(prevTimeAngle != 0)
	//if(turningAngle != 0)
	//turningAngle /= bones.size();
	{
		//TA = Ki*(Amax - Aactual)/Aactual   
		//TODO find out what is wrong with this equation. It does not make sense.
		//TODO fix the maxAngle usage. 
		
		//TA = prevTimeAngle*(maxAngle-turningAngle) / turningAngle; //turning angle
		//TA = prevSegmentAngle*(maxAngle-prevTimeAngle)/ prevTimeAngle; //turning angle
		//TA = turningAngle*((maxAngle-prevTimeAngle) / maxAngle);//prevTimeAngle; //turning angle
	}
	int finalAngle = (propellingAmplitude*sin((2.0*180.0*swimFrequency*elapsedTime - prevSegmentAngle)*3.14159265/180.0))* 180.0/3.14159265;
	//int finalAngle = (propellingAmplitude*sin((2.0*180.0*swimFrequency*elapsedTime - prevSegmentAngle)*3.14159265/180.0) + TA)* 180.0/3.14159265;
	//printf("%d\n", finalAngle);
	if(finalAngle > maxAngle){finalAngle = maxAngle; }
	if(finalAngle < -maxAngle){finalAngle = -maxAngle; }
	finalAngle += TA;
	return finalAngle; 
}

/*Sets the final angles that can be turned into the Keyframe */
void SharkSkeleton::calcNextAngles(int railAngle)
{
	//Add the animation frame angles to the computed turn angle
	findRailCurve(railAngle);

	vector<int> oldAngles = finalAngles;
	finalAngles.clear();
	vector<int> maxAngles = getMaxAngles();

	if(elapsedTime == 0) //first time running
	{
		oldAngles.clear();
		for(int i = 0; i < anglesPerFrame; i++)
		{
			oldAngles.push_back(0);	
		}
	}

	int prevSegmentAngle = 0;
	turningAngle = railAngle;
	for(int i = 0; i < anglesPerFrame; i++)
	{
		int newAngle = 0;
		if(i >= anglesPerFrame - gNumLocomotionBones()){
			newAngle = nextSegmentAngle(prevSegmentAngle, oldAngles[i], maxAngles[i]);
		}
		finalAngles.push_back(newAngle);
		prevSegmentAngle = newAngle;
	}
	//Next Frame Overflow calculation
	nextFrameNo++;
	if(nextFrameNo >= framesPerSequence){ nextFrameNo = 0;}

}

/*Gives the number of segments being used for locomotion, verses those that are holding 'still'. 
 * This depends on the locomotion Mode of the shark */
int SharkSkeleton::gNumLocomotionBones()
{
	switch(locomotionMode)
	{
	case ANGUILIFORM:
		return anglesPerFrame;
	case SUBCARANGIFORM:
		return anglesPerFrame - anglesPerFrame/4;
	case CARANGIFORM:
		return anglesPerFrame - 2*anglesPerFrame/4;
	case THUNNIFORM:
		return anglesPerFrame - 3*anglesPerFrame/4;
	default:
		return -1;
	}
}

/*return the maximum flexibility of shark parts depending ont he type of locomotion and the number of bone segments  */
vector<int> SharkSkeleton::getMaxAngles()
{
	vector<int> newAngles;
	const int looseAngle = 47;  //magic s
	const int stiffAngle = 1;
	int i = 0;

	//Angles dampen nearer to the tail
	for(i = 0; i < anglesPerFrame - gNumLocomotionBones(); i++)
	{
		newAngles.push_back(stiffAngle);
	}
	for(/*i stays the same */; i < anglesPerFrame; i++)
	{
		newAngles.push_back(looseAngle - i*1.5);  //magic
	}	
	return newAngles;
}


void SharkSkeleton::setNewAngles()   //clearly a Loco function
{
	for(int i = 0; i < anglesPerFrame; i++)
	{
		gBone(i)->changeAngle(finalAngles[i], i<4);  //magic number is root node in Loco
	}
}

/* This prepares the matrix stack for transforming the heirchical shark skeleton. The individual bone transformations will be set by this.
 * it is recursive
 * isDownstream is a value that tells the mesh whether it is recursing towards the tail or towards the head of the shark from the root node. the root node is zero, 1 is tailward,
 * 	2 is headward.
 * the curSegment is used to index the bones with.
 * the stack is the rotations and translations. */
void SharkSkeleton::transformHeirarchy(int isDownstream, MyMat newstack)
{
	//transformt the Bone itself. The matrix is modified by this action
	bones[rootNode]->transformBone(&newstack);
}

/*Applies matrix transformations to the SharkMesh. Accomplishes this by starting the recursive chain in transformHierarchy.
 * After calling this, the Keyframe is ready to save the vertex information. Flag in mesh (hasNewTransform) will be set. */
void SharkSkeleton::applyTransformation()
{
	nmesh->newUpdateApproved = false;
	//perform all the matrix magic in there to make a keyframe.

	MyMat stackMatrix = MyMat();

	//this sets the mesh back so it's aligned in world coordinates as it should be.
	stackMatrix.multRight(armTranslation);	

	transformHeirarchy(0, stackMatrix); //transform
	nmesh->hasNewTransform = true; //polling for new Keyframes will succeed now.
}


