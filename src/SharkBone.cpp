#include "SharkBone.h"

/* Builds a mesh from CalShark
 * Puts verticeis in the SharkMesh and puts the quads into the Bone 
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

/*builds a bone from an aobj file. Copies over the vertices. Does not set child bones; That has to be done separate 
 * The shark mesh should be loaded already*/
void SharkBone::buildBoneAOBJ(string bName, Vector3f headpt, Vector3f tailpt )
{
	boneName = bName;
	headPoint = headpt;
	tailPoint = tailpt;

	//length of bone
	boneLength = headpt.EuclDistance(tailpt);
	
	//get rotation
	Vector3f boneVec = headpt - tailpt;
	//Vector3f boneVec = tailpt - headpt;
	Vector3f rot = boneVec.Normalize();
	float theta = acos(rot.Dot(Vector3f(1,0,0)));
	glQuaternion qr = glQuaternion(); 
	
	qr.CreateFromAxisAngle(0, 1, 0, theta);
        changeAngle(qr);	
}


/*creates translation matrices for this bone and all child bones */
void SharkBone::buildTranslation(Vector3f root, Vector3f headL, Vector3f tailL)
{
	Vector3f locTrans = (headPoint - (headPoint-tailPoint))*-1;
	transMatLocal.makeTranslate(locTrans);   //skin space to joint space
	
	Vector3f transHeir = headPoint-tailPoint;
	transHeir = transHeir * -1;
	transMatHeir.makeTranslate(transHeir);   //joint space to shark space

	vector<SharkBone*>::iterator ic;
	for(ic = childBones.begin(); ic != childBones.end(); ic++)
	{
		(*ic)->buildTranslation(root, headPoint, tailPoint);
	}
}

/*Creates a translation matrix from the bonelength and direction. 
 *Because this is the CalShark loader, the translation would be in the x direction only. */
void SharkBone::boneLengthToTranslation(bool downstream)
{
	double xtrans = downstream ? boneLength : -boneLength;
	double xend = downstream ? -endB : -startB;	//start and end points on the bone.

	transMatHeir.makeTranslate(Vector3f(xtrans, 0, 0));
	transMatLocal.makeTranslate(Vector3f(xend, 0, 0));
}

/*This changes the angles for this bone.
 * This is the cal shark version, that needs to know whether the bone is ahead of the root */
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

/*Change the angle of this bone.
 * This is the general case of this function. Cal Shark needs the other one. */
void SharkBone::changeAngle(int newAngle)
{
	changeAngle(newAngle, false);
}

/*sets the rotation based on an existing quaternion */
void SharkBone::changeAngle(glQuaternion newAngle)
{
	GLfloat glm[16];
	newAngle.CreateMatrix(glm);	
	rotationMatrix = MyMat(glm[0], glm[4], glm[8], glm[12], glm[1], glm[5],glm[9],
			glm[13],glm[2],glm[6],glm[10],glm[14],glm[3],glm[7],
			glm[11],glm[15]);
}


/*creates the matrices. Advances matrix hierarchy and derives the combined heirarchical matrix and binding matrix
 * returns: the binding matrix that is not tied tot he hierarchy */
MyMat SharkBone::createTransforms(MyMat *stackMatrix)
{
	//current shark model segment
	MyMat Matrix = *stackMatrix;
	MyMat secondStack = MyMat();

	Matrix = Matrix.multRight(jointTrans);
	Matrix = Matrix.multRight(rotationMatrix); //roatation goes before translates
	Matrix = Matrix.multRight(transMatHeir); 
	
	*stackMatrix = Matrix; //advance heirarchy, without applying the below local translation to the whole stack
	Matrix = Matrix.multRight(transMatLocal);

	return Matrix;
}

/*Matrix multiplies the smark SharkMesh
 * pass down the hierarchical model matrix and a value  */
void SharkBone::transformBone(MyMat *stackMatrix, bool rigidBody)
{
	//current shark model segment
	MyMat Matrix = createTransforms(stackMatrix); 

	if(rigidBody){ 
		vector<Quad*>::iterator iq;
		//transform each quad in the mesh
		for(iq = quads.begin(); iq < quads.end(); iq++)
		{
			(*iq)->matrixTransform(Matrix);
			
		}
	}
	else {
		sMesh->linearBlendTransform(Matrix, boneName);
	}

	//recursive transform downwards to child bones. 
	//The matrix is copied so that child changes don't propagate upstream  
	vector<SharkBone*>::iterator ib; 
	for(ib = childBones.begin(); ib != childBones.end(); ib++ )
	{
		MyMat tmp = MyMat(*stackMatrix);	
		(*ib)->transformBone(&tmp, rigidBody);
	}

}

void SharkBone::drawTri(MyMat matrix)
{
	Vector3f perpen = headPoint.Cross(tailPoint);
	perpen = perpen / perpen.Magnitude();
	Vector3f tailV = Vector3f(matrix.multVec(tailPoint, true));
	Vector3f headVa = Vector3f(matrix.multVec(headPoint, true)) + (perpen*.15);
	Vector3f headVb = Vector3f(matrix.multVec(headPoint, true)) - (perpen*.15);

	glColor3f(0,1.0,0);
	glBegin(GL_TRIANGLES);
	{
		glVertex3f(tailV.x, tailV.y, tailV.z);	
		glVertex3f(headVa.x, headVa.y, headVa.z);	
		glVertex3f(headVb.x, headVb.y, headVb.z);	
	} glEnd();
	glColor3f(1,1,1);

}

void SharkBone::draw(MyMat *stackmatrix)
{
	MyMat Matrix = createTransforms(stackmatrix);

	//current shark model segment
	drawTri(Matrix);

	//recursive transform downwards to child bones. 
	//The matrix is copied so that child changes don't propagate upstream  
	vector<SharkBone*>::iterator ib; 
	
	for(ib = childBones.begin(); ib != childBones.end(); ib++ )
	{
		MyMat tmp = MyMat(*stackmatrix);	
		(*ib)->draw(&tmp);
	}
}
