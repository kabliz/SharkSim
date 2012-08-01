#ifndef D_SHARK_MESH_VERTEX_QUAD
#define D_SHARK_MESH_VERTEX_QUAD

/*This is a "smart mesh" structure that remembers unique vertices. The other mesh structure is to read blender files with */
#include <cstdlib>
#include <vector>
#include <map>

#include "SharkVertex.h"
#include "Quad.h"

/*Compares two vertices to make sure they don't already exist in the mesh. */
struct compareVect3
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
};

using namespace std;
class SharkMesh
{
	public:
		SharkMesh(){ }
		~SharkMesh(){}
		void deleteHeap(); 

		map<Vector3f, SharkVertex*, compareVect3> sVertices;
		vector<Quad*> sFaces;
		bool hasNewTransform;
		bool newUpdateApproved;
};

#endif
