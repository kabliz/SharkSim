#ifndef BLENDER_ARMATURE_ANIMATION_IMPORTER_RUNNER
#define BLENDER_ARMATURE_ANIMATION_IMPORTER_RUNNER

#include <map>
#include <string>
#include "ArmTimeline.h"
#include "SharkSkeleton.h"
#include "SharkMesh.h"

class BlendArmAnimation
{
	public:
		BlendArmAnimation(){runningTime = 0;}
		BlendArmAnimation(SharkMesh *smesh, SharkSkeleton* sk){runningTime = 0; skeleton = sk;} 
		~BlendArmAnimation(){}

		void buildSkeleton(string ModelFile, vector<string> actionFiles);  //.aobj file and an action file for each action.
		
		void sCurrentAction(string nextAction){currentAction = nextAction;}  
		void sRandomTimeInAction(string name){gAction(name).setRandomTime();}

		void update(float dt);
		bool isReady(){return gAction(currentAction).isReady();}
		void applyTransformation();

	private:
		SharkSkeleton* skeleton;
		map<string, ArmTimeline>* actions;   //set of actions this skeleton takes. 
		string currentAction;
		float runningTime;		    //total Time this instance has been running

		ArmTimeline gAction(string name){return actions->find(name)->second;}
};

#endif
