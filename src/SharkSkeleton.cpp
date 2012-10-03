#include "SharkSkeleton.h"


/*Given the initial Blender mesh, build the smart mesh and the bones that make up the skeleton. 
 * Needs to know, the number of segments that make up the shark, and the length of the segments*/
void SharkSkeleton::buildSkeleton(Mesh* mesh, int numSegments, float *segLength)
{
	MyMat multiplier;  //this is here just to multiply matrices toghether
	float start; //start and end determine where to slice the mesh into bones. 
	float end;

	//Mesh transformation to smart sharkMesh form. 
	start = mesh->lengthMax; 
	end = mesh->lengthMax;
	for(int i = 0; i < numSegments; i++)
	{
		SharkBone newBone = SharkBone(nmesh, i);
		end = start;
		start -= segLength[i];
		newBone.buildBone(mesh, start, end, multiplier);
		bones.push_back(newBone);

	}
	nmesh->newUpdateApproved = true;
	//printf("numBones %d\n", bones.size());
}

/*copies user defined animation frames into the class. It needs the number of the current sequence, and the total number of frames. 
 * the array should be stored in row-majorness*/
void SharkSkeleton::buildAnimations(  int totalFrames, GLfloat *segmentRot, int totalAngles)
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

/*main update method for the simulation 
* Rail angle is the angle provided from the world, showing the sharpness of the turn on the point of the rail the shark is at.*/
void SharkSkeleton::update(int dt, int railAngle, float velocity)
{
	//velocity factors
	propellingAmplitude = velocity / velocityToAmp;

	//check if the recalculate flag is set
	if(nmesh->newUpdateApproved)
	{
		//figure out what the next keyframe's angles are going to be.
		calcNextAngles(railAngle);

		//change the bones
		setNewAngles();

		//apply and export the transformations to the SharkMesh
		applyTransformation();
	}
	printf("%f %d\n", propellingAmplitude, turningAngle);
}

/*pushes an angle down on the rail angle queue */
void SharkSkeleton::findRailCurve(int railAngle)
{
	curveAngles.insert(curveAngles.begin(), railAngle);
	curveAngles.erase(curveAngles.end()-1);
}

int SharkSkeleton::nextSegmentAngle(int prevSegmentAngle, int prevTimeAngle, int maxAngle)
{
	int TA = turningAngle;
	/*if(prevTimeAngle != 0)
	{
		TA = turningAngle*(maxAngle-prevTimeAngle) / prevTimeAngle; //turning angle
	}*/
	int finalAngle = (propellingAmplitude*sin((2.0*3.14159265*swimFrequency*elapsedTime - prevSegmentAngle)*3.14159265/180.0))* 180.0/3.14159265;
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

	elapsedTime += 1;

	//TODO take max angle into account
	int prevSegmentAngle = 0;
	turningAngle = railAngle;
	for(int i = 0; i < anglesPerFrame; i++)
	{
		//turningAngle = (turningAngle) - (bones.size())/i;  //turning angle distributed across segments 

		//turningAngle = (3.0*turningAngle) / (bones.size() - (bones.size() - rootNode) );
		int newAngle = nextSegmentAngle(prevSegmentAngle, oldAngles[i], maxAngles[i]);
		finalAngles.push_back(newAngle);
		prevSegmentAngle = newAngle;
	}
	//Next Frame Overflow calculation
	nextFrameNo++;
	if(nextFrameNo >= framesPerSequence){ nextFrameNo = 0;}

}

/*return the maximum flexibility of shark parts depending ont he type of locomotion and the number of bone segments  */
vector<int> SharkSkeleton::getMaxAngles()
{
	vector<int> newAngles;
	const int looseAngle = 35;
	const int stiffAngle = 1;
	int i = 0;
	if(locomotionMode == ANGUILIFORM)
	{
		for(int i = 0; i < anglesPerFrame; i++)
		{
			newAngles.push_back(looseAngle);
		}
	}
	else if(locomotionMode == SUBCARANGIFORM)
	{
		for(i = 0; i < anglesPerFrame/4; i++)
		{
			newAngles.push_back(stiffAngle);
		}
		for(;i < anglesPerFrame; i++)
		{
			newAngles.push_back(looseAngle);
		}
	}
	else if(locomotionMode == CARANGIFORM)
	{
		for(i = 0; i < 2*anglesPerFrame/4; i++)
		{
			newAngles.push_back(stiffAngle);
		}
		for(;i < anglesPerFrame; i++)
		{
			newAngles.push_back(looseAngle);
		}
	}
	else if(locomotionMode == THUNNIFORM)
	{
		for(i = 0; i < 3*anglesPerFrame/4; i++)
		{
			newAngles.push_back(stiffAngle);
		}
		for(;i < anglesPerFrame; i++)
		{
			newAngles.push_back(looseAngle);
		}
	}
	return newAngles;
}

void SharkSkeleton::setNewAngles()
{
	for(int i = 0; i < anglesPerFrame; i++)
	{
		bones[i].changeAngle(finalAngles[i], i<rootNode);
	}
}

/* This prepares the matrix stack for transforming the heirchical shark skeleton. The individual bone transformations will be set by this.
 * it is recursive
 * isDownstream is a value that tells the mesh whether it is recursing towards the tail or towards the head of the shark from the root node. the root node is zero, 1 is tailward,
 * 	2 is headward.
 * the curSegment is used to index the bones with.
 * the stack is the rotations and translations. */
void SharkSkeleton::transformHeirarchy(int isDownstream, int curSegment, MyMat newstack)
{
	//transformt the Bone itself.
	bones[curSegment].transformBone(&newstack, isDownstream);
	//printf("r %d ", curSegment);

	//if there were other bones that are not part of the spine (which are not part of the design as of this writing), they would recurse at this point.

	//then recurse to the other bones.
	if(isDownstream == 1 && curSegment < bones.size()-1 ) //going towards tail
	{
		transformHeirarchy(isDownstream, curSegment+1, newstack);
	}
	else if(isDownstream == 2 && curSegment > 0) //gowing towards head
	{
		transformHeirarchy(isDownstream, curSegment-1, newstack);
	}
	else if(isDownstream == 0)//root node
	{
		//make two directions
		if(rootNode > 0) //move forward
		{
			MyMat forwardtrans;
			forwardtrans.makeTranslate(Vector3f((bones[curSegment-1].boneLength), 0, 0));
			transformHeirarchy(2, curSegment-1,forwardtrans.multLeft(newstack));
					
		}
		if(rootNode < bones.size()-1) //move backward
		{
			transformHeirarchy(1,curSegment+1, newstack);
		}
	}
}

/*Applies matrix transformations to the SharkMesh. Accomplishes this by starting the recursive chain in transformHierarchy.
 * After calling this, the Keyframe is ready to save the vertex information. Flag in mesh (hasNewTransform) will be set. */
void SharkSkeleton::applyTransformation()
{
	nmesh->newUpdateApproved = false;
	//perform all the matrix magic in there to make a keyframe.

	MyMat stackMatrix = MyMat();

	//this sets the mesh back so it's aligned in world coordinates as it should be.
	stackMatrix.makeTranslate(Vector3f(bones[rootNode].boneLength, 0, 0));

	transformHeirarchy(0, rootNode, stackMatrix); //transform
	nmesh->hasNewTransform = true; //polling for new Keyframes will succeed now.
}


