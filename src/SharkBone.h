#ifndef SHARK_BONE_3D_D
#define SHARK_BONE_3D_D

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>
#include "Quad.h"
#include "glQuaternion.h"
#include "SharkVertex.h"
#include "Mesh.h"
#include "SharkMesh.h"
#include "MyMat.h"

/*This is a shark bone, a collection of quads and vertices that can all rotate and translate in unison. It makes up a SharkSkeleton,
 which simulates the movement of a shark's swim by moving the individual bones around. */
class SharkBone
{
	public:
		SharkBone(SharkMesh *nmesh, int bon){ sMesh = nmesh; boneNo = bon;}
		~SharkBone(){}
		void buildBone(Mesh *mesh, float start, float end, MyMat matrixMultiplier); //takes in mesh input and records them in 
		void transformBone(MyMat* stackmatri);
		//Quad and SharkVertex form
		void changeAngle(int newAngle, bool isAheadRoot); //cal shark. changes the quaternion and associted angle
		void changeAngle(int newAngle); 
		void changeAngle(glQuaternion newAngle); 
		void printToFrame();

		//lowerclass management
		Quad* gQuad(int index){return quads[index];}
		void sQuad(Quad* q){return quads.push_back(q);}
		void sLength(double len){boneLength = len;}
		double gLength(){return boneLength;}
		void boneLengthToTranslation(bool downStream); //for the non-general building (Cal Shark) method only  	
		void addTranslation(MyMat trans){transMatHeir.multRight(trans);}
		void sJointTranslation(MyMat trans){jointTrans.multRight(trans);}
		string gName(){return boneName;}
		void sName(string newname){boneName = newname;}
		
		void addChild(SharkBone* ch){childBones.push_back(ch);}

	//private:		
		vector<Quad*> quads; //Quads are organized into rows, and each row has a list of quads in it
		SharkMesh *sMesh;  //pointer to the main shark mesh.
		
		MyMat rotationMatrix;
		double boneLength; //this is the length of the mesh segment. 

	private:
		string boneName;   //given bone name.  A bone's name should be unique in its skeleton.
		int boneNo;  // the index of the bone in the skeleton
		glQuaternion rotatQ;
		float angleOfRot; //rotation along xz plane
		double uLength; //if the shark were to travel the path like a monorail, 
				//this is the u difference between the bone passing the same location as the bone in front of it.
		float startB;
		float endB;  //these are the start and end points on the bone, relative to the entire mesh in units. related to boneLength.
		
		MyMat transMatHeir;   //apply this translation to the stack.           translates from boneLength
		MyMat transMatLocal;      //only apply this translation to this bone.  translates from startB and endB
		MyMat jointTrans;     //any custom trans add here. Possibly hacky.

		//child bones. Recursive transform downwards
		vector<SharkBone*> childBones;
};
#endif

