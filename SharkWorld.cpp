#include "SharkWorld.h"


double doubleLerp(double input, double minx, double maxx, double miny, double maxy)
{
	return miny + (input-minx) * ((maxy-miny)/(maxx-minx));
}


/*simple linear interpolation *
Vector3f SharkWorld::interpolateVertices(Vector3f first, Vector3f second, int step, int max) 
{ 
        Vector3f fin = first; 
        fin = fin + ((second-first)*((float)step))/((float)max); 
        return fin;      
}*/

/* Initializes the tangent list 
   requires a points list of at least three points long
   make whole points list before calling
* */
//void SharkWorld::initialize(string splineFilename)
//{
//	traveler.initSpline(splineFilename);
	//path.parseDataFile("xytData.mat");
	//path.gatherDTPoints();
	//path.paramaterizeSpline(); 
//}

/*Looks at the curvature of the rail path, and return an angle representing the current angle *
int SharkWorld::deriveRailAngle()
{
	float lookAhead = .15; 
	int aheadPoint = curPoint;
	float pu = totalSteps == 0 ? 0 : ((float) steps) /(float) totalSteps;  
	if(pu+lookAhead > 1) {
		  pu = pu+lookAhead-1.0;
		  aheadPoint++; 
	} else {
		 pu+lookAhead; 
	}
	
	Vector3f pastpoint = path.getNearbyPoint(-.3, aheadPoint, pu);
	Vector3f futurepoint = path.getNearbyPoint(.3, aheadPoint, pu);
	Vector3f thispoint = path.splineLocation (pu,  aheadPoint);

	Vector3f firstBranch = (thispoint - pastpoint).Normalize();
	Vector3f secondBranch = (futurepoint - thispoint).Normalize();

	//arcos dot product/productof magnitudes = angle in radians
	float railAngle;
	railAngle = atan2(secondBranch.x, secondBranch.z) - atan2(firstBranch.x, firstBranch.z);
	if(railAngle > 3.14159265) { railAngle -= 3.14195265; }
	if(railAngle < -3.14159265) { railAngle += 3.14195265; }
	railAngle *= 180/3.14159265 * -1;
	return (int)railAngle;
}*/

/*returns a string based on how wide the next turn is *
//depreciated
std::string SharkWorld::getSharkTurn()
{
	double angle = deltaTheta.x*180/3.1415926;
	//printf("%f", totalSteps*.9);
	if(totalSteps*.9 > steps) //if not 90% progress yet, don't change turn yet.
	{
		return string("no change");
	}

	if(totalSteps*.9 < steps && !(angle <= 35.0 && angle >= -35.0))
	{
		if(angle > 35.0 && angle <= 110.0)
		{
			return string("left turn");
		}
		if(angle > 110.0 && angle <= 150.0)
		{
			return string("hard left");
		}
		if(angle < -35.0 && angle >= -110.0)
		{
			return string("right turn");
		}
		if(angle < -110.0  && angle >= -150.0 )
		{
			return string("hard right");
		}
		if(angle > 150  && angle < 180.0)
		{
			return string("uturn left");
		}
		if(angle < -150  && angle >= -180.0)
		{
			return string("uturn right");
		}
	}
	else if(totalSteps*.9 < steps) //if progress is over 90% to the next point
	{
		if(totalSteps < 7 )
		{
			return string("fast straight");
		}
		else
		{
			return string("slow straight");
		}
	}
	return string("no change");
}*/



/*updates the world, with the shark's location returned 
Vector3f SharkWorld::upCurrentLocation()
{
	//frame checks
	//hit next point. Update curPoint.
	if(totalSteps <= steps && curPoint < path.size())	
	{
		//set next variables
		steps = 0;
		curPoint = nextPoint;
		nextPoint++;
		totalSteps = path.gDTS(curPoint) * updateRate; //TODO change u to some speed value 
		updateAnimationFlag = true;
		animationLoop = string("no change"); //tell shark to stop turning
	
		rotation reset 
		
		desiredRotation = futureRotation;
		if(nextPoint == points.size()-1 )
		{
			futureRotation = calcRotation(0, 1);
		}
		else
		{
			futureRotation = calcRotation(nextPoint, nextPoint+1 == points.size()-1 ? 0: nextPoint+1);
		}

		//rotational difference
		deltaTheta = futureRotation - desiredRotation;
		
		if (deltaTheta.x > 3.1415926)
		{
			deltaTheta.x = deltaTheta.x - (2*3.1415926)  ;
		}
	        else if(deltaTheta.x < -3.1415926)
		{
			deltaTheta.x = deltaTheta.x + (2*3.1415926);	
		}*
		//TODO adjust for other axies of rotation
	}
	else if(totalSteps <= steps ) //go back to beginning.
	{
		steps = 0;
		nextPoint = 0;
	}
	else //interpolate
	{
		steps++;
			
	}
	//animation changes
	if(steps - totalSteps*.9 > 0 && updateAnimationFlag)
	{
		animationLoop = getSharkTurn();
	}
	
	//position
	Vector3f newLoc;
	Vector3f aheadTarget; //shark looks a bit ahead of itself
	if(curPoint < path.size()-1)
	{
		//printf("\n%f ___ ", ((float)steps)/((float)totalSteps));
		//TODO this is a test for timestamp. Change to proper dt usage from Main
		double uVal = path.catmullTimestamp((float)steps/((float)totalSteps), curPoint); 
		newLoc = path.splineLocation(((float)steps)/((float)totalSteps), curPoint); //this is the location of the shark
		//newLoc = path.splineLocation(uVal, curPoint); //this is the location of the shark
		
		//this is the look-ahead for the shark
		if(((float)steps)+(totalSteps*.3) > (float)totalSteps) 
		{
			//in the case where the lookahead is past the next point
			float difference = (((float)steps)+(totalSteps*.3)-((float)totalSteps));
			aheadTarget = path.splineLocation(difference/((float)totalSteps), nextPoint);
		}
		else
		{
			//in the case where the lookahead is within the next point
			aheadTarget = path.splineLocation(((((float)steps)+(totalSteps*.3)))/((float)totalSteps), curPoint);
		}
	}
	else
	{
		newLoc =  path.gPoint(curPoint);
	}

	//auto-calculate rotation
	rotation = calcRotation(newLoc, aheadTarget);	
	return newLoc;
}*/

//TODO: replace with catmulling timestamps
/*int SharkWorld::interpolateSpeed()
{
	int Qtotal = totalSteps - totalSteps*.9;
	int Qstep = steps - totalSteps*.9;
	if(Qstep > 0)
	{
		int Sthis = path.gDTS(curPoint) * updateRate;
		int Snext = path.gDTS(nextPoint) * updateRate;
		return doubleLerp( Qstep, 0,  Qtotal, Sthis, Snext);
	}
	return path.gDTS(curPoint) * updateRate;
}
*/

/*gradually rotates shark from the current (aka desired) rotation and the future rotation */
/*Vector3f SharkWorld::interpolateRotation()
{
	int Qtotal = totalSteps - totalSteps*.9;
	int Qstep = steps - totalSteps*.9;
	if(Qstep > 0)
	{
		return interpolateVertices(desiredRotation, desiredRotation+deltaTheta, Qstep, Qtotal);
	}
	return desiredRotation;
}*/

/*wrapper calling rotation based on the current point */
/*Vector3f SharkWorld::calcRotation()
{
	return calcRotation(
			path.gPoint(curPoint),
			path.gPoint(nextPoint)
			);
}*/

/* Calculates the GLOBAL rotation of the world, not relative to the world's current rotation
* pFrom, the first point,
* pDest, the next point. The two points create a line and the line's angle from the x axis is measured */
/*Vector3f SharkWorld::calcRotation(Vector3f pFrom, Vector3f pDest)
{
	Vector3f grotation;
	Vector3f xaxis = Vector3f(1,0,0);
	Vector3f yaxis = Vector3f(0,1,0);
	Vector3f zaxis = Vector3f(0,0,1);
	Vector3f point = pDest - pFrom;
	point.y = 0;
	bool tr = false;
	//if(!(point.x == 0 && point.y == 0))
	{
		tr = true;
		//printf("< %f %f >\n", point.x, point.y);
		double x = acos((point.Dot(xaxis))/(point.Magnitude()));
		if(point.z > 0)
		{
			x = 2.0*3.14159 - x;
		}

		/*double y = acos((point.Dot(yaxis))/(point.Magnitude()));
		if(point.y < 0)
		{
			y = 2.0*3.14159 - y;
		}
		double y = 0;

		double z = 0;//acos((point.Dot(zaxis))/(point.Magnitude()));
		//if(point.x < 0)
		//{
		//	z = 2.0*3.14159 - z;
		//}

		grotation = Vector3f(x,y,z);
	}
	return grotation;
}*/



void SharkWorld::updateWorld(int dt)
{
	//location = upCurrentLocation();
	traveler.update(dt);
}
void SharkWorld::displayWorld()
{
	glPushMatrix();
	{
		//Vector3f center = traveler.gLocation();
		//Vector3f center = location;
		glPushMatrix();
		{
			//glTranslatef(-center.x, -center.y, -center.z );	

			glPushMatrix();
			{
				drawSkybox();
			}glPopMatrix();	

			glPushMatrix();
			{
				//drawPoints();
				traveler.drawAndMoveCamera();
			}glPopMatrix();
		}glPopMatrix();
	}glPopMatrix();
}

void SharkWorld::drawSkybox()
{
	Vector3f location = traveler.gLocation();
	double xx = location.x + 90; //maxPt.x + 20;
	double xy = location.y + 90; //maxPt.y + 20;
	double xz = location.z + 90; //maxPt.z + 20;
	double nx = location.x - 90; //minPt.x - 20;
	double ny = location.y - 90; //minPt.y - 20;
	double nz = location.z - 90; //minPt.z - 20;
	//double step = ((maxPt.y+10) - (minPt.y-10))/3.0;

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();{
		if(skyboxrotation> 2.0*3.1415926) {skyboxrotation = 0;}
		else {skyboxrotation += 1.0*(3.1415926/180.0);}
		glRotatef(skyboxrotation, 0,1,0);
		glBegin(GL_QUADS);
		{
			//top
			glBindTexture(GL_TEXTURE_2D, 0);
			glColor3f(0, .6, 1);
			glNormal3f(0, -1, 0);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(xx, xy, xz);	
			glTexCoord2f(0.0, 1.0);
			glVertex3f(nx, xy, xz);	
			glTexCoord2f(1.0, 1.0);
			glVertex3f(nx, xy, nz);	
			glTexCoord2f(1.0, 0.0);
			glVertex3f(xx, xy, nz);
			
			//side 1
			glBindTexture(GL_TEXTURE_2D, 1);
			glNormal3f(0, 0, -1);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(xx, xy, xz);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(xx, xy, nz);
			glColor3f(0,0,.5);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(xx, ny, nz);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(xx, ny, xz);

			//side 2
			glNormal3f(-1,0,0);
			glColor3f(0, .6, 1);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(xx, xy, xz);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(nx, xy, xz);
			glColor3f(0,0,.5);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(nx, ny, xz);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(xx, ny, xz);
				
			//side 3
			glNormal3f(1,0,0);
			glColor3f(0, .6, 1);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(nx, xy, nz);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(nx, xy, xz);
			glColor3f(0,0,.5);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(nx, ny, xz);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(nx, ny, nz);

			//side4 
			glNormal3f(0,0,1);
			glColor3f(0, .6, 1);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(nx, xy, nz);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(xx, xy, nz);
			glColor3f(0,0,.5);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(xx, ny, nz);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(nx, ny, nz);

			//bottom
			glBindTexture(GL_TEXTURE_2D, 2);
			glNormal3f(0,1,0);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(nx, ny, xz);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(xx, ny, xz);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(xx, ny, nz);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(nx, ny, nz);

		}glEnd();
	}glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}
/*
 void SharkWorld::drawPointLine(int i)
{
	glPushMatrix();
	{
		glTranslatef(path.gPoint(i).x, path.gPoint(i).y, path.gPoint(i).z);
		if(pointInFrustum(path.gPoint(i))){
		       	glutSolidSphere(.1, 3, 2);
		}
	}glPopMatrix();
	if(i > 0)
	{
		if(pointInFrustum(path.gPoint(i)) || pointInFrustum(path.gPoint(i-1)))
		{
			//splined points
			Vector3f p1 = path.splineLocation(.1, i-1);
			Vector3f p2 = path.splineLocation(.2, i-1);
			Vector3f p3 = path.splineLocation(.3, i-1);
			Vector3f p4 = path.splineLocation(.4, i-1);
			Vector3f p5 = path.splineLocation(.5, i-1);
			Vector3f p6 = path.splineLocation(.6, i-1);
			Vector3f p7 = path.splineLocation(.7, i-1);
			Vector3f p8 = path.splineLocation(.8, i-1);
			Vector3f p9 = path.splineLocation(.9, i-1);
	
			glBegin(GL_LINES);
			glVertex3f(path.gPoint(i-1).x, path.gPoint(i-1).y, path.gPoint(i-1).z);
			glVertex3f(p1.x, p1.y, p1.z);
			
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);
			
			glVertex3f(p2.x, p2.y, p2.z);
			glVertex3f(p3.x, p3.y, p3.z);
			
			glVertex3f(p3.x, p3.y, p3.z);
			glVertex3f(p4.x, p4.y, p4.z);
			
			glVertex3f(p4.x, p4.y, p4.z);
			glVertex3f(p5.x, p5.y, p5.z);
			
			glVertex3f(p5.x, p5.y, p5.z);
			glVertex3f(p6.x, p6.y, p6.z);
	
			glVertex3f(p6.x, p6.y, p6.z);
			glVertex3f(p7.x, p7.y, p7.z);
	
			glVertex3f(p7.x, p7.y, p7.z);
			glVertex3f(p8.x, p8.y, p8.z);
	
			glVertex3f(p8.x, p8.y, p8.z);
			glVertex3f(p9.x, p9.y, p9.z);
	
			glVertex3f(p9.x, p9.y, p9.z);
			glVertex3f(path.gPoint(i).x, path.gPoint(i).y, path.gPoint(i).z);
			glEnd();
		}
	}
}
void SharkWorld::drawPoints()
{
	int step = 1;
	
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	float blue = 1.0;
	float red = 1.0;
	float green = 1.0;
	int i;
	int chunk = path.size()*.1 / 4;
	ExtractFrustum();

	//TODO: delete these testing spheres. 
	glPushMatrix();
        {
		glColor3f(0,.52,.86);
		float u;
		if(curPoint == 0){ u = 0;}
		else { u = (float)steps/(float)totalSteps; }
		Vector3f testAhead = path.getNearbyPoint(-.3 , curPoint, u );
		glTranslatef(testAhead.x, testAhead.y, testAhead.z);
		glutSolidSphere(.1, 3, 2);
	}glPopMatrix();
	glPushMatrix();
        {
		glColor3f(0,.52,.86);
		float u;
		if(curPoint == 0){ u = 0;}
		else { u = (float)steps/(float)totalSteps; }
		Vector3f testAhead = path.getNearbyPoint(.3 , curPoint, u );
		glTranslatef(testAhead.x, testAhead.y, testAhead.z);
		glutSolidSphere(.1, 3, 2);
	}glPopMatrix();
	glPushMatrix();
        {
		glColor3f(1,.52,.86);
		float u;
		if(curPoint == 0){ u = 0;}
		else { u = (float)steps/(float)totalSteps; }
		Vector3f testAhead = path.getNearbyPoint(.01 , curPoint, u );
		glTranslatef(testAhead.x, testAhead.y, testAhead.z);
		glutSolidSphere(.1, 3, 2);
	}glPopMatrix();

	//Close future points are drawn in a gradient going from white to yellow to green to black
	for(i = curPoint+1; i < (curPoint+(path.size()*.1)); i += step)
	{
		glColor3f(red,green,blue);
		drawPointLine(i);
		if(i > curPoint+chunk) red -= .02;
		if(i > curPoint+(3.0*chunk)){ red += .02; green -=.01; }
		if(i < curPoint+(chunk*2)){ blue -= .02; }
		if(i > curPoint+(chunk*2)){ blue += .02; }
		
	}
	
	//Past points drawn in red
	glColor3f(1,0,0);
	for(i = 0; i <= curPoint; i++)
	{
		drawPointLine(i);	
	}

	//Far future points drawn in black
	glColor3f(0,0,0);
	for(i = (curPoint+(path.size()*.1)) ;i < path.size(); i ++)
	{
		drawPointLine(i);	
	}
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

}

*/


