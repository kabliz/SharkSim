#ifndef SHARK_WORLD_3D_SIMULATION
#define SHARK_WORLD_3D_SIMULATION


#include "MATreader.h"
#include "EXEreader.h"
#include "Vector.h"
#include "SplinePath.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <GL/glut.h>

//represents a SINGLE world a shark inhabits
class SharkWorld
{
	public:
		SharkWorld(){ rotation = Vector3f(0,0,0); nextPoint = 1; updateRate = 60; updateAnimationFlag = true; }
		~SharkWorld(){}
		void updateWorld();
		void displayWorld();
		void ExtractFrustum();
		bool pointInFrustum(Vector3f v);
		Vector3f upCurrentLocation();
		string getSharkTurn();
		void initSpline();
		int deriveRailAngle();

		//lowerclass management
		void deleteHeap(){path.deleteHeap();} //delete's SplinePath heap
		Vector3f gPathPoint(int index){return path.gPoint(index);}
		Vector3f gCurrentPoint(){return path.gPoint(curPoint);} 
		Vector3f gNextPoint(){if(curPoint < path.size()){ 
						return path.gPoint(curPoint+1); }
					else {
						return path.gPoint(0);}}
		Vector3f gPrevPoint(){if(curPoint == 0){
						return path.gPoint(path.size()-1); }
				     else {	
						return path.gPoint(curPoint-1);}}
		Vector3f gRotationDegrees(){return rotation * 180/3.14159265 ;}
		Vector3f gRotationRadians(){return rotation;}
		
		double gDTS(int index){return path.gDTS(index);} //time difference between points		
		double gCurrentDTS(){return path.gDTS(curPoint);}
		
		string gAnimationLoop(){return animationLoop;}


	private:

		//world data info
		SplinePath path;
		double frustum[6][4];
		
		//global animation controls
		Vector3f rotation; //current rotation
		Vector3f location; //current location
		int curPoint;
		int timer;  //TODO toss around the dt value. Interpolate time into utime. Double check conversion from time into catmull paramaters
		int steps;  //interpolation progress
		int totalSteps; //total interpolation needed. Steps and totalSteps help keep track of the time. 
				//They get converted to a arc progress value when interpolating.
		int nextPoint;
		int updateRate;  //how much to update each step through between knots by, related to the framerate
		bool updateAnimationFlag;
		Vector3f desiredRotation;
		Vector3f futureRotation;
		Vector3f deltaTheta; //difference between desired and future rotation
		string animationLoop;
		
		float skyboxrotation;

		void drawSkybox();
		void drawPoints();
		void drawPrettyStuff();
		void drawPointLine(int i);
		Vector3f interpolateVertices(Vector3f first, Vector3f second, int step, int max);
		Vector3f calcRotation();
		Vector3f calcRotation(Vector3f pFrom, Vector3f pDest);
		Vector3f interpolateRotation();
		int interpolateSpeed();	

};

#endif
