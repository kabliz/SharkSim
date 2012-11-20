#include "SharkVertex.h"

//returns the weight of the bone if this vertex belongs to it.
float SharkVertex::checkBone(string boneName)
{
	for(int i = 0; i < bones.size(); i++)
	{
		if(gBoneName(i).compare(boneName) == 0)
		{
			return gBoneWeight(i);
		}	
	}
	return 0;

}

