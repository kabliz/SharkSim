#ifndef ARMATURE_ANIMATION_KEYFRAMES_MODEL
#define ARMATURE_ANIMATION_KEYFRAMES_MODEL

#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <cstdio>
#include "ArmFrame.h"
#include "glQuaternion.h"

/* Timeline keeper for one instance of a skeletal animated figure. 
 * Collection of armature keyframes 
 * This represents one loop/action of animtion*/
class ArmTimeline
{
	public:
		ArmTimeline(){mode = LOOP;}
		~ArmTimeline(){}

		void buildKeyframes(string fileName); 	//sets up all of the frames by reading in each file
		void clearHeap();
	     
		void update(float dt);	
		map<string, glQuaternion> gNextKeyframe(){return frames.at(nextFrame)->allAngles();}   //get the next keyframe after the specified time.		
		map<string, glQuaternion> gKeyframe(int index){return frames.at(index)->allAngles();}   //get the next keyframe after the specified time.		
		map<string, glQuaternion> gAheadKeyframe(){return frames.at(futureFrame)->allAngles();}   //get the next keyframe after the specified time.		
		
		void setRandomTime(){elapsedTime = (float) (rand() % ((int)floor(length)));}		//sets the time to a random part of the action.
		float gNextKeyTime(){return frames.at(nextFrame)->gTime();}
		bool isReady(){return ready;}

		//gMode(){return mode;}
		void sMode(int newmode){mode = newmode;}
		string gName() {return actionName;}


		enum{PLAY_ONCE, LOOP, BOUNCE};    //modes of playback
						//LOOPed animations need the last keyframe to be NOT the same as the first.
	private:
		vector<ArmFrame*> frames;  //must be ordered by time.
		string actionName;         //name of this timeline
		int mode;
		int curFrame;		   //current index with the frames	
		int nextFrame;
		int futureFrame;
		float length;              //length (in seconds) of this animation
		float elapsedTime; 	   //currentRunningTime. 

		bool ready; 		//if it is time to export a frame

};

#endif

