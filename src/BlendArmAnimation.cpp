#include "BlendArmAnimation.h"


void BlendArmAnimation::buildSkeleton(string modelFile, vector<string> actionFiles)
{
	skeleton->buildSkeletonAOBJ(modelFile);    //geometry of the skeleton

	//create each action. ArmTimeline initialization
	for(int i = 0; i < actionFiles.size(); i++)
	{
		ArmTimeline newTim = ArmTimeline();
		newTim.buildKeyframes( actionFiles[i] );
		string name = newTim.gName(); 
		(*actions)[name] = newTim;
	}
}

void BlendArmAnimation::update(float dt)
{
	gAction(currentAction).update(dt);	
	applyTransformation();
}

void BlendArmAnimation::applyTransformation()
{
	if(isReady())
	{
		skeleton->applyTransformation();
	}
}
