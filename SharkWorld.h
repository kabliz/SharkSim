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

		//world data info
		SplinePath sPath;
		double frustum[6][4];
		
		//global animation controls
		Vector3f rotation; //current rotation
		Vector3f location; //current location
		int curPoint;
		int steps;  //interpolation progress
		int totalSteps; //total interpolation needed. Steps and totalSteps help keep track of the time. They get converted to a arc progress value when interpolating.
		int nextPoint;
		int updateRate;  //how much to update each frame by, related to the framerate
		bool updateAnimationFlag;
		Vector3f desiredRotation;
		Vector3f futureRotation;
		Vector3f deltaTheta; //difference between desired and future rotation
		string animationLoop;
		
		
	private:
		void drawSkybox();
		void drawPoints();
		void drawPrettyStuff();
		void drawPointLine(int i);
		Vector3f interpolateVertices(Vector3f first, Vector3f second, int step, int max);
		Vector3f calcRotation();
		Vector3f calcRotation(Vector3f pFrom, Vector3f pDest);
		Vector3f interpolateRotation();
		int interpolateSpeed();	

		float skyboxrotation;
};

#endif
