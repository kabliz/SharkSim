#include "Quad.h"

//.dont know why shark breaks up in unknown places when transformed
//unsure that x and y are the hieght and lenght numbers. hoping z is the shark's width. 
// axis alighed surface problem? nah

Vector3f Quad::calcNormal()
{
	Vector3f vectorA, vectorB, normal;
	int i;
	/*for ( i = 0; i < vertCounter; i+=4 ) 
	{
	       vectorA = vertList[i+1] - vertList[i];
	       vectorB = vertList[i+3] - vertList[i];
	       normal = vectorB.Cross( vectorA );
	       normal = normal.Normalize();
	       normals[i] += normal;
	       normals[i+1] += normal;
	       normals[i+2] += normal;
	       normals[i+3] += normal;
	}*/
	return normal;
}

double Quad::attenuate(int numedges, int iteration, int k)
{
	double core = ((double)iteration)/(((double)numedges) + 1.0);
	double newk = (k >= 0 ? k : -k) + 1.0;
	if(k >= 0)
	{
		return 1 - pow(core, newk);
	}
	return pow(1-core, newk);
}

void Quad::locateAdjacants(vector<Quad*> lisq)
{
	vector<Quad*>::iterator iq;
	for(iq = lisq.begin(); iq < lisq.end(); iq++)
	{
		if(compareFront(*(*iq)))
		{
			front = *iq;
		}
		else if(compareBack(*(*iq)))
		{
			back = *iq;
		}
		else if(compareLeft(*(*iq)))
		{
			left = *iq;
		}
		else if(compareRight(*(*iq)))
		{
			right = *iq;
		}
	}
}




bool Quad::compareFront(Quad oth)
{
	//compares edges 0 and 3, if not, switch the function names
	bool topmatch = gLocalVert(0).fEquals(oth.gLocalVert(1), .00001);	
	bool bottmatch = gLocalVert(3).fEquals(oth.gLocalVert(2), .00001);	
	
	return topmatch && bottmatch;
}

bool Quad::compareBack(Quad oth)
{
	//compares edges 1 and 2
	bool topmatch = gLocalVert(1).fEquals(oth.gLocalVert(0), .00001);
	bool bottmatch = gLocalVert(2).fEquals(oth.gLocalVert(3), .00001);
	
	return topmatch && bottmatch;
}

bool Quad::compareLeft(Quad oth)
{
	//compares edges 2 and 3
	bool topmatch = gLocalVert(2).fEquals(oth.gLocalVert(1), .00001);
	bool bottmatch = gLocalVert(3).fEquals(oth.gLocalVert(0), .00001);
	
	return topmatch && bottmatch;
}

bool Quad::compareRight(Quad oth)
{
	//compares edges 0 and 1
	bool topmatch = gLocalVert(0).fEquals(oth.gLocalVert(3), .00001);
	bool bottmatch = gLocalVert(1).fEquals(oth.gLocalVert(2), .00001);
	
	return topmatch && bottmatch;
}

/*resets all vertex transforms to the local (rest) position */
void Quad::restPosition()
{
	for(int i = 0; i < 4; i++)
	{
		verts[i]->transformed = verts[i]->local;
	}
}

/*Multiplies the local value of this quad by a transformation matrix, and sets the transformed value of this quad.
 * Transformation is Rigid body (absolute) and does not consider vertex weights */
void Quad::matrixTransform(MyMat matrix)
{
	verts[0]->transformed = Vector3f(matrix.multVec(verts[0]->local, true));
	verts[3]->transformed = Vector3f(matrix.multVec(verts[3]->local, true));

	verts[1]->transformed = Vector3f(matrix.multVec(verts[1]->local, true));
	verts[2]->transformed = Vector3f(matrix.multVec(verts[2]->local, true));
}

/*Skinning transform of a quad with a transformation and the name of the bone that created that transformation.
 * Vertex weights taken from the bone name affect the strength of the  */
void Quad::linearBlendTransform(MyMat matrix, string boneName)
{
	for(int i = 0; i < 4; i++)
	{
		float weight = verts[i]->checkBone(boneName);
		if(weight > 0)
		{
			verts[i]->transformed = verts[i]->transformed + 
						(Vector3f(matrix.multVec(verts[i]->local, true))*weight);
		}
	}
}

/*Returns the weight on the first vertex that matches this bone */
float Quad::checkBone(string boneName)
{
	double weight;
	for(int i = 0; i < 4; i++)
	{
		weight = verts[i]->checkBone(boneName);
		if(weight > 0){return weight;}
	}
	return -1;
}








