#include "SharkMesh.h"

//TODO delete Heap properly.
void SharkMesh::deleteHeap()
{
	map<Vector3f, SharkVertex*, compareVect3>::iterator im;
	for(im = vertices.begin(); im != vertices.end(); im++ )
	{
		//delete each element
		delete im->second;
	}
}
