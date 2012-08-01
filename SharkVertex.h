#ifndef _3D_SHARK_VERTEX_STRUCTURE_TRACKER
#define _3D_SHARK_VERTEX_STRUCTURE_TRACKER

#include <cstdlib>
#include "Vector.h"

class SharkVertex
{
	public:
		Vector3f local;    
		Vector3f transformed;  //vertex after transformation
		Vector3f normal;
};

#endif

