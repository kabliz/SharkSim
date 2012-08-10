#ifndef _3D_SHARK_VERTEX_STRUCTURE_TRACKER
#define _3D_SHARK_VERTEX_STRUCTURE_TRACKER

#include <cstdlib>
#include "Vector.h"

class SharkVertex
{
	public:
		Vector3f gLocal(){return local;}
		Vector3f gTransformed(){return transformed;}
		Vector3f gNormal(){return normal;}
		void sNormal(Vector3f n){normal = n;}
		void sTransformed(Vector3f n){transformed = n;}
		void sLocal(Vector3f n){local = n;}

//	private:
		Vector3f local;    
		Vector3f transformed;  //vertex after transformation
		Vector3f normal;
};

#endif

