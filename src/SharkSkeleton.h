#ifndef SHARK_SKELETON_BONES_ANIMATION_3D
#define SHARK_SKELETON_BONES_ANIMATION_3D

#include "SharkBone.h"
#include "Keyframe.h"
#include "MyMat.h"
#include "Mesh.h"
//#include "SharkFacts.h"
#include "glQuaternion.h"
#include <cmath>
#include <map>


/*This is the Skeletel model of an animated kinematic system. An armature. 
 * It is an interface to the bones which can turn them into Keyframes so they can be drawn  */
class SharkSkeleton
{
	public:
		//initializations
		SharkSkeleton(){ bones = map<string, SharkBone*>(); rootNode = ""; isLinearBlendSkinned = true; }
		SharkSkeleton(SharkMesh *smesh){nmesh = smesh;  bones = map<string, SharkBone*>(); isLinearBlendSkinned = true; }
		~SharkSkeleton(){}		 
		
		void buildSkeleton(Mesh *mesh, int numSegs, float* segLengths); //given a set of mesh segments, it creates the bones. 
		void buildSkeletonAOBJ(string filename); //given a set of mesh segments, it creates the bones. 
		void setMesh(SharkMesh *s){nmesh =s;}
		void clearHeap(); //TODO clean the heap out


		//manipulations
		void sAngle(string boneName, int angle);
		void sAngle(int boneName, int angle);      
		void sAngle(int boneName, int angle, bool reverseAngledir);      
		void sAngle(string boneName, int angle, bool reverseAngledir);      
		void sAngle(string boneName, glQuaternion rotation);      
		void sRoot(string newRoot){rootNode = newRoot;}

		//transformation related functions
		void applyTransformation();	//export sketon to the smart SharkMesh  so it can be drawn by the Keyframe
		
		bool newUpdateApproved(){return nmesh->newUpdateApproved;}

		void draw();

	private:
	
		//objs
		map<string, SharkBone*> bones; //List of Bones in the shark. 

		//matrices
		MyMat armTranslation;   //A transform matrix that will move the whole armature before positioning the bones. 
					//use in case the model is offset when drawn.	

		vector<int> angles;
		//double segmentLength;
		string rootNode; //root node of the heirarchical model.
		SharkMesh *nmesh;
		double totalLength;  //the length of the shark, in world coordinates.
		bool isLinearBlendSkinned; 

		//animation frames
		int nextFrameNo;    //TODO ??? what is this again
		
		//Bone manipulation
		SharkBone* gBone(string boneName);
		SharkBone* gBone(int boneName);
		string itoa(int toConvert);       //int to string.   not really atoi backwards, but supports numbers up to 100000.  
		void sBone(SharkBone* newBone);   //sets bone with its name as the key

		string nextToken(char delimiter, FILE* readFile);	
};


#endif
