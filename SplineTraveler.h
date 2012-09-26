/*CSC 474 lab 2 */
#ifndef SPLINE_TRAVELLER_3D_SPLINE
#define SPLINE_TRAVELLER_3D_SPLINE


#include "SplinePath.h"
#include "Vector.h"
#include "Frustum.h"

#include <cstdio>
#include <vector>
#include <cstdlib>
#include <string>
#include <GL/glut.h>

/* A Spline progress management class. Assumes one entity has a spline. */
class SplineTraveler
{
	public:
		SplineTraveler();
		SplineTraveler(Frustum *frus, string splineFilename); 
		~SplineTraveler(){}
		void update(float dt);
		void drawAndMoveCamera();
		void drawStatic();
		void ExtractFrustum();
		bool pointInFrustum(Vector3f v);
		Vector3f upCurrentLocation(float dt);
		void initSpline(string filename);    //reads .mat or .csv data sheets 
		void initSplineMAT(string matfilename);     //opens mat files
		void initSplineEXE(string exefilename);     //opens csv files
		void setFrustum(Frustum *frust){frustum = frust;}   
		//int deriveRailAngle();


		void deleteHeap(){path.deleteHeap();} //delete's SplinePath heap
		Vector3f gPathPoint(int index){return path.gPoint(index);}
		Vector3f gCurrentPoint(){return path.gPoint(curPoint);}
		Vector3f gNextPoint(){if(curPoint < path.size()){
			return path.gPoint(curPoint+1); }
			else {
				return path.gPoint(0);}
		}
		Vector3f gPrevPoint(){if(curPoint == 0){
			return path.gPoint(path.size()-1); }
			else {
				return path.gPoint(curPoint-1);}
		}
		Vector3f gRotationDegrees(){return rotation * 180/3.14159265 ;}
		Vector3f gRotationRadians(){return rotation;}
		Vector3f gLocation(){return location;}

		double gDTS(int index){return path.gDTS(index);} //time difference between points               
		double gCurrentDTS(){return path.gDTS(curPoint);}


	private:

		void drawPoints();
		void drawPointLine(int i);
		Vector3f calcRotation();
		Vector3f calcRotation(Vector3f pFrom, Vector3f pDest);
		Vector3f interpolateRotation();
		


		SplinePath path;
		Frustum *frustum;

		//global animation controls
		Vector3f rotation; 	//current rotation
		Vector3f location; 	//current location
		Vector3f desiredRotation;
		Vector3f futureRotation;
		Vector3f deltaTheta; 	//difference between desired and future rotation

		
		int curPoint;
		int timer;       	//TODO toss around the dt value. Interpolate time into utime. Double check conversion from time into catmull paramaters
		int steps;  		//interpolation progress
		int totalSteps; 	//total interpolation needed. Steps and totalSteps help keep track of the time. 
					//They get converted to a arc progress value when interpolating.
		int nextPoint;
		int updateRate;  	//how much to update each step through between knots by, related to the framerate

};

#endif
