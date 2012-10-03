#ifndef SHARK_SKELETON_BONES_ANIMATION_3D
#define SHARK_SKELETON_BONES_ANIMATION_3D

#include "SharkBone.h"
#include "Keyframe.h"
#include "MyMat.h"
#include "Mesh.h"
#include "SharkFacts.h"
#include <cmath>

/*This is the main simulation skeleton class. It simulates (moves angles of) the bones and prints them to Keyframes so they can be drawn  */
class SharkSkeleton
{
	public:
		//initializations
		SharkSkeleton(){SharkSkeleton(NULL); locomotionMode = SUBCARANGIFORM; totalLength = 0;} //please set mesh 
		SharkSkeleton(SharkMesh *bmesh){nmesh = bmesh; rootNode = 4; finalAngles = vector<int>(); curveAngles = vector<int>(); 
						animatedAngles = vector<vector<int> >(); bones = vector<SharkBone>(); nextFrameNo = 0; 
						locomotionMode = SUBCARANGIFORM; totalLength = 0;
						swimFrequency = 8; propellingAmplitude = .15;  turningAngle = 0; elapsedTime = 0;}
		void buildSkeleton(Mesh *mesh, int numSegs, float* segLengths); //given a set of mesh segments, it creates the bones. 
		void uCalibration();  //calibrates the separation between bones in terms of the spline value U.
		void buildAnimations( int totalFrames, GLfloat *segmentRot, int totalAngles  );
		bool buildAngles( GLfloat segmentRot[], int FrameNo, int totalAngles  );
		void setMesh(SharkMesh *s){nmesh =s;}

		//simulation related functions
		void calcNextAngles(int railAngle);	//automatically gets next keyframe based on the speed of the animated angle rate.
		//void findNextCurve();
		
		int calcTimestep(); //finds the point in time where it is optimal to make a new keyframe.
		void update(int dt, int railAngle, float velocity); //main update call
		void setNewAngles(); //modifies the underlying bones

		//transformation related functions
		void applyTransformation();//export sketon to the smart SharkMesh  so it can be drawn by the Keyframe
		void transformHeirarchy(int isDownstream , int curNode, MyMat stackMatrix);	

		enum{ANGUILIFORM = 1, SUBCARANGIFORM = 2, CARANGIFORM = 3, THUNNIFORM = 4};
	//private:
	
		//objs
		vector<SharkBone> bones; //List of Bones in the shark. They are each made up of Quads
		//map<Vector3f, SharkVertex*, compareVect3> baseVertices; //the vertices that Quad classes untimately point to. 
								        //Pointer structure automatically connects the mesh segments
		vector<int> finalAngles;  //these are the bone angles that will be exported to the Keyframe
		vector<int> curveAngles; //these represent the curvature of the	spline at a point the shark will be.
		vector<vector<int> > animatedAngles; //these are preset, artist driven, bone angles. 
		
		int locomotionMode; //the method of locomotion being used currently: anguiliform, subcarangiform, ...etc.
		float propellingAmplitude; //amount of force to put in a swim stroke. Higher for more force. 
					//Often Never exceeds 1/4th-1/5th of the body length. Decreases as the fish gets larger.
		int turningAngle; //amount of turning happening
		float swimFrequency; //frequency of oscillation.  Maximum 5 beats per second
		int elapsedTime; //time since the simulation began. 
		

	private:
		//modes of swimming locomotion in fish. 
		

		vector<int> angles;
		double segmentLength;
		int rootNode; //root node of the heirarchical model.
		SharkMesh *nmesh;
		double totalLength;  //the length of the shark, in world coordinates.
		float sharkRealLength; //TODO incorporate into totalLength when scaled.
		int framesPerSequence;
		int anglesPerFrame;
		
		//animation frames
		int nextFrameNo;

		//angle calculation funcs
		float calcTurningAngle();
		vector<int> getMaxAngles();  //returns the max angle for the current locomotion mode
		int nextSegmentAngle(int prevSegmentAngle, int prevTimeAngle, int maxAngle); //finds the individual angle of a segment.
		void findRailCurve(int railAngle);
		//need frequency (f), time (t), turning angle (TA),turning coefficeint (K sub i), relational initial angle (beta), propelling amplitude coefficent (K sub a),
		static float const velocityToAmp = 5.0;  //divides velocity	by this value to determine the amplitude of a stroke


		float deriveFrequency(float velocity); //derives frequency of tail swish based on the speed it is moving
};


#endif
