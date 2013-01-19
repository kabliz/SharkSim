#include "Keyframe.h"

Keyframe::Keyframe(map<Vector3f, SharkVertex*, compareVect3> *rawVerts, vector<Quad*> *rawFaces)
{
	//deep copy verts over.
	map<Vector3f, SharkVertex*, compareVect3>::iterator im;
	for(im = rawVerts->begin(); im != rawVerts->end(); im++)
	{
		pair<Vector3f, SharkVertex*> serk = *im;
		SharkVertex * d = new SharkVertex();
		d->sNormal(serk.second->gNormal());
		d->sTransformed(serk.second->gTransformed());
		d->sLocal(serk.second->gLocal());
		uVertices.insert(pair<Vector3f, SharkVertex*>(serk.first, d));
	}

	//create faces.
	vector<Quad*>::iterator iq;
        for(iq = rawFaces->begin(); iq != rawFaces->end(); iq++)
        {
		Quad * nRect = new Quad();
		for(int i = 0; i < 4; i++)
		{
			nRect->sVert(i, uVertices.find((*iq)->gLocalVert(i))->second);
		}
		faces.push_back(nRect);
	}

	//setNormals
	createQuads();
}

void Keyframe::draw(void)
{
	vector<Quad*>::iterator iq;
	glBegin(GL_QUADS);
	int i = 0;
	for(iq = faces.begin(); iq < faces.end(); iq++)
	{
		glColor3f((*iq)->gBoneNo()*.1, 1.0-(((float) (i%3))/10.0), 1.0-((*iq)->gBoneNo()*.1));
		
		glNormal3f((*iq)->gNormalVert(0).x, (*iq)->gNormalVert(0).y, (*iq)->gNormalVert(0).z);
		glVertex3f((*iq)->gTransformedVert(0).x, (*iq)->gTransformedVert(0).y, (*iq)->gTransformedVert(0).z);
		
		glNormal3f((*iq)->gNormalVert(1).x, (*iq)->gNormalVert(1).y, (*iq)->gNormalVert(1).z);
		glVertex3f((*iq)->gTransformedVert(1).x, (*iq)->gTransformedVert(1).y, (*iq)->gTransformedVert(1).z);
		
		glNormal3f((*iq)->gNormalVert(2).x, (*iq)->gNormalVert(2).y, (*iq)->gNormalVert(2).z);
		glVertex3f((*iq)->gTransformedVert(2).x, (*iq)->gTransformedVert(2).y, (*iq)->gTransformedVert(2).z);
		
		glNormal3f((*iq)->gNormalVert(3).x, (*iq)->gNormalVert(3).y, (*iq)->gNormalVert(3).z);
		glVertex3f((*iq)->gTransformedVert(3).x, (*iq)->gTransformedVert(3).y, (*iq)->gTransformedVert(3).z);

		i++;
	}
	
	glEnd();	
}


/*Subroutine of transformHeiararchy(), this function calculates the position and rotation for a given bone segment and 
* generates quaderlaterals with those transformations */
void Keyframe::segmentMatrixMake( GLfloat segmentRot[], GLfloat segLength[], Mesh *mesh, int segments, glQuaternion *glQuat, int curSegment, 
		MyMat *stackMatrix, float start, float end, int isDownstream)
{
	double rotate = segmentRot[curSegment];                  //get rotation of current segment
	                                   //current shark model segment
	MyMat Matrix = *stackMatrix;
	MyMat transrix;
	transrix.makeTranslate(Vector3f((isDownstream == 2 ? segLength[curSegment] : -segLength[curSegment]), 0, 0));	
	MyMat secondStack = MyMat();
	GLfloat glm[16];
	
	glQuat->CreateFromAxisAngle(0,1,0,isDownstream == 2? -rotate : rotate);
	glQuat->CreateMatrix(glm); 
	MyMat rotatrix = MyMat(glm[0], glm[4], glm[8], glm[12], glm[1], glm[5],glm[9],
			glm[13],glm[2],glm[6],glm[10],glm[14],glm[3],glm[7],
			glm[11],glm[15]);

	Matrix = Matrix.multRight(rotatrix); //roatation goes before translates
	Matrix = Matrix.multRight(transrix); //doesnt matter which order translates are done

	*stackMatrix = Matrix; //advance heirarchy
	MyMat transrix2;
	transrix2.makeTranslate(Vector3f(isDownstream == 2? -end : -start, 0, 0));
	Matrix = Matrix.multRight(transrix2);
	//------------------------------------ 
	float center;
	for(int in = 0; in < mesh->vertCounter; in+=4)
	{
		//find center of face
		center = (mesh->vertList[in].x + mesh->vertList[in+1].x + mesh->vertList[in+2].x 
				+ mesh->vertList[in+3].x ) / 4;
		//draw if face is between start and end
		if(center >= start && center <= end )
		{
			Quad *curQuad = new Quad();
			curQuad->gNormal() = Vector3f(0,0,0);
			for(int corn = 0; corn < 4; corn++) //corner iteration
			{
				SharkVertex *curVert = new SharkVertex();   //current vertex
				curVert->local = mesh->vertList[in+corn];   //setting untransformed vertex
				curVert->transformed = Vector3f(Matrix.multVec(mesh->vertList[in+corn], true)); //setting transformed vertex
				curVert->normal = Vector3f(0,0,0);	//normal of the vertex, away from mesh			
				map<Vector3f, SharkVertex*, compareVect3>::iterator findTest 
					= uVertices.find(mesh->vertList[in+corn]);   //checking to see if vertex is already in the list.
				if(findTest == uVertices.end()) 
				{ 
					//vertex not in list. Add it to the list and to the quad.
					uVertices.insert(pair<Vector3f, SharkVertex*>(mesh->vertList[in+corn]
								, curVert));	
					curQuad->sVert(corn, curVert);
				}
				else   //vertex is in the list, so it's added to the existing quad.
				{
					delete curVert;
					curQuad->sVert(corn, (*findTest).second);
				}
				curQuad->gNormal() += Matrix.multVec(mesh->normals[in+corn], false);  //setting normal of the face
				
			}//end corners
			curQuad->gNormal() /= 4.0; //take average of normals to get the actual normal.
			curQuad->sBoneNo(curSegment); //record the bone this quad belongs to the most.
			faces.push_back(curQuad);
		}//end if
	}//end quads 

}

/*In two directions starting from the root of the heirarcical model, compute the matrix roatations for a particular bone, 
* then recurse down to the next bone.
* The initial call of this function should have isDownstream equal to 0, and it will fork into upstream and downstream versions 
* in recursive calls.
* It is a subroutine of gatherTransformData() */
void Keyframe::transformHeirarchy(int startingPoint, int isDownstream, GLfloat segmentRot[], GLfloat segLength[], Mesh *mesh, int segments, 
	glQuaternion *glQuat, int curSegment, MyMat newstack, float start, float end)
{
	segmentMatrixMake(segmentRot, segLength, mesh, segments, glQuat, curSegment, &newstack, start, end, isDownstream);

	if(isDownstream == 1 && startingPoint <= segments ) //going towards tail
	{
		transformHeirarchy(startingPoint+1, 1, segmentRot, segLength, mesh, segments, glQuat, curSegment+1, newstack, 
			start-segLength[curSegment+1], start);
	}
	else if(isDownstream == 2 && startingPoint > 0) //gowing towards head
	{
		transformHeirarchy(startingPoint-1, 2, segmentRot, segLength, mesh, segments, glQuat, curSegment-1, newstack,
			start, end+segLength[curSegment-1]);
	}
	else if(isDownstream == 0)//root node
	{
		//make two directions
		if(startingPoint > 0) //move forward
		{
			MyMat forwardtrans;
			forwardtrans.makeTranslate(Vector3f((segLength[curSegment-1]), 0, 0));	
			transformHeirarchy(startingPoint-1, 2, segmentRot, segLength, mesh, segments, glQuat, curSegment-1, 
				forwardtrans.multLeft(newstack),
				start, end+segLength[curSegment-1]);
		}
		if(startingPoint <= segments) //move backward
		{
			transformHeirarchy(startingPoint+1, 1, segmentRot, segLength, mesh, segments, glQuat, curSegment+1, newstack,
				start-segLength[curSegment+1], start);
		}
	}
} 
/* Parse raw rotation data into new keyframes for the shark. Locations of points and quads that make up the shark 
* will be set with this method
* Requires infomation in Shark class with the same names to be passed in. 
 */
void Keyframe::gatherTransformData(GLfloat segmentRot[], GLfloat segLength[], Mesh *mesh, int segments, glQuaternion *glQuat )
{
	int startingPoint = 3;

	float start = mesh->lengthMax, end = mesh->lengthMax;//, length =0;
	for(int i = 0; i <= startingPoint; i++)
	{
		end = start; 
		start -= segLength[i];
		//length += start;
	}
	MyMat stackMatrix = MyMat();
	stackMatrix.makeTranslate(Vector3f(-segLength[startingPoint], 0, 0));

	transformHeirarchy(startingPoint, 0, segmentRot, segLength, mesh, segments, 
			glQuat, startingPoint, stackMatrix, start, end);
	 
	createQuads();                    
}


/*Creates quaderlaterals that make up the shark, noting each quad's side faces */
void Keyframe::createQuads(void)
{
	vector<Quad*>::iterator iq;
	map<Vector3f, SharkVertex*, compareVect3>::iterator isv;
	//O(n squared operation)
	for(int i = 0; i < faces.size(); i++)
	//for(iq = faces.begin(); iq < faces.end(); iq++)
	{
		//(*iq)->locateAdjacants(faces);
		/*(*iq)->sNormalVert(0, (*iq)->gNormalVert(0) + (*iq)->gNormal());
		(*iq)->sNormalVert(1, (*iq)->gNormalVert(1) + (*iq)->gNormal());
		(*iq)->sNormalVert(2, (*iq)->gNormalVert(2) + (*iq)->gNormal());
		(*iq)->sNormalVert(3, (*iq)->gNormalVert(3) + (*iq)->gNormal());
		*/
		faces[i]->sNormalVert(0, faces[i]->gNormalVert(0) + faces[i]->gNormal());
		faces[i]->sNormalVert(1, faces[i]->gNormalVert(1) + faces[i]->gNormal());
		faces[i]->sNormalVert(2, faces[i]->gNormalVert(2) + faces[i]->gNormal());
		faces[i]->sNormalVert(3, faces[i]->gNormalVert(3) + faces[i]->gNormal());
	}

	for(isv = uVertices.begin(); isv != uVertices.end(); isv++)
	{
		(isv)->second->normal /= 4.0;
	}
}

/*Rigging: Slightly adjusts quads to make sections where gaps were closed in more smooted out */
void Keyframe::multiBoneAttenuate(void)
{
	//find all the quads whose front quadrant has a different bone number, 
	//and start labeling rings from the bone edge;
	vector<Quad*> quadRing;
	vector<Quad*>::iterator iq;
	int numEdges = 5;
	int attenK = -2;
	//initializing first ring.
	for(iq = faces.begin(); iq < faces.end(); iq++)
	{
		if(*iq != NULL && (*iq)->gBack() != NULL)
		{
			if((*iq)->gBoneNo() < (*iq)->gBack()->gBoneNo())
			{
				quadRing.push_back((*iq)->gBack());
			}
		}
	}

	//main attenuate loop
	for(int i = 1; i < numEdges; i++)
	{
		for(iq = quadRing.begin(); iq < quadRing.end(); iq++)
		{
			if((*iq) != NULL)
			{
				//move vertices 1 and 2 forward
				Vector3f diff1 = ((*iq)->gVert(0)->transformed) - ((*iq)->gVert(1)->transformed);
				Vector3f diff2 = ((*iq)->gVert(3)->transformed) - ((*iq)->gVert(2)->transformed);
				double attenVal = (*iq)->attenuate(numEdges, i, attenK);	
				((*iq)->gVert(1)->transformed) = ((*iq)->gVert(1)->transformed) + (diff1*attenVal);
				((*iq)->gVert(2)->transformed) = ((*iq)->gVert(2)->transformed) + (diff2*attenVal);
			}
		}
		//move to the next ring
		for(int j = 0; j < quadRing.size(); j++)
		{
			if(quadRing[j] != NULL && quadRing[j]->gBack() != NULL)
				quadRing[j] = quadRing[j]->gBack();
		}
	}
}

/*linear interpolation funcion */
Vector3f Keyframe::interpolateVertices(Vector3f first, Vector3f second, int step, int max)
{
	//TODO: replace Lerp with Slerp if necessary
	Vector3f fin = first;
	fin = fin + ((second-first)*((float)step))/((float)max);
	return fin;	
}

/*Draws in inbetweener frame given a target keyframe, its interpolation step and the total (max) interpolation steps
* This keyframe is the starting point 
* Outputs in OpenGL to the screen*/
void Keyframe::drawInBetween(Keyframe end, int step, int max)
{
	vector<Quad*>::iterator iq;
	Vector3f newCorn[4];
	Vector3f newNorm[4];
	glBegin(GL_QUADS);
	int i = 0;
	for(i = 0; i < faces.size(); i++)
	{
		newCorn[0] = interpolateVertices((faces[i]->gTransformedVert(0)), (end.faces[i]->gTransformedVert(0)), step, max);
		newCorn[1] = interpolateVertices((faces[i]->gTransformedVert(1)), (end.faces[i]->gTransformedVert(1)), step, max);
		newCorn[2] = interpolateVertices((faces[i]->gTransformedVert(2)), (end.faces[i]->gTransformedVert(2)), step, max);
		newCorn[3] = interpolateVertices((faces[i]->gTransformedVert(3)), (end.faces[i]->gTransformedVert(3)), step, max);
		
		newNorm[0] = interpolateVertices((faces[i]->gNormalVert(0)), (end.faces[i]->gNormalVert(0)), step, max);
		newNorm[1] = interpolateVertices((faces[i]->gNormalVert(1)), (end.faces[i]->gNormalVert(1)), step, max);
		newNorm[2] = interpolateVertices((faces[i]->gNormalVert(2)), (end.faces[i]->gNormalVert(2)), step, max);
		newNorm[3] = interpolateVertices((faces[i]->gNormalVert(3)), (end.faces[i]->gNormalVert(3)), step, max);

		glColor3f(faces[i]->gBoneNo()*.1, 1.0-(((float) (i%3))/10.0), 1.0-(faces[i]->gBoneNo()*.1));
		
		glNormal3f(newNorm[0].x, newNorm[0].y, newNorm[0].z);
		glVertex3f(newCorn[0].x, newCorn[0].y, newCorn[0].z);	
		
		glNormal3f(newNorm[1].x, newNorm[1].y, newNorm[1].z);
		glVertex3f(newCorn[1].x, newCorn[1].y, newCorn[1].z);
		
		glNormal3f(newNorm[2].x, newNorm[2].y, newNorm[2].z);
		glVertex3f(newCorn[2].x, newCorn[2].y, newCorn[2].z);	
	
		glNormal3f(newNorm[3].x, newNorm[3].y, newNorm[3].z);
		glVertex3f(newCorn[3].x, newCorn[3].y, newCorn[3].z);
	}
	glEnd();
}

/*Deallocate heap space before exiting the program */
void Keyframe::deleteHeap()
{
	map<Vector3f, SharkVertex*, compareVect3>::iterator iv;
	vector<Quad*>::iterator iq;

	for(iv = uVertices.begin(); iv != uVertices.end(); iv++)
	{
		delete (*iv).second;
	}
	for(iq = faces.begin(); iq < faces.end(); iq++)
	{
		delete (*iq);
	}
}
