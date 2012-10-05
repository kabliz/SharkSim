#include "SplineTraveler.h"

/*
   double doubleLerp1(double input, double minx, double maxx, double miny, double maxy)
   {
   return miny + (input-minx) * ((maxy-miny)/(maxx-minx));
   }*/

//build spline with a frustum and a fileName to spline point data
SplineTraveler::SplineTraveler(Frustum *frus, string fileName)
{

	rotation = Vector3f(0,0,0); 
	nextPoint = 1;
        curPoint = 0;
	curU = 0;
	timer = 0;
	timeSinceKnot = 0;	
	elapseRate = 1;
	initSpline(fileName);
	frustum = frus;
	areGhostPoints = false;
}

SplineTraveler::SplineTraveler()
{ 
	rotation = Vector3f(0,0,0); 
	nextPoint = 1; 
	elapseRate = 1;
	areGhostPoints = false;
}

void SplineTraveler::resetTime() 
{
	rotation = Vector3f(0,0,0);
	curU = 0;
	timeSinceKnot = 0;
	elapseRate = 1;
	if(!areGhostPoints) {
		
		nextPoint = 1;
		curPoint = 0;
		timer = 0;
	}
	else {
		nextPoint = 2;
		curPoint = 1;
		timer = path.gDTS(1);	
	}
}

void SplineTraveler::initSpline(string filename){    //reads .mat or .csv data sheets
	path.initSpline(filename);
	/*char end[3] = { filename.at(filename.size()-3), 
		 	filename.at(filename.size()-2), 
			filename.at(filename.size()-1) };
	if(end[1] == 't' || end[1] == 'T' ) { 
		initSplineZOE(filename); 
	}
	else if(end[1] == 'm' || end[1] == 'M' ) { 
		initSplineMAT(filename); 
	}
	else if (end[1] == 'c' || end[1] == 'C'){ 
		initSplineEXE(filename);
	}
	else {
		printf("Spline file does not correspond to data file type\n");
	}*/
}


/*Update each frame with the amount of time elapsed */
void SplineTraveler::update(float dt)
{
	float prevTime = timer;
	Vector3f prevLocation = location;
	location = upCurrentLocation(dt);
	velocity = location.EuclDistance(prevLocation) / (timer - prevTime); 
}

/*Draws the Spline with the center set to the current location */
void SplineTraveler::drawAndMoveCamera()
{
	glPushMatrix();
	{
		Vector3f center = location;
		glTranslatef(-center.x, -center.y, -center.z );
		drawPoints();

	}glPopMatrix();
}

/*Just draws the spline points within the frustum */
void SplineTraveler::drawStatic()
{
	glPushMatrix();
	{
		drawPoints();
	}glPopMatrix();
}

Vector3f SplineTraveler::gNearbyPoint(float ahead, int point, float u)
{
	Vector3f res;
	if(ahead <= 0.0001 && ahead >= -0.0001) {
		res = path.splineLocation(u, point);
	}
	else {
		res = path.getNearbyPoint(ahead, point, u);
	}
	return res;
	
}

/*updates the world, with the traveler's location returned
 * receives delta time in miliseconds since the last update */
Vector3f SplineTraveler::upCurrentLocation(int dt)
{
	
	timer += (dt/1000.0) * elapseRate;
	timeSinceKnot += (dt/1000.0) * elapseRate;
	
	//frame checks
	
	//hit next point. Update curPoint.
	if(timer >= path.gTotalDTS(nextPoint))
	{
		//increment the points and move on.
		timeSinceKnot = timer - path.gTotalDTS(nextPoint);
		curPoint = nextPoint;
		nextPoint++;
			
		//updateAnimationFlag = true;
		//animationLoop = string("no change"); //tell traveler to stop turning
	}
	else if(curPoint >= path.size()-1 ) //go back to beginning.
	{
		if(!areGhostPoints) {
			nextPoint = 0;
		}
		else {
			nextPoint = 1;
		}
	}
	//animation changes
	//if(steps - totalSteps*.9 > 0 && updateAnimationFlag)
	//{
	//animationLoop = getSharkTurn();
	//}

	//position
	Vector3f newLoc;
	Vector3f aheadTarget; //traveler looks a bit ahead of itself
	
	curU = path.convertTimestampToU(timer, curPoint);
	int ppoint = curPoint;
	if(curU < 0) {    //interpolation slid to previous point. currently only handles one point in reverse
		ppoint -= 1;
		if(areGhostPoints && (ppoint < 1))
		{
			newLoc = path.splineLocation(0,1);
		}
		else if (!areGhostPoints && ppoint < 0) {//went off the spline backwards. set everything to zero
			newLoc = path.splineLocation(0,0);	
		}
		else {  //only went one point backwards.
			curU = 1 + curU;
		}

	}
	newLoc = path.splineLocation(curU, ppoint); //this is the location of the traveler 
	aheadTarget = path.getNearbyPoint(.3, ppoint, curU);

	//auto-calculate rotation
	rotation = calcRotation(newLoc, aheadTarget);
	return newLoc;
}

/*returns the angle of the curve around the current point, 
 * takes in the amount (in u paramater) to lookAhead of the current point (for predictive movement)
 * and then two distances ahead and behind the point to derive the endpoints of the vectors measuring the angle.
 * Returns an angle in degrees */
int SplineTraveler::deriveRailAngle(float lookAhead, float frontBy, float behindBy)
{
	int aheadPoint = curPoint;
	//float pu = totalSteps == 0 ? 0 : ((float) steps) /(float) totalSteps;
	float pu = curU; 
	if(pu+lookAhead > 1) {
		pu = pu+lookAhead-1.0;
		aheadPoint++;
	} else {
		pu+lookAhead;
	}

	Vector3f pastpoint = path.getNearbyPoint(behindBy, aheadPoint, pu);
	Vector3f futurepoint = path.getNearbyPoint(frontBy, aheadPoint, pu);
	Vector3f thispoint = path.splineLocation(pu,  aheadPoint);

	Vector3f firstBranch = (pastpoint - thispoint).Normalize();
	Vector3f secondBranch = (futurepoint - thispoint).Normalize();

	//arcos dot product/productof magnitudes = angle in radians
	float railAngle;
	railAngle = atan2(secondBranch.x, secondBranch.z) - atan2(firstBranch.x, firstBranch.z);

	//accidentally measures 2pi sometimes when very small angles are being measured .
	if(railAngle > 5.5) { railAngle -= 6.283185; }
	if(railAngle < -5.5) { railAngle += 6.283185; }

	//acute angles only
	while(railAngle > 3.14159265) { railAngle -= 3.14195265; }
	while(railAngle < -3.14159265) { railAngle += 3.14195265; }
	railAngle *= -180/3.14159265 ;
	return (int)railAngle;
}

/*gradually rotates shark from the current (aka desired) rotation and the future rotation */
Vector3f SplineTraveler::interpolateRotation()
{
	//TODO check this function for usage amounts. It may be deprecated.
	//int Qtotal = .9; //totalSteps - totalSteps*.9;
	//int Qstep = curU; //steps - totalSteps*.9;
	//if(Qstep > 0)
	{
	//	return desiredRotation.Interpolate( desiredRotation+deltaTheta, Qstep/ Qtotal);
	}
	return desiredRotation;
}

/*wrapper calling rotation based on the current point */
Vector3f SplineTraveler::calcRotation()
{
	return calcRotation(
			path.gPoint(curPoint),
			path.gPoint(nextPoint)
			);
}

/* Calculates the GLOBAL rotation of the world, not relative to the world's current rotation
 * * pFrom, the first point,
 * * pDest, the next point. The two points create a line and the line's angle from the x axis is measured */
Vector3f SplineTraveler::calcRotation(Vector3f pFrom, Vector3f pDest)
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
		double x = acos((point.Dot(xaxis))/(point.Magnitude()));
		if(point.z > 0)
		{
			x = 2.0*3.14159 - x;
		}

		/*double y = acos((point.Dot(yaxis))/(point.Magnitude()));
		*/
		double y = 0;

		double z = 0;//acos((point.Dot(zaxis))/(point.Magnitude()));

		grotation = Vector3f(x,y,z);
	}
	return grotation;
}



/*Draws line between the previous line and the current one */
void SplineTraveler::drawPointLine(int i)
{
	glPushMatrix();
	{
		glTranslatef(path.gPoint(i).x, path.gPoint(i).y, path.gPoint(i).z);
		if(frustum->testPoint(path.gPoint(i)))
		{
			glutSolidSphere(.1, 3, 2);
		}
	}glPopMatrix();
	if(i > 0)
	{
		if(frustum->testPoint(path.gPoint(i)) || frustum->testPoint(path.gPoint(i-1)))
		{
			//splined points for more curvature
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

void SplineTraveler::drawPoints()
{
	int step = 1;

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	float blue = 1.0;
	float red = 1.0;
	float green = 1.0;
	int i;
	int chunk = path.size()*.1 / 4;

	frustum->extract();

	//TODO: delete these testing spheres. 
	glPushMatrix();
	{
		glColor3f(0,.52,.86);
		float u;
		if(curPoint == 0){ u = 0;}
		else { u = curU;}//(float)steps/(float)totalSteps; }
		Vector3f testAhead = path.getNearbyPoint(-.3 , curPoint, u );
		glTranslatef(testAhead.x, testAhead.y, testAhead.z);
		glutSolidSphere(.1, 3, 2);
	}glPopMatrix();
	glPushMatrix();
	{

		glColor3f(0,.52,.86);
		float u;
		if(curPoint == 0){ u = 0;}
		else { u = curU;}//(float)steps/(float)totalSteps; }
		Vector3f testAhead = path.getNearbyPoint(.3 , curPoint, u );
		glTranslatef(testAhead.x, testAhead.y, testAhead.z);
		glutSolidSphere(.1, 3, 2);
	}glPopMatrix();

	glPushMatrix();
	{
		glColor3f(1,.52,.86);
		float u;
		if(curPoint == 0){ u = 0;}
		else { u = curU;}//(float)steps/(float)totalSteps; }
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
	if(areGhostPoints){
		i = 1;
	}
	else {i = 0;}
	for(/*same val*/ ; i <= curPoint; i++)
	{
		drawPointLine(i);
	}

	//Far future points drawn in black
	glColor3f(0,0,0);
	for(i = (curPoint+(path.size()*.1)) ;i < (areGhostPoints?path.size()-1: path.size()); i ++)
	{
		drawPointLine(i);
	}
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}









