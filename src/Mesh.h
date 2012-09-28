#ifndef _MESH_3D_OBJECT_SHARK_
#define _MESH_3D_OBJECT_SHARK_
//#include <GL\glui.h>
//#include <GL/glui.h>
#include "glui/include/GL/glui.h"
#include <GL/glut.h>
#include "Vector.h"

class Mesh
{
public:
	Mesh();
	void readBlender(const char* file);
	void init_blender();
	void draw();
	void drawBetween(float start, float end);
	void calculateNormals();
//private:


	Vector3f vertList[3000];	
	Vector3f normals[3000];
	float blenderTotalLength;

	int vertCounter;
	float lengthMax;
	float lengthMin;
	float heightMax;
	float heightMin;
	float widthMax;
	float widthMin;
	
};

#endif
