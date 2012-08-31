#ifndef _3D_SHARK_KEYFRAME_SYSTEM_MOOSE
#define _3D_SHARK_KEYFRAME_SYSTEM_MOOSE
#include <cstdlib>
#include <cstdio>
#include <map>
#include <GL/glut.h>
#include "Quad.h"
#include "MyMat.h"
#include "Mesh.h"
#include "glQuaternion.h"
#include "SharkMesh.h"

/*struct compareVect3
{
        bool operator() (Vector3f one, Vector3f two) const
        {
		
		if (!(one.x < two.x+.00001 && one.x > two.x-.00001))
		{
			return one.x < two.x;//+.00001;
		}
		else if (!(one.y < two.y+.00001 && one.y > two.y-.00001))
		{
			
			return one.y < two.y;//+.00001;
		}
		else
		{
			//printf("<%f %f %f>, <%f, %f, %f> %d \n", one.x, one.y, one.z, two.x, two.y, two.z, one.z < two.z+.00001);
			return one.z < two.z;//+.00001;
		}
        }
};*/


class Keyframe
{
	public:
		Keyframe(){}
		Keyframe(Vector3f* rawVerts);
		Keyframe(map<Vector3f, SharkVertex*, compareVect3> *rawVerts, vector<Quad*> *quads); 
		~Keyframe(){}
		
		void draw(void);
		void gatherTransformData(GLfloat segmentRot[], GLfloat segLengthInput[], Mesh *mesh, int numsegments, 
					glQuaternion *glQuat);
		void deleteHeap(void);
		static void drawInBetween(Keyframe start, Keyframe end, int step, int max) {start.drawInBetween(end, step, max);}	
		void drawInBetween(Keyframe end, int step, int max);	


	private:
			
		vector<Quad*> faces;
		map<Vector3f, SharkVertex*, compareVect3> uVertices;
		Keyframe *nextFrame;
		int frameNo;
		int followingBtwns;
		static int defaultBtwns;
		
		void createQuads(void);
		void multiBoneAttenuate();	
		Vector3f interpolateVertices(Vector3f first, Vector3f second, int step,int max);
		void segmentMatrixMake( GLfloat segmentRot[], GLfloat segLength[], Mesh *mesh, int segments, 
				glQuaternion *glQuat, int curSegment,  MyMat *stackmatrix, float start, float end, int iD);
		void transformHeirarchy(int startingPoint, int isDownstream, GLfloat segmentRot[], 
					GLfloat segLength[], Mesh *mesh, int segments,
        				glQuaternion *glQuat, int curSegment, MyMat stackMatrix, float start, float end);


};
#endif
