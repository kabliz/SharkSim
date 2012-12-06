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
		SharkLoco(){locomotionMode = SUBCARANGIFORM; totalLength = 0; sharkRealLength = TSEMI_LENGTH_M; } //please set mesh 
		SharkLoco(SharkMesh *bmesh){finalAngles = vector<int>(); 
						curveAngles = vector<int>();
						axialAngles = vector<int>();
						animatedAngles = vector<vector<int> >(); 
						skeleton = SharkSkeleton(bmesh); 
						locomotionMode = SUBCARANGIFORM; 
						swimFrequency = 8; propellingAmplitude = .15; turningAngle = 0; elapsedTime = 0; totalLength = 0;
						sharkRealLength = TSEMI_LENGTH_M;
						}
		~SharkLoco(){}

		void buildSkeleton(Mesh* mesh, int numSegments, float *segLength);
		void buildSkeleton(string modelFilename);
		int calcTimestep(); //finds the point in time where it is optimal to make a new keyframe.  //TODO
		void update(int dt, int railAngle, Vector3f velocity); //main update call
		void setNewAngles(); //modifies the underlying bones
		void applyTransformation();    //commits all recent changes to the mesh 
		void setMesh(SharkMesh *s){skeleton.setMesh(s);}
		
		void buildAnimations( int totalFrames, GLfloat *segmentRot, int totalAngles  );
                bool buildAngles( GLfloat segmentRot[], int FrameNo, int totalAngles  );
		
		void draw(){skeleton.draw();}

		float scaleRatio(){return sharkRealLength / totalLength;}

		//simulation related functions
		void calcNextAngles(int railAngle);     //automatically gets next keyframe based on the speed of the animated angle rate.
		//takes in the angle of the curve 

		enum{ANGUILIFORM = 1, SUBCARANGIFORM = 2, CARANGIFORM = 3, THUNNIFORM = 4};

	private:
		SharkSkeleton skeleton;
		Vector3f velocity;   //velocity of the shark. magnitude gets the scalar speed

		int framesPerSequence;
		int anglesPerFrame;
		float totalLength;
		float sharkRealLength; //TODO incorporate into totalLength when scaled.
		float segmentLength;

		int bodyPitch;
		vector<int> angles;
		vector<int> oldAngles;			//previous frame's angles
		vector<int> finalAngles;  		//these are the bone angles that will be exported to the Keyframe
		vector<int> curveAngles; 		//these represent the curvature of the spline at a point the shark will be.
		vector<vector<int> > animatedAngles; 	//these are preset, artist driven, bone angles. 
		vector<int> axialAngles;		//rotation along the shark axis
		int caudalLag;				//follow through on the tail fin
		int hedral;  				//angle lateral fins attach to the shark side.
		int betaAng;				//angle of posterior plane of pectoral fin
		int alphaAng;				//angle of anterior plane of pectoral fin
		enum{ HOLD_HED = -23, RISE_HED = -35, SINK_HED = -5, HOLD_POST = -1, HOLD_ANT = -6, RISE_POST = -9, RISE_ANT = 5, SINK_POST = 10, SINK_ANT = -12};
			//pectoral fin angles: hedral angles, plus posterior and anterior planes

		/*Bone names */
		static const string spineKeys[];// = ["Spine1", "Spine2", "Spine3", "Spine4", "Spine5", "Spine6", "Spine7", "Spine8", "Spine9", "Spine10"];
		static const string leftPec;// = "PecLeft";  	//Pectoral Fins
		static const string rightPec;// = "PecRight"; 
		static const string leftPel;// = "PelLeft"; 	//Pelvic Fins
		static const string rightPel;// = "PelRight"; 
		static const string dorsal1;// = "Dorsal1"; 
		static const string dorsal2;// = "Dorsal2"; 
		static const string anal ;//= "Anal"; 
		static const string lowCaudal;// = "Caudal"; 

		int locomotionMode; //the method of locomotion being used currently: anguiliform, subcarangiform, ...etc.
		float propellingAmplitude; //amount of force to put in a swim stroke. Higher for more force. 
				//Often Never exceeds 1/4th-1/5th of the body length. Decreases as the fish gets larger.
		int turningAngle; //amount of turning happening
		float swimFrequency; //frequency of oscillation.  Maximum 5 beats per second
		float elapsedTime; //time (in seconds) since the simulation began. 
		bool beatDirection;  //direction tail is swinging
		bool prevBeatDirection;  //direction tail was swinging last update 

		static float const velocityToAmp = 15.0;  //divides velocity    by this value to determine the amplitude of a stroke

		//angle calculation funcs
		float calcTurningAngle();
		vector<int> getMaxAngles();  //returns the max angle for the current locomotion mode
		int nextSegmentAngle(int index, int prevSegmentAngle, int prevTimeAngle, int maxAngle); //finds the individual angle of a segment.
		int nextAxialAngle(int index, int prevSegmentAngle, int prevTimeAngle, int maxAngle); //finds the individual angle of a segment.
		void findRailCurve(int railAngle);
		void lateralFins();  				//changes lateral fin angles
		float waveAngle(float time, int harmonic, float prevSegmentAngle);
		//need frequency (f), time (t), turning angle (TA),turning coefficeint (K sub i), relational initial angle (beta), propelling amplitude coefficent (K sub a),
		void lowerCeratotrichia();

		int gNumLocomotionBones();

		float deriveFrequency(); //derives frequency of tail swish based on the speed it is moving
};


#endif
