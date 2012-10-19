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
void SharkSkeleton::sAngle(int key, int angle, bool downstream)
{
	gBone(itoa(key))->changeAngle(angle, downstream);
}
void SharkSkeleton::sAngle(int key, int angle)
{
	sAngle(itoa(key),angle);
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

/*Applies matrix transformations to the SharkMesh. Accomplishes this by starting the recursive chain in transformHierarchy.
 * After calling this, the Keyframe is ready to save the vertex information. Flag in mesh (hasNewTransform) will be set. */
void SharkSkeleton::applyTransformation()
{
	nmesh->newUpdateApproved = false;
	//perform all the matrix magic in there to make a keyframe.

	MyMat stackMatrix = MyMat();

	//this sets the mesh back so it's aligned in world coordinates as it should be.
	stackMatrix.multRight(armTranslation);	

	bones[rootNode]->transformBone(&stackMatrix);
	//transformHeirarchy(stackMatrix); //transform
	nmesh->hasNewTransform = true; //polling for new Keyframes will succeed now.
}


