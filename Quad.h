#ifndef _QUADERLATERIAL_3D_THING
#define _QUADERLATERIAL_3D_THING
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "Vector.h"
#include "SharkVertex.h"


/*A quad is a set of four points that make a rectangle. The two vertices towards the shark front are numbers zero and three */
class Quad
{
	public:
		Quad(){front = NULL; back = NULL; left = NULL; right = NULL; verts[0] = NULL; 
			verts[1] = NULL; verts[2] = NULL; verts[3] = NULL; //rawVerts[0] = NULL;
			//rawVerts[1] = NULL; rawVerts[2] = NULL; rawVerts[3] = NULL; 
		}
		
		Quad(SharkVertex *one, SharkVertex *two, SharkVertex  *three, SharkVertex *four)
			{front = NULL; back = NULL; left = NULL; right = NULL; verts[0] = one; 
			verts[1] = two; verts[2] = three; verts[3] = four; //rawVerts[0] = NULL;
			//rawVerts[1] = NULL; rawVerts[2] = NULL; rawVerts[3] = NULL; 
		}
		
		/*Quad(Vector3f *one, Vector3f *two, Vector3f *three, Vector3f *four, 
		     Vector3f *rone, Vector3f *rtwo, Vector3f *rthree, Vector3f *rfour)
		{
			front = NULL; back = NULL; left = NULL; right = NULL; verts[0] = one; 
			verts[1] = two; verts[2] = three; verts[3] = four; rawVerts[0] = rone;
			rawVerts[1] = rtwo; rawVerts[2] = rthree; rawVerts[3] = rfour; 
		}*/

		void locateAdjacants(vector<Quad*> lisq);
		bool compareLeft(Quad oth);
		bool compareRight(Quad oth);
		bool compareFront(Quad oth);
		bool compareBack(Quad oth);
		Vector3f calcNormal();	
		static double attenuate(int numedges, int iteration, int k);		
		
		//Vector3f* verts[4];
		//Vector3f* rawVerts[4];
		//Vector3f normal[4];
		SharkVertex* verts[4];
		Vector3f faceNormal;
		int boneNo;
		Quad* front;
		Quad* back;
		Quad* left;
		Quad* right;
};

#endif
