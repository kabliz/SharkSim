#include "SharkLoco.h"

const string SharkLoco::spineKeys[10] = {string("Spine1"), string("Spine2"), string("Spine3"), string("Spine4"), 
				      string("Spine5"), string("Spine6"), string("Spine7"), string("Spine8"), string("Spine9"), string("Spine10")};
const string SharkLoco::leftPec = "PecLeft";       //Pectoral Fins
const string SharkLoco::rightPec = "PecRight";       
const string SharkLoco::leftPel = "PelLeft";       //Pelvic Fins
const string SharkLoco::rightPel = "PelRight";       
const string SharkLoco::dorsal1 = "Dorsal1";       
const string SharkLoco::dorsal2 = "Dorsal2";       
const string SharkLoco::anal = "Anal";       
const string SharkLoco::lowCaudal = "Caudal";


/* Builds the skeleton from the CalShark shark model. This is the non-general build method
 * Given the initial Blender mesh, build the smart mesh and the bones that make up the skeleton. 
 * Needs to know, the number of segments that make up the shark, and the length of the segments*/
void SharkLoco::buildSkeleton(Mesh* mesh, int numSegments, float *segLength)
{
	skeleton.buildSkeleton(mesh, numSegments, segLength);
}

void SharkLoco::buildSkeleton(string modelFile)
{
	skeleton.sRoot("Root");
	skeleton.buildSkeletonAOBJ(modelFile);
}

/*copies user defined animation frames into the class. It needs the number of the current sequence, and the total number of frames. 
 * the array should be stored in row-majorness*/
void SharkLoco::buildAnimations(int totalFrames, GLfloat *segmentRot, int totalAngles)
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
bool SharkLoco::buildAngles( GLfloat segmentRot[], int sequenceNum, int totalAngles )
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
float SharkLoco::deriveFrequency(float velocity)
{
	//Velocity = .25(length(3freq - 4))
	//therefore
	//f = (4(v+L))/(3L)
	
	return (4.0*( velocity+ TSEMI_LENGTH_M))/(3.0*TSEMI_LENGTH_M);	
}



/*main update method for the simulation 
* Rail angle is the angle provided from the world, showing the sharpness of the turn on the point of the rail the shark is at.*/
void SharkLoco::update(int dt, int railAngle, float velocity)
{
	elapsedTime += dt/1000.0;
	//velocity factors
	//swimFrequency = deriveFrequency(velocity/velocityToAmp); 
	//propellingAmplitude = Vector3f(0.0, .967(?) , 0.0).Interpolate(Vector3f(0.1, 5.0, 0), swimFrequency); //velocity ;  
				////amplitude increases with frequency until a max is reached at 5 beats per ssecond.
	//propellingAmplitude = Vector3f(0.1, 5.0, 0).Interpolate(Vector3f(.0, 0.967, 0), 
	//						(swimFrequency > 5.0 ? 5.0 : swimFrequency)).y;
	//propellingAmplitude = swimFrequency / velocityToAmp; // TODO amplitude scale. uncomment to renable movement

	//check if the recalculate flag is set
	if(skeleton.newUpdateApproved())
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
void SharkLoco::findRailCurve(int railAngle)
{
	curveAngles.insert(curveAngles.begin(), railAngle);
	curveAngles.erase(curveAngles.end()-1);
}

int SharkLoco::nextSegmentAngle(int prevSegmentAngle, int prevTimeAngle, int maxAngle)
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
	//printf("%d %f %f %f\n", finalAngle, propellingAmplitude, swimFrequency, elapsedTime);
	return finalAngle; 
}

/*Sets the final angles that can be turned into the Keyframe */
void SharkLoco::calcNextAngles(int railAngle)
{
	//Add the animation frame angles to the computed turn angle
	findRailCurve(railAngle);

	vector<int> oldAngles = finalAngles;
	finalAngles.clear();
	vector<int> maxAngles = getMaxAngles();

	//if(elapsedTime == 0) //first time running
	if(oldAngles.size() == 0) //first time running
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
	//nextFrameNo++;   //TODO why is this here again
	//if(nextFrameNo >= framesPerSequence){ nextFrameNo = 0;}

}

/*Gives the number of segments being used for locomotion, verses those that are holding 'still'. 
 * This depends on the locomotion Mode of the shark */
int SharkLoco::gNumLocomotionBones()
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
vector<int> SharkLoco::getMaxAngles()
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

/*Changes the bones with the angles set in finalAngles */
void SharkLoco::setNewAngles()  
{
	for(int i = 0; i < anglesPerFrame; i++)
	{
		skeleton.sAngle(spineKeys[i], finalAngles[i], i<4);  //magic number is root bone 
	}
}

/*Applies matrix transformations to the SharkMesh. Accomplishes this by starting the recursive chain in the armature.
 * After calling this, the Keyframe is ready to save the vertex information. Flag in mesh (hasNewTransform) will be set. */
void SharkLoco::applyTransformation()
{
	skeleton.applyTransformation();
}


