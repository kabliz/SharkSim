#ifndef ANIMATION_KEYFRAME_ARMATURE_THING
#define ANIMATION_KEYFRAME_ARMATURE_THING

#include <map>
#include <string>
#include "glQuaternion.h"
#include "SharkBone.h"

/* Represents a keyframe in a skeletal system.  */
class ArmFrame
{
	public:
		ArmFrame(){angles = map<string, glQuaternion>(); time = 0;}
		~ArmFrame(){}

		void buildAngles(string keyframeFilename);

		void sTime(float dt){time = dt;}
		float gTime(){return time;}

		glQuaternion gAngle(string boneName){return angles.find(boneName)->second;}
		map<string, glQuaternion> allAngles(){return angles;}	

	private:
		map<string, glQuaternion> angles;	//angles the bones are at this keyframe
		float time;				//time during cycle this keyframe occurs.  in seconds.
};

#endif

