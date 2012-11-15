#ifndef D_SHARK_SPLINE_PATH
#define D_SHARK_SPLINE_PATH

#include "MATreader.h"
#include "EXEreader.h"
#include "ZOEreader.h"
#include "ParamFunctions.h"
#include "ParamStructures.h"
#include "SplineTable.h"
#include "GPSconverter.h"
#include "Vector.h"
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <GL/glut.h>


/*Holds data for a catmull-rom path. This class can take in points read in from a file, and also reparamaterize teh curve by arc-length   */
class SplinePath
{
	public:
		//initialization functions
		SplinePath(){Tolerance= .2; tabSet = false; isGhostPointMode = false;}
		SplinePath(bool ghostPoints){Tolerance= .2; tabSet = false; isGhostPointMode = ghostPoints;}    //set true to make the first and 
														//last points invisible
		~SplinePath(){}
		void initSpline(string filename);    //reads .txt or .mat or .csv data sheets 
		void initSplineMAT(string matfilename);     //opens mat files
		void initSplineEXE(string exefilename);     //opens csv files
		void initSplineZOE(string zoefilename);     //opens txt files
		void gatherDTPoints();
                void gatherEXPoints();
		void gatherZOEPoints();
		void paramaterizeSpline();
		void initTangents();
		void calcRadius();
		void deleteHeap();
		int size(){return points.size();}

		//interpolation functions
		double StoU(double, int);
		Vector3f splineLocation(float u, int startpoint);	
		Vector3f getNearbyPoint(double distanceAhead, int startPoint, double startU);
		double convertTimestampToU(float u, int currentLocation);
		
		//displayFunctions
		//void drawPoints(float** frustum);

		//points
		Vector3f gPoint(int index){return points[index];}
		void parseDataFileMAT(const char* filename){mreader.parseFile(filename);}	
		void parseDataFileEXE(const char* filename){ereader.parseFile(filename);}	
		void parseDataFileZOE(const char* filename){zoereader.parseFile(filename);}	
		double gDTS(int index){return dts[index];} //time delta distance between two points
		double gTotalDTS(int index){return totts[index];} //time delta distance between two points
		bool sGhostPoints(bool b){isGhostPointMode = b;}

	private:	

		//data
		vector<SplineTable*> paramTable;

		//original data read
		vector<Vector3f> points;   //vector stores x, y, z(where z is water depth, unlike rendering)
		vector<Vector3f> tangents; //tangents for the curve as it hits each point
		vector<double> dts; //difference in time (seconds) between one point and the next
		vector<double> totts; //total running time at each point 
		Vector3f maxPt; //records the maximum point the shark traveled
		Vector3f minPt; //minimum point
		Vector3f midPt;

		//running totals of timestamp for catmull interpolation
		//double timePast;
		//double timeCurrent;
		//double timeNext;
		double curTimeSpline;

		MATreader mreader;
		EXEreader ereader;
		ZOEreader zoereader;
		double radius;
		ParamFunctions paramfuncs;

		void isLargerPoint(Vector3f cur);
		void isSmallerPoint(Vector3f cur);
		double doubleLerp(double input, double minx, double maxx, double miny, double maxy);
		Vector3f hermiteMatrix(float u, Vector3f startLocation, Vector3f endLocation, Vector3f startTangent, Vector3f endTangent);
		Vector3f catmullMatrix(float u, int currentLocation);
		
		double HmInt(float U[4], const float M[16], float B[4]);
		bool tabSet;

		bool isGhostPointMode;
		float static const Mcat[16];// = {-.5, 1.5, -1.5, .5, 1, -2.5, 2, -.5, -.5, 0, .5, 0, 0, 1, 0, 0};
		float static const Mher[16];// = {2, -2, 1, 1, -3, 3, -2, -1, 0, 0, 1, 0, 1, 0, 0, 0};
		double Tolerance;
		int static const totalSlices = 100; //determines how finely to sample the curve
		bool static const isCatmullMode = false;
};

#endif

