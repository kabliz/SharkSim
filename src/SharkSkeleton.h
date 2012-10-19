#ifndef SHARK_SKELETON_BONES_ANIMATION_3D
#define SHARK_SKELETON_BONES_ANIMATION_3D

#include "SharkBone.h"
#include "Keyframe.h"
#include "MyMat.h"
#include "Mesh.h"
#include "SharkFacts.h"
#include <cmath>
#include <map>


/*This is the Skeletel model of an animated kinematic system. An armature. 
 * It is an interface to the bones which can turn them into Keyframes so they can be drawn  */
class SharkSkeleton
{
	public:
		//initializations
		SharkSkeleton(){SharkSkeleton(NULL); locomotionMode = SUBCARANGIFORM; totalLength = 0;} //please set mesh 
		SharkSkeleton(SharkMesh *bmesh){nmesh = bmesh; rootNode = itoa(4); finalAngles = vector<int>(); curveAngles = vector<int>(); 
						animatedAngles = vector<vector<int> >(); bones = map<string, SharkBone*>(); nextFrameNo = 0; 
						locomotionMode = SUBCARANGIFORM; totalLength = 0;
						swimFrequency = 8; propellingAmplitude = .15;  turningAngle = 0; elapsedTime = 0;}
		void buildSkeleton(Mesh *mesh, int numSegs, float* segLengths); //given a set of mesh segments, it creates the bones. 
		void buildAnimations( int totalFrames, GLfloat *segmentRot, int totalAngles  );
		bool buildAngles( GLfloat segmentRot[], int FrameNo, int totalAngles  );
		void setMesh(SharkMesh *s){nmesh =s;}
		void clearHeap(); //TODO clean the heap out

		//manipulations
		void sAngle(string boneName, int angle);

		//transformation related functions
		void applyTransformation();	//export sketon to the smart SharkMesh  so it can be drawn by the Keyframe
		void transformHeirarchy(int isDownstream , MyMat stackMatrix);	

		//modes of swimming locomotion in fish. 
		enum{ANGUILIFORM = 1, SUBCARANGIFORM = 2, CARANGIFORM = 3, THUNNIFORM = 4};

		//MOVE TO LOCO
		int calcTimestep(); //finds the point in time where it is optimal to make a new keyframe. //TODO
		void update(int dt, int railAngle, float velocity); //main update call
		void setNewAngles(); //modifies the underlying bones
		void calcNextAngles(int railAngle);	//automatically gets next keyframe based on the speed of the animated angle rate.
								//takes in the angle of the curve 

	//private:
	
		//objs
		map<string, SharkBone*> bones; //List of Bones in the shark. 

		//Move to loco
		vector<int> finalAngles;  //these are the bone angles that will be exported to the Keyframe
		vector<int> curveAngles; //these represent the curvature of the	spline at a point the shark will be.
		vector<vector<int> > animatedAngles; //these are preset, artist driven, bone angles. 
	
	
		int locomotionMode; //the method of locomotion being used currently: anguiliform, subcarangiform, ...etc.
		float propellingAmplitude; //amount of force to put in a swim stroke. Higher for more force. 
					//Often Never exceeds 1/4th-1/5th of the body length. Decreases as the fish gets larger.
		int turningAngle; //amount of turning happening
		float swimFrequency; //frequency of oscillation.  Maximum 5 beats per second
		float elapsedTime; //time (in seconds) since the simulation began. 
		

	private:
		//matrices
		MyMat armTranslation;   //A transform matrix that will move the whole armature before positioning the bones. 
					//use in case the model is offset when drawn.	

		vector<int> angles;
		double segmentLength;
		string rootNode; //root node of the heirarchical model.
		SharkMesh *nmesh;
		double totalLength;  //the length of the shark, in world coordinates.
		float sharkRealLength; //TODO incorporate into totalLength when scaled.
		int framesPerSequence;
		int anglesPerFrame;
		
		//animation frames
		int nextFrameNo;    //TODO ??? what is this again
		
		//Bone manipulation
		SharkBone* gBone(string boneName);
		SharkBone* gBone(int boneName);
		string itoa(int toConvert);       //int to string.   not really atoi backwards, but supports numbers up to 100000.  
		void sBone(SharkBone* newBone);   //sets bone with its name as the key

		//angle math
		static float const velocityToAmp = 15.0;  //divides velocity	by this value to determine the amplitude of a stroke

		//angle calculation funcs. Move to Loco
		float calcTurningAngle();
		vector<int> getMaxAngles();  //returns the max angle for the current locomotion mode
		int nextSegmentAngle(int prevSegmentAngle, int prevTimeAngle, int maxAngle); //finds the individual angle of a segment.
		void findRailCurve(int railAngle);
		//need frequency (f), time (t), turning angle (TA),turning coefficeint (K sub i), relational initial angle (beta), propelling amplitude coefficent (K sub a),
		int gNumLocomotionBones();
		float deriveFrequency(float velocity); //derives frequency of tail swish based on the speed it is moving
};


#endif
