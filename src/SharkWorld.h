#ifndef SHARK_WORLD_3D_SIMULATION
#define SHARK_WORLD_3D_SIMULATION


#include "MATreader.h"
#include "EXEreader.h"
#include "Vector.h"
#include "SplinePath.h"
#include "SplineTraveler.h"
#include "Frustum.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <GL/glut.h>

//represents a SINGLE world a shark inhabits
class SharkWorld
{
	public:
		SharkWorld(){}
		SharkWorld(Frustum *frus, string splineFilename){ traveler = SplineTraveler(frus, splineFilename);  
					 updateAnimationFlag = true; }
		~SharkWorld(){}
		void updateWorld(int dt);
		void displayWorld();
		//string getSharkTurn();
		//void initialize(string splineFilename);
		int deriveRailAngle();

		//lowerclass management
		void deleteHeap(){traveler.deleteHeap();} //delete's SplinePath heap
		Vector3f gPathPoint(int index){return traveler.gPathPoint(index);}
		Vector3f gCurrentPoint(){return traveler.gCurrentPoint();} 
		Vector3f gNextPoint(){ return traveler.gNextPoint();} 
		Vector3f gPrevPoint(){ return traveler.gPrevPoint();}
		Vector3f gRotationDegrees(){return traveler.gRotationDegrees();}
		Vector3f gRotationRadians(){return traveler.gRotationRadians();}
		
		double gDTS(int index){return traveler.gDTS(index);} //time difference between points		
		double gCurrentDTS(){return traveler.gCurrentDTS();}
		
		string gAnimationLoop(){return animationLoop;}

		float gVelocity(){return traveler.gVelocity();} //velocity the traveler is moving

		void speedUp(){traveler.speedUp();}
		void slowDown(){traveler.slowDown();}
		void resetTime(){traveler.resetTime();}

	private:

		//world data info
		SplineTraveler traveler;
		
		//global animation controls
		bool updateAnimationFlag;
		Vector3f deltaTheta; //difference between desired and future rotation
		string animationLoop;
		
		float skyboxrotation;

		//for calculating shark's lookahead and angle calculating range
		static const float lookAhead = .15;
		static const float frontby = -.3;
		static const float behindby = .3;

		void drawSkybox();
		void drawPrettyStuff();

};

#endif
