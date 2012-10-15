#include "SharkBone.h"

/*Puts verticeis in the SharkMesh and puts the quads into the Bone 
 *The vertices created live in the heap space. The Quads and the Sharkmesh both point to them */
void SharkBone::buildBone(Mesh *mesh, float start, float end, MyMat multiplier)
{
	startB = start;
	endB = end;
	boneLength = end-start;
	for(int in = 0; in < mesh->vertCounter; in+=4)
	{
		//find center of face
		float center = (mesh->vertList[in].x + mesh->vertList[in+1].x + mesh->vertList[in+2].x
				+ mesh->vertList[in+3].x ) / 4;
		//draw if face is between start and end
		if(center >= start && center <= end )
		{
			Quad *curQuad = new Quad();
			curQuad->sNormal(Vector3f(0,0,0));
			for(int corn = 0; corn < 4; corn++) //corner iteration
			{
				SharkVertex *curVert = new SharkVertex();
				curVert->local = mesh->vertList[in+corn];
				curVert->transformed = Vector3f(0,0,0); // not transformed yet here
				curVert->normal = Vector3f(0,0,0); //normal is initially set to zero until it can be compared later. 
				map<Vector3f, SharkVertex*, compareVect3>::iterator findTest
					= sMesh->vertices.find(mesh->vertList[in+corn]);
				if(findTest == sMesh->vertices.end())  //this vertex isn't in the smart Mesh yet.
				{
					//uVertices.insert(pair<Vector3f, SharkVertex*>(mesh->vertList[in+corn]
					sMesh->vertices.insert(pair<Vector3f, SharkVertex*>(mesh->vertList[in+corn]
								, curVert));
					curQuad->sVert(corn, curVert);
				}
				else //vertex is in the smart mesh already, it just needs to be added to this quad.
				{
					delete curVert;
					curQuad->sVert(corn, (*findTest).second);
				}
				curQuad->sNormal(curQuad->gNormal() + multiplier.multVec(mesh->normals[in+corn], false));

			}//end corners
			curQuad->sNormal(curQuad->gNormal() / 4.0);
			curQuad->sBoneNo(boneNo); //curSegment;
			//faces.push_back(curQuad);
			quads.push_back(curQuad); //push to Bone
			sMesh->pushFace(curQuad); //push to Smart Mesh
		}//end if
	}//end quads
}

/*This changes the angles for this bone */
void SharkBone::changeAngle(int newAngle, bool isAheadRoot)
{
	angleOfRot = newAngle;

	GLfloat glm[16];

	rotatQ.CreateFromAxisAngle(0,1,0, isAheadRoot ? -angleOfRot : angleOfRot);
	rotatQ.CreateMatrix(glm);
	rotationMatrix = MyMat(glm[0], glm[4], glm[8], glm[12], glm[1], glm[5],glm[9],
			glm[13],glm[2],glm[6],glm[10],glm[14],glm[3],glm[7],
			glm[11],glm[15]);
}


/*Matrix multiplies the smark SharkMesh */
void SharkBone::transformBone(MyMat *stackMatrix, int isDownstream)
{
	//current shark model segment
	MyMat Matrix = *stackMatrix;
	MyMat transrix;
	transrix.makeTranslate(Vector3f((isDownstream == 2 ? boneLength : -boneLength), 0, 0));
	MyMat secondStack = MyMat();

	Matrix = Matrix.multRight(rotationMatrix); //roatation goes before translates
	Matrix = Matrix.multRight(transrix); //doesnt matter which order translates are done

	*stackMatrix = Matrix; //advance heirarchy
	MyMat transrix2;
	transrix2.makeTranslate(Vector3f((isDownstream == 2? -endB : -startB), 0, 0));
	Matrix = Matrix.multRight(transrix2); //not part of the heirarchy

	//map<Vector3f, SharkVertex*, compareVect3>::iterator im;
	vector<Quad*>::iterator iq;

	for(iq = quads.begin(); iq < quads.end(); iq++)
	{
		(*iq)->matrixTransform(Matrix);
	 	/*(*iq)->verts[0]->transformed = Vector3f(Matrix.multVec((*iq)->verts[0]->local, true));
	 	(*iq)->verts[3]->transformed = Vector3f(Matrix.multVec((*iq)->verts[3]->local, true));
	 	
		(*iq)->verts[1]->transformed = Vector3f(Matrix.multVec((*iq)->verts[1]->local, true));
	 	(*iq)->verts[2]->transformed = Vector3f(Matrix.multVec((*iq)->verts[2]->local, true));*/
	}

}
