#include "SharkMesh.h"

string SharkMesh::nextToken(char delimit, FILE* readFile)
{
	string numb;
	while(!feof(readFile))
	{
		if(ferror(readFile)){ printf("1Error reading FILE\n"); exit(-1); }
		char cur = fgetc(readFile);
		if(ferror(readFile)){ printf("2Error reading FILE\n"); exit(-1);}
		if(cur == delimit || cur == '\n') {break;}
		numb.append(1, cur);
	}
	return numb;
}

//FILE* SharkMesh::buildAOBJ(string filename)
void SharkMesh::buildAOBJ(FILE* readFile)
{
	vector<Vector3f> localVerts = vector<Vector3f>();   //in order to use OBJs as keys, need to remember their order.
	while(!feof(readFile))
	{
		//tokenize the line identifier. It's only one character and 
		//it should be on the first on the line
		char identifier = fgetc(readFile);
		if(ferror(readFile)){ printf("888888888Error reading FILE\n"); exit(-1);}


		//load in the vertices
		//v x y z nx ny nz boneName/weight boneName/weight ...
		if(identifier == 'v')
		{
			char cur = fgetc(readFile);  //space
			if(ferror(readFile)){ printf("0Error reading FILE\n"); exit(-1);}
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
				sv->sNormal(nor*-1.0);  //TODO magic.  fix blender normals. shouldn't need to reverse them.

				sv->sTransformed(Vector3f(0,0,0));
				//bone / weight repeats
				fseek(readFile, -1, SEEK_CUR);
				cur = fgetc(readFile);
				while(cur != '\n')
				{
					string boneName = nextToken('/', readFile);
					float weight = atof(nextToken(' ', readFile).c_str());
					sv->sBonePair(boneName, weight);
					fseek(readFile, -1, SEEK_CUR);
					cur = fgetc(readFile);
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
			if(ferror(readFile)){ printf("4Error reading FILE\n"); exit(-1);}
			Quad *curQuad = new Quad();
			curQuad->sNormal(Vector3f(0,0,0));

			//caution. Vertices listed in mesh may not be consistant with other quads 
			int vertex1 = atoi(nextToken(' ', readFile).c_str());
			int vertex2 = atoi(nextToken(' ', readFile).c_str());
			int vertex3 = atoi(nextToken(' ', readFile).c_str());
			int vertex4 = atoi(nextToken(' ', readFile).c_str());

			curQuad->sVert(0, gVertex(localVerts[vertex1-1]));
			curQuad->sVert(1, gVertex(localVerts[vertex2-1]));
			curQuad->sVert(2, gVertex(localVerts[vertex3-1]));
			curQuad->sVert(3, gVertex(localVerts[vertex4-1]));
			curQuad->calcNormal();

			pushFace(curQuad);	
			//neighboring quads are to be found later, after parsing is done.

			fseek(readFile, -1, SEEK_CUR);
			cur = fgetc(readFile);
			while(cur != '\n')
			{
				cur = fgetc(readFile);
				if(ferror(readFile)){ printf("5Error reading FILE\n"); exit(-1);}
			}
		}		
		//bones
		//b name headRestArmature tailRestArmature ... child names ...
		//read outside of this function	
		else if(identifier == 'b')
		{
			return;
		}
	}
}


/*Restores vertex transformation back to rest pose (joint space) */
void SharkMesh::restPosition()
{
	map<Vector3f, SharkVertex*, compareVect3>::iterator im;
	for(im = vertices.begin(); im != vertices.end(); im++ )
	{
		im->second->transformed = im->second->local;
	}

	skinTransforms = map<string, MyMat>();  //wipe out bone information
}

/*Skinning transform of a quad with a transformation and the name of the bone that created that transformation.
 *  * Vertex weights taken from the bone name affect the strength of the  */
void SharkMesh::linearBlendTransform(MyMat matrix, string boneName)
{
	map<Vector3f, SharkVertex*, compareVect3>::iterator im;
	for(im = vertices.begin(); im != vertices.end(); im++ )
	{
		float weight = (*im).second->checkBone(boneName);
		if(weight > 0.001)
		{
			(*im).second->transformed += 
				matrix.multScalar(weight).multVec((*im).second->local, true);
		}
	}
}

/*Skinning transform of a quad with a transformation and the name of the bone that created that transformation.
 *  * Vertex weights taken from the bone name affect the strength of the  */
void SharkMesh::linearBlendTransform()
{
	map<Vector3f, SharkVertex*, compareVect3>::iterator im;
	for(im = vertices.begin(); im != vertices.end(); im++ )
	{
		string boneName;
		map<string, MyMat>::iterator ib;
		for(ib = skinTransforms.begin(); ib != skinTransforms.end(); ib++)
		{
			float weight = (*im).second->checkBone((*ib).first);
			if(weight > 0.001)
			{
				MyMat matrix = gSkinMatrix(ib->first); 
				(*im).second->transformed += 
					matrix.multScalar(weight).multVec((*im).second->local, true);
			}
		}
	}
}



void SharkMesh::countWeights()
{
	map<Vector3f, SharkVertex*, compareVect3>::iterator im;
	for(im = vertices.begin(); im != vertices.end(); im++ )
	{
		float weight = 0;
		for(int i = 0; i < im->second->gNumInfluences(); i++)
		{
			weight += im->second->gBoneWeight(i);
		}
		printf("%f\n", weight);
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
