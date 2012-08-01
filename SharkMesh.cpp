#include "SharkMesh.h"

//TODO
void SharkMesh::deleteHeap()
{
	map<Vector3f, SharkVertex*, compareVect3>::iterator im;
	for(im = sVertices.begin(); im != sVertices.end(); im++ )
	{
		//delete each element
		delete im->second;
	}
}
