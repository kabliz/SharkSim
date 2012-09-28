#ifndef FRUSTUM_FOR_3D_RENDERING
#define FRUSTUM_FOR_3D_RENDERING

#include <GL/glut.h>
#include "Vector.h"

class Frustum
{
	public:
		Frustum(){}
		~Frustum(){}
		void extract();
		bool testPoint(Vector3f pt);
	
	private:
		double frustum[6][4];
};

#endif
