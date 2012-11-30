#ifndef _3D_SHARK_VERTEX_STRUCTURE_TRACKER
#define _3D_SHARK_VERTEX_STRUCTURE_TRACKER

#include <cstdlib>
#include <vector>
#include <utility>
#include <string>
#include "Vector.h"

class SharkVertex
{
	public:
		SharkVertex(){bones = vector<pair<string,float> >();}
		~SharkVertex(){}
		Vector3f gLocal(){return local; }
		Vector3f gTransformed(){return transformed;}
		Vector3f gNormal(){return normal;}
		void sNormal(Vector3f n){normal = n;}
		void sTransformed(Vector3f n){transformed = n;}
		void sLocal(Vector3f n){local = n;}
		int gNumInfluences(){return bones.size();}
		string gBoneName(int index){return bones[index].first;}
		float gBoneWeight(int index){return bones[index].second;}
		void sBoneWeight(int index, float newWeight){bones[index].second = newWeight ;}
		void sBonePair(pair<string,float> f){bones.push_back(f);}
		void sBonePair(string boneName, float boneWeight){sBonePair(pair<string, float>(boneName, boneWeight));}
		float checkBone(string boneName);

	//private:
		Vector3f local;    
		Vector3f transformed;  //vertex after transformation
		Vector3f normal;
		vector<pair<string,float> > bones;
};

#endif

