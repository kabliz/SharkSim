#include "Mesh.h"

Mesh::Mesh()
{
	this->vertCounter = 0;
	this->blenderTotalLength = 0;
	this->lengthMin = 1000;
	this->lengthMax = -1000;	
	this->heightMax = -1000;
	this->heightMin = 1000;
	this->widthMax = -1000;
	this->widthMin = 1000;
}

void Mesh::readBlender(const char *file)
{
	  FILE * pFile;
	  pFile = fopen (file, "r");
	  printf("opening file: %s\n", file);
	  while(!feof(pFile)){
		  fscanf (pFile, "%f %f %f ", &vertList[vertCounter].x, &vertList[vertCounter].y, &vertList[vertCounter].z);
		  //find boundaries of model with max/min (used to translate/scale model correctly relative to Blender coords)

		  if(vertList[vertCounter].x > lengthMax)
			  lengthMax = vertList[vertCounter].x;
		  else if(vertList[vertCounter].x < lengthMin)
			  lengthMin = vertList[vertCounter].x;

		  if(vertList[vertCounter].y > heightMax)
			  heightMax = vertList[vertCounter].y;
		  else if(vertList[vertCounter].y < heightMin)
			  heightMin = vertList[vertCounter].y;

		  if(vertList[vertCounter].z > widthMax)
			  widthMax = vertList[vertCounter].z;
		  else if(vertList[vertCounter].z < widthMin)
			  widthMin = vertList[vertCounter].z;

		  vertCounter++;
	  }
	  
	  //when done, store segment length
	  blenderTotalLength = (lengthMax - lengthMin);
	  fclose (pFile);
}

void Mesh::init_blender()
{
	//TODO cross platform compilation/directories
	readBlender("Raw/bettershark6.raw");
	readBlender("Raw/bettershark5.raw");
	readBlender("Raw/bettershark4.raw");
	readBlender("Raw/bettershark3.raw");
	readBlender("Raw/bettershark2.raw");
	readBlender("Raw/bettershark1.raw");
	vertCounter -= 1;
	printf("min len: %lf\n", lengthMin);
	printf("max len: %lf\n", lengthMax);
	calculateNormals();
}

void Mesh::draw()
{
	glPushMatrix();
	glBegin(GL_QUADS);
	for(int i = 0; i < vertCounter; i++)
	{
		glNormal3f(normals[i].x, normals[i].y, normals[i].z);
		glVertex3f(vertList[i].x, vertList[i].y, vertList[i].z);
	}
	glEnd();
	glPopMatrix();
}

void Mesh::drawBetween(float start, float end)
{
	glPushMatrix();
	glBegin(GL_QUADS);
	float center;
	float red = 1.0;
	float green = 0.0;
	float blue = 0.0;
	for(int i = 0; i < vertCounter; i+=4)
	{
		//find center of face
		center = (vertList[i].x + vertList[i+1].x + vertList[i+2].x + vertList[i+3].x ) / 4;
		//draw if face is between start and end
		if(center >= start && center <= end )
		{
			glColor3f(red, green, blue);
			
			//Need to move vertex over from transform
			glNormal3f(normals[i].x, normals[i].y, normals[i].z);
			glVertex3f(vertList[i].x, vertList[i].y, vertList[i].z);

			glNormal3f(normals[i+1].x, normals[i+1].y, normals[i+1].z);
			glVertex3f(vertList[i+1].x, vertList[i+1].y, vertList[i+1].z);

			glNormal3f(normals[i+2].x, normals[i+2].y, normals[i+2].z);
			glVertex3f(vertList[i+2].x, vertList[i+2].y, vertList[i+2].z);

			glNormal3f(normals[i+3].x, normals[i+3].y, normals[i+3].z);
			glVertex3f(vertList[i+3].x, vertList[i+3].y, vertList[i+3].z);
			green += .02;
		}
	}
	glEnd();
	glPopMatrix();
}

void Mesh::calculateNormals() {
   Vector3f vectorA, vectorB, normal;
   int i;
   for ( i = 0; i < vertCounter; i+=4 ) {
      vectorA = vertList[i+1] - vertList[i];
	  vectorB = vertList[i+3] - vertList[i];
	  normal = vectorB.Cross( vectorA );
	  normal = normal.Normalize();
	  normals[i] += normal;
	  normals[i+1] += normal;
	  normals[i+2] += normal;
	  normals[i+3] += normal;
   }
}
