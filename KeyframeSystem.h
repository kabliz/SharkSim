#ifndef _3D_SHARK_SYSTEM_TO_DO_KEYFRAMES_WITH
#define _3D_SHARK_SYSTEM_TO_DO_KEYFRAMES_WITH
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include "Vector.h"
#include "Quad.h"
#include "FrameSequence.h"
#include "SharkMesh.h"

/*The Main playback system. Manages sequences of individual Keyframes (and their inbetweeners)
 * Dynamic Mode expects SharkMesh to periodically update with more Transformations that will be cnverted to Keyframes.
 * Static Mode expects that animation sequences are preloaded entirely to make multiple sequences to switch between. Static Mode functionality not guarenteed, currently..
 * */
class KeyframeSystem
{
	public:
		KeyframeSystem(){curFrame = 0; curSequence = 0; nextSequence = 0; 
					frameInd = 0; nextFrameInd = 1; isDynamicMode = true;
					}
		KeyframeSystem(bool DynamicSystem){isDynamicMode = DynamicSystem; 
					curFrame = 0; curSequence = DynamicSystem ? 0 : 1; nextSequence = curSequence; 
					frameInd = 0; nextFrameInd = 1;}
		~KeyframeSystem(){}
		void setFrameSpeed(int newFrameRate);
		void deleteFrameHeaps(); //free this memory before exiting.
		void initFrames();
		void adjNumFrames(int newFramerate);  //change velocity of framerate
		void decreaseFrames(){setFrameSpeed(sequences[0].gNumInBetweens() > 1 ? sequences[0].gNumInBetweens()-1 : 1 );}
		void increaseFrames(){setFrameSpeed(sequences[0].gNumInBetweens()+1);}
		void update();  //c
		void draw(); //draws the correct frame
		void prepareNextSeq(string SeqName); //accepts "fast straight" "slow straight" "left turn" 
						//"right turn" "hard left" "hard right" "uturn left" "uturn right"
		int getNumFrames(){return sequences[0].gNumInBetweens();} //get number of inbetweens for this current frame

		//animation index keys //TODO deprecated
		static const int a_stiff = 0;
		static const int a_slow_straight = 1;
		static const int a_fast_straight = 2;
		static const int a_left_turn = 3;
		static const int a_right_turn = 4;
		static const int a_hard_left = 3;
		static const int a_hard_right = 4;
		static const int a_right_uturn = 5;
		static const int a_left_uturn = 5;

		void sNextSequence(int ind){nextSequence = ind;}
		int gNextSequence(){return nextSequence;}
		int gCurrentSequence(){return curSequence;}
		void incrementNextSequence(){nextSequence++;}
		void setSharkMesh(SharkMesh* mesh){sMesh = mesh;}
		void insertStaticSequence(FrameSequence n){sequences.push_back(n);}  //TODO handle multiple static sequences

	private:
		//global animation controls
		bool isDynamicMode;  //determines whether to dynamically compute keyframe during runtime, or use the predefined art assets.
		int accFrameSpeed;
		int adjFrameSpeed;
		int curSequence; 
		int nextSequence; //set when sequence is about to change
		int curFrame;   //points to the inbetweener frame being rendered or imterpolated
		int tmpFrame;  //used for transitioning sequences
		Keyframe lastFrame;  //making curFrame less relevant
		Keyframe nextFrame;
		int frameInd;
		int nextFrameInd;
		vector<FrameSequence> sequences;
		SharkMesh *sMesh;

	//private:
		void updateStaticMode();
		void updateDynamicMode();
};

#endif
