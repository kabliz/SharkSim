#ifndef SHARK_BONE_3D_D
#define SHARK_BONE_3D_D

#include <cstdio>
#include <cstdlib>
#include <vector>
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
		void transformBone(MyMat* stackmatri, int isDownstream);
		//Quad and SharkVertex form
		void changeAngle(int newAngle, bool isAheadRoot); //changes the quaternion and associted angle
		//void draw(); //draws quads to screen 
		void printToFrame();

		//lowerclass management
		Quad* gQuad(int index){return quads[index];}
		void sQuad(Quad* q){return quads.push_back(q);}
		void sLength(double len){boneLength = len;}
		double gLength(){return boneLength;}
		

	//private:		
		vector<Quad*> quads; //Quads are organized into rows, and each row has a list of quads in it
		SharkMesh *sMesh;  //pointer to the main shark mesh.
		MyMat rotationMatrix;
		double boneLength; //this is the length of the mesh segment. It will be helpful in knowing the amount to translate by.

	private:
		int boneNo;  // the index of the bone in the skeleton
		glQuaternion rotatQ;
		float angleOfRot; //rotation along xz plane
		double uLength; //if the shark were to travel the path like a monorail, 
				//this is the u difference between the bone passing the same location as the bone in front of it.
		float startB;
		float endB;  //these are the start and end points on the bone, relative to the entire mesh in units. related to boneLength.
};

#endif

