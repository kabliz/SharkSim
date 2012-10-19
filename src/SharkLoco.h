#ifndef SHARK_LOCOMOTION_PROCEDURAL_KEYFRAME_GENERATION
#define SHARK_LOCOMOTION_PROCEDURAL_KEYFRAME_GENERATION

#include "SharkSkeleton.h"
#include "Keyframe.h"
#include "MyMat.h"
#include "Mesh.h"
#include "SharkFacts.h"
#include <cmath>


/*Sets the angles in the skeleton. Represents the sinusodial motion of a shark */
class SharkLoco {

	public:
		SharkLoco(){locomotionMode = SUBCARANGIFORM; totalLength = 0;} //please set mesh 
		SharkLoco(SharkMesh *bmesh){finalAngles = vector<int>(); 
						curveAngles = vector<int>();
						animatedAngles = vector<vector<int> >(); 
						skeleton = SharkSkeleton(bmesh); 
						locomotionMode = SUBCARANGIFORM; 
						swimFrequency = 8; propellingAmplitude = .15; turningAngle = 0; elapsedTime = 0; totalLength = 0;
						}
		~SharkLoco(){}

		void buildSkeleton(Mesh* mesh, int numSegments, float *segLength);
		int calcTimestep(); //finds the point in time where it is optimal to make a new keyframe.  //TODO
		void update(int dt, int railAngle, float velocity); //main update call
		void setNewAngles(); //modifies the underlying bones
		void applyTransformation();    //commits all recent changes to the mesh 
		void setMesh(SharkMesh *s){skeleton.setMesh(s);}
		
		void buildAnimations( int totalFrames, GLfloat *segmentRot, int totalAngles  );
                bool buildAngles( GLfloat segmentRot[], int FrameNo, int totalAngles  );
		


		//simulation related functions
		void calcNextAngles(int railAngle);     //automatically gets next keyframe based on the speed of the animated angle rate.
		//takes in the angle of the curve 

		enum{ANGUILIFORM = 1, SUBCARANGIFORM = 2, CARANGIFORM = 3, THUNNIFORM = 4};

	private:
		SharkSkeleton skeleton;

		int framesPerSequence;
		int anglesPerFrame;
		int totalLength;
		float sharkRealLength; //TODO incorporate into totalLength when scaled.
		float segmentLength;

		vector<int> angles;
		vector<int> finalAngles;  		//these are the bone angles that will be exported to the Keyframe
		vector<int> curveAngles; 		//these represent the curvature of the spline at a point the shark will be.
		vector<vector<int> > animatedAngles; 	//these are preset, artist driven, bone angles. 

		int locomotionMode; //the method of locomotion being used currently: anguiliform, subcarangiform, ...etc.
		float propellingAmplitude; //amount of force to put in a swim stroke. Higher for more force. 
				//Often Never exceeds 1/4th-1/5th of the body length. Decreases as the fish gets larger.
		int turningAngle; //amount of turning happening
		float swimFrequency; //frequency of oscillation.  Maximum 5 beats per second
		float elapsedTime; //time (in seconds) since the simulation began. 

		static float const velocityToAmp = 15.0;  //divides velocity    by this value to determine the amplitude of a stroke

		//angle calculation funcs
		float calcTurningAngle();
		vector<int> getMaxAngles();  //returns the max angle for the current locomotion mode
		int nextSegmentAngle(int prevSegmentAngle, int prevTimeAngle, int maxAngle); //finds the individual angle of a segment.
		void findRailCurve(int railAngle);
		//need frequency (f), time (t), turning angle (TA),turning coefficeint (K sub i), relational initial angle (beta), propelling amplitude coefficent (K sub a),

		int gNumLocomotionBones();

		float deriveFrequency(float velocity); //derives frequency of tail swish based on the speed it is moving
};


#endif
