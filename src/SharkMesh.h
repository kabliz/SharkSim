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

		SharkVertex *gVertex(Vector3f key){return vertices.find(key)->second;}	
		map<Vector3f, SharkVertex*, compareVect3>::iterator gVertices(){return vertices.begin();}
		map<Vector3f, SharkVertex*, compareVect3>::iterator gVerticesEnd(){return vertices.end();}
		void insertVec (pair< Vector3f, SharkVertex*> u ){vertices.insert(u);}

		Quad* gFace(int index){return faces[index];}
		//vector<Quad*> gFaces(){return faces;}
		void pushFace(Quad* f){faces.push_back(f);}
		vector<Quad*>::iterator gFaceBegin(){return faces.begin();}
		vector<Quad*>::iterator gFaceEnd(){return faces.end();}

		bool isTransformReady(){return hasNewTransform;}
		bool isUpdateApproved(){return newUpdateApproved;}
		void sNewTransform(bool n){hasNewTransform = n;}
		void sUpdateApproved(bool n){newUpdateApproved = n;}

	//private:
		map<Vector3f, SharkVertex*, compareVect3> vertices;
		vector<Quad*> faces;
		bool hasNewTransform;
		bool newUpdateApproved;
};

#endif
