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
}


/*Update each frame with the amount of time elapsed */
void SplineTraveler::update(float dt)
{
	float prevTime = timer;
	Vector3f prevLocation = location;
	location = upCurrentLocation(dt);
	if(timer - prevTime > 0){
		velocity = (location-prevLocation) / (timer - prevTime); 
	}

	//printf("%d <%f, %f, %f>\n", curPoint, location.x, location.y, location.z ); 
	//printf("%d ", curPoint); 
}

/*Draws the Spline with the center set to the current location */
void SplineTraveler::drawAndMoveCamera()
{
	glPushMatrix();
	{
		Vector3f center = location;
		glTranslatef(-center.x, -center.y, -center.z );
		path.drawPoints(curPoint, areGhostPoints, frustum);

	}glPopMatrix();
}

/*Just draws the spline points within the frustum */
void SplineTraveler::drawStatic()
{
	glPushMatrix();
	{
		path.drawPoints(curPoint, areGhostPoints, frustum);
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
	//rotation = calcRotation(newLoc, aheadTarget);
	calcRotationAngle();
	
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

	//accidentally measures 2pi sometimes when very small angles are being measured.
	if(railAngle > 5.5) { railAngle -= 6.283185; }
	if(railAngle < -5.5) { railAngle += 6.283185; }

	//acute angles only
	while(railAngle > 3.14159265) { railAngle -= 3.14195265; }
	while(railAngle < -3.14159265) { railAngle += 3.14195265; }
	railAngle *= -180/3.14159265 ;
	//save axis of rotation
	
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
double SplineTraveler::calcRotationAngle()
{
	//rotationAngle = deriveRailAngle(0, .6, .3);
	//compare rotation of traveler to an side axis vector
	
	//calcRotation(Vector3f(1,0,0), path.splineLocation(curU, curPoint), 
	calcRotation(Vector3f(1,0,0), path.getNearbyPoint(-.2, curPoint, curU), 
					path.getNearbyPoint(.3, curPoint, curU));
	return rotationAngle;
}

/* Calculates the GLOBAL rotation of the world, not relative to the world's current rotation
 * * pFrom, the first point,
 * * pDest, the next point. The two points create a line and the line's angle from the x axis is measured */
double SplineTraveler::calcRotation(Vector3f axis, Vector3f pVertex, Vector3f pDest)
{
	//Vector3f firstBranch = (pFrom - pVertex).Normalize();  
	Vector3f secondBranch = (pDest - pVertex).Normalize(); 

	//TODO handle 180 degree case
	float railAngle;
	//dot product angle. magtides of the vectors are already equal to one
	railAngle = acos(axis.Dot(secondBranch));
	if(secondBranch.z > 0)
	{
		railAngle = 2.0*3.14159265 - railAngle;
	}
	rotationAngle = railAngle;
	rotateAxis = secondBranch.Cross(axis);   //axis of this rotation.
	//rotateAxis = Vector3f(0,1,0);
	/*Vector3f xaxis = Vector3f(1,0,0);
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

		//double y = acos((point.Dot(yaxis))/(point.Magnitude()));
		
		double y = 0;

		double z = 0;//acos((point.Dot(zaxis))/(point.Magnitude()));

		grotation = Vector3f(x,y,z);
	}*/
	return railAngle;
}



