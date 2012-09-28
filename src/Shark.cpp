#include "Shark.h"

Shark::Shark()
{
	this->showSkin = true;
	this->showSpine = false;
	this->ismoving = false;
	this->play = 0;

	this->segPercent = 0;
	this->totalLength = 4;
	this->lengthLeft = 4;

	this->tempSegments = 10;
	this->segNum = 1;
	this->tempSegPercent = 0;

	mesh.init_blender(); //global var
	this->totalLength = mesh.blenderTotalLength;
}

materialStruct RedFlat = {
  {0.3f, 0.0f, 0.0f, 1.0f},
  {0.9f, 0.0f, 0.0f, 1.0f},
  {0.0f, 0.0f, 0.0f, 1.0f},
  {0.0f}
};
materialStruct White = {
  {1, 1, 1, 1},
  {1, 1, 1, 1},
  {1, 1, 1, 1},
  {0.0}
};
materialStruct Black = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0.0}
};
materialStruct GreenFlat = {
  {0.0f, 0.3f, 0.0f, 1.0f},
  {0.0f, 0.9f, 0.0f, 1.0f},
  {0.0f, 0.0f, 0.0f, 1.0f},
  {0.0f}
};
materialStruct BlueFlat = {
  {0.0f, 0.0f, 0.3f, 1.0f},
  {0.0f, 0.0f, 0.9f, 1.0f},
  {0.0f, 0.0f, 0.0f, 1.0f},
  {0.0f}
};
materialStruct PurpleFlat = {
  {0.3f, 0.0f, 0.3f, 1.0f},
  {0.9f, 0.0f, 0.9f, 1.0f},
  {0.0f, 0.0f, 0.0f, 1.0f},
  {0.0f}
};


materialStruct Grey = {
  {0.25f, 0.25f, 0.25f, 0.25f},
  {0.25f, 0.25f, 0.25f, 0.25f},
  {0.25f, 0.25f, 0.25f, 0.25f},
  {0.0}
};

void Shark::drawSharkSeg(float rotate, float start, float end, int index, float length)
{
	GLfloat Matrix[16];

	glQuat.CreateFromAxisAngle(0,1,0,rotate);
	glQuat.CreateMatrix(Matrix);
	glMultMatrixf(Matrix);
	//glRotatef(rotate, 0, 1, 0);  //commented out when i found this
	
	glTranslatef(-segLength[index], 0, 0);
	glPushMatrix();
		glTranslatef(-start, 0, 0);
		mesh.drawBetween(start, end);
	
	glPopMatrix();
	//glTranslatef(-segLength[index], 0, 0); wrong order
}

KeyframeSystem Shark::genKeyframes(bool dynamicMode, SharkMesh *shm)
{
	if(dynamicMode)
	{
		kfSys = KeyframeSystem(true);
		kfSys.setSharkMesh(shm);
		//animation sequence #1 is the slow swim. TODO, less magic nubmers
		int numAngles = segments;
		skeleton.buildAnimations(numAngles, *(segmentRot[1]), numAngles);
		skeleton.update(0); //force one update cycle to happen before startup, initializes the system this way.
		kfSys.initFrames();
	}
	else
	{
		kfSys = KeyframeSystem(false);

		int segs = segments;
		
		for(int k = 0; k < sequencesParsed; k++)
		{
			GLfloat segRot2[segments];
			FrameSequence curSeq;
			for(int j = 0; j < segments; j++)
			{
				Keyframe curFrame = Keyframe();

				for(int i = 0; i < segments;i++)
				{
					segRot2[i] = segmentRot[k][i][j];
				}
				curFrame.gatherTransformData(segRot2, 
					segLength, 
					&mesh, 
					segs, 
					&glQuat);
				curSeq.sFrame(curFrame);
			}
			kfSys.insertStaticSequence(curSeq);
		}
	}
	
	return kfSys;	
}

void Shark::drawSkin(int frame)
{
	float startSeg = mesh.lengthMax, endSeg = mesh.lengthMax, length = 0;
	GLfloat rotate;
	materials(Grey);
	glPushMatrix();

		
	if(showSkin && !(ismoving || play))  //if we are drawing the skin
	{
		
		glTranslatef(segLength[0], 0, 0); //segment drawing moves the whole mesh back a space
		for ( int i = 0; i < segments; i++)
		{
			if(ismoving || play)									//if the shark is moving
				rotate = segmentRot[kfSys.gCurrentSequence()][i][frame];			//get rotation of current segment
			else
				rotate = 0;

			endSeg = startSeg;							//get proper drawing coordinates for
			startSeg -= segLength[i];					//current shark model segment
			
			glPushMatrix();
				
				drawSharkSeg(rotate, startSeg, endSeg, i, length);	//draw segment
			length += segLength[i];						//increase offset to draw segment back
														//to head location
		}
		
		for(int i = 0; i < segments; i++)
			glPopMatrix();
		
	}
	else if(showSkin && ismoving)
	{
		kfSys.draw();
	}
	else if(showSkin && play)
	{
		//kfSys.sequences[0].drawKeyFrame(frame); //TODO
	}
	
	
	glPopMatrix();
}

void Shark::drawSegment(int index, float rotate, GLUquadricObj *quadratic)
{
	GLfloat Matrix[16];

	glQuat.CreateFromAxisAngle(0,1,0,rotate);	//convert rotation to quaternion
	glQuat.CreateMatrix(Matrix);
	glMultMatrixf(Matrix);
	//glRotatef(rotate, 0, 1, 0);  //commented out when code was handed off
	
	gluCylinder(quadratic, .1, .1, segLength[index], 5, 5);
	glTranslatef(0, 0, segLength[index]);	

}

void Shark::drawSpine(int frame, GLUquadricObj *quadratic)
{
	float rotate;
	materials(Grey);
	glPushMatrix();
	glTranslatef(.4, 0, 0);		//draw on top of skin
	glRotatef(-90, 0, 1, 0);
	if(showSpine)
	{
		gluSphere(quadratic, .1, 5, 5);
		for ( int i = 0; i < segments; i++)
		{
			if(i % 2)
				materials(White);
			else
				materials(RedFlat);

			if(ismoving || play)			//if the shark is moving
				rotate = segmentRot[kfSys.gCurrentSequence()][i][frame];	//get rotation of current segment
			else
				rotate = 0;						

			glPushMatrix();
			drawSegment(i, rotate, quadratic);	
		}
		materials(Grey);
		gluSphere(quadratic, .1, 5, 5);

		for(int i = 0; i < segments; i++)
			glPopMatrix();				//pop for every push done in the loop above
	}							//recursive calling of this function would be prettier code
	
	materials(White);
	glPopMatrix();
}

void Shark::drawShark(int frame, GLUquadricObj *quadratic) 
{
	drawSpine(frame, quadratic); //TODO, fix spine so it interpolates too
	drawSkin(frame);
}

/*The general update function that is to be called regularly and predicitibly.
 * It needs to know what the curvature of the path is. */
//void Shark::timedUpdate(int railAngle)
void Shark::timedUpdate(int railAngle)
{
	kfSys.update();
	skeleton.update(railAngle); 
}

void Shark::materials(materialStruct materials) {
  glMaterialfv(GL_FRONT, GL_AMBIENT, materials.ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, materials.diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, materials.specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, materials.shininess);
}

double doubleLerpFrames(double input, double minx, double maxx)
{
        double maxy = 20.0;
        return 4.0 + (input-minx) * ((maxy-4.0)/(maxx-minx));
}


void Shark::updateVelocity(Vector3f start, Vector3f end, double dt)
{
	double dist = (end-start).Magnitude();
	double unitTime = dt / dist;
	int Lframe = doubleLerpFrames(unitTime, 0.1, 10.0); +1;

	kfSys.adjNumFrames(Lframe);
	//printf("%f\n", unitTime);

}

/* parses CSV files containing animation data. Not all CSV's are animation. Some of them are world data points. */
void Shark::readMovementData(const char* file, bool dynaMode)
{
        FILE * fp;
        fp = fopen(file, "r");
        if(fp == NULL)
        {
                printf("can't open file %s\n", file);
                exit(-1);
        }
        for(int i = 0; i < segments; i++)
        {
                for(int j = 0; j < segments; j++)
                {
			float segr;
                        //read in file. Note that CALShark writes segment data (j value) backwards
                        if(!dynaMode)
                        {
                                fscanf(fp, "%f,", &segmentRot[gParsedSoFar()][i][segments-1-j]);
                        }
                        else
                        {
                                fscanf(fp, "%f,", &segmentRot[gParsedSoFar()][j][segments-1-i]);
                        }
                }
        }
        fclose (fp);
        incrementSequences();
}

/*default shark animation sequence creator. Makes a shark that stays stiff */
void Shark::defSequence()
{
        for(int i = 0; i < 2; i++)
        {
                for(int j = 0; j < segments; j++)
                {
                        segmentRot[0][i][j] = 0;
                }
        }
        incrementSequences();

}



/* Used to update the segments of the shark spine when using the glui interface
 * Does appropriate checking to keep the segments within desired bounds.
 */
void Shark::segUpdate()
{
	double checkLength = 0;
	double tempPercent = tempSegPercent;					//convert tempSegPercent to double for accurate division
	double percentSpecified = 0;					  	//tracker for printing
																	
	for(int i = 0; i < tempSegments; i++)				//check to make sure specific lengths don't exceed maximum length
	{
	if(i == segNum - 1)						//if check is equal to current segment being updated
		{
			checkLength += totalLength * (tempPercent/100);	//update checkLength with new percent converted to actual length
			percentSpecified += totalLength * (tempPercent/100);	//add to printed value of percent specified
		}
		else 
		{
			if(segLengthInput[i] == 0)				//if segment has no user specified length
				checkLength += totalLength * .01;		//each segment must be atleast 1% long
			else
			{
				checkLength += segLengthInput[i];		//add specified length to checkLength
				percentSpecified += segLengthInput[i];
			}
		}
	}

	//printf("Total percent specified: %3.2lf\n", (percentSpecified/totalLength)*100);

	if(checkLength <= totalLength)						//user inputed lengths check to be OK
	{
		//Shark.segments = tempSegments;				//update amount of segments in spine
		segPercent = tempSegPercent;					//udate segment percent
		segLengthInput[segNum-1] = totalLength * (segPercent/100);	//update segment length array with new value (actual length)

		lengthLeft = totalLength;					//reset lengthLeft
		for(int j = 0; j < segments; j++)				//update lengthLeft
		{
			if(segLengthInput[j] != 0)				//If user inputed data exists
				lengthLeft -= segLengthInput[j];		//add the length specified
		}

		for(int k = 0; k < segments; k++)				//update drawing array segments without a specific length
		{
			if(segLengthInput[k]== 0)				//if segment has no user specified length
			   segLength[k] = lengthLeft / segments;		//divide the length left by the amount of segments used 
			else
				segLength[k] = segLengthInput[k];		//set the specified length
		}
	}
	else
		printf("User generated lengths plus minimum segment lengths exceed maximum length\n");

	glutPostRedisplay();
}



