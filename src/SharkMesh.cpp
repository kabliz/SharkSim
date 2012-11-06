#include "SharkMesh.h"

string SharkMesh::nextToken(char delimit, FILE* readFile)
{
	char numb[32];
	int k = 0;
	while(!feof(readFile))
	{
		char cur = fgetc(readFile);
		if(cur == delimit) {break;}
		numb[k] = cur;
		k++;
	}
	numb[k] = '\0';
	return string(numb);
}

FILE* SharkMesh::buildAOBJ(string filename)
{

	//open file.
	FILE* readFile;
	readFile = fopen(filename.c_str(), "r");
	if(readFile == NULL)
	{
		printf(".aobj file not found\n");
		exit(-1);
	}

	while(!feof(readFile))
	{
		//tokenize the line identifier. It's only one character and 
		//it should be on the first on the line
		char identifier = fgetc(readFile);

		vector<Vector3f> localVerts;   //in order to use OBJs as keys, need to remember their order.

		//load in the vertices
		//v x y z nx ny nz boneName/weight boneName/weight ...
		if(identifier == 'v')
		{
			char cur = fgetc(readFile);  //space
			while(cur != '\n')  //per line
			{
				SharkVertex* sv = new SharkVertex();

				//location of vertex    
				Vector3f vert;
				vert.x = atof(nextToken(' ', readFile).c_str());
				vert.y = atof(nextToken(' ', readFile).c_str());
				vert.z = atof(nextToken(' ', readFile).c_str());
				sv->sLocal(vert);

				Vector3f nor; //normal
				nor.x = atof(nextToken(' ', readFile).c_str());
				nor.y = atof(nextToken(' ', readFile).c_str());
				nor.z = atof(nextToken(' ', readFile).c_str());
				sv->sNormal(nor);

				sv->sTransformed(Vector3f(0,0,0));
				//bone / weight repeats
				while(cur != '\n')
				{
					string boneName = nextToken('/', readFile);
					float weight = atof(nextToken(' ', readFile).c_str());
					sv->sBonePair(boneName, weight);
				}
				localVerts.push_back(vert);
				insertVec(pair<Vector3f, SharkVertex*>(vert, sv));
			}
		} 
		//faces
		//f ... vertices in mesh .....
		else if(identifier == 'f')
		{
			char cur = fgetc(readFile); //space

			Quad *curQuad = new Quad();

			//caution. Vertices listed in mesh may not be consistant with other quads 
			int vertex1 = atoi(nextToken(' ', readFile).c_str());
			int vertex2 = atoi(nextToken(' ', readFile).c_str());
			int vertex3 = atoi(nextToken(' ', readFile).c_str());
			int vertex4 = atoi(nextToken(' ', readFile).c_str());
			
			curQuad->sVert(0, gVertex(localVerts[vertex1]));
			curQuad->sVert(1, gVertex(localVerts[vertex2]));
			curQuad->sVert(2, gVertex(localVerts[vertex3]));
			curQuad->sVert(3, gVertex(localVerts[vertex4]));

			pushFace(curQuad);	
			//neighboring quads are to be found later, after parsing is done.

			while(cur != '\n')
			{
				cur = fgetc(readFile);
			}
		}		

		//bones
		//b name headRestArmature tailRestArmature ... child names ...
		else if(identifier == 'b')
		{
			//this mesh doesn't read any bones, so it's got to pass the file reader back
			return readFile;
		}
		else
		{
			//something blew up
			return 0;
		}
	}
}


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
