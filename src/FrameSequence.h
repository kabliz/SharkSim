#ifndef _3D_CUT_SEQUENCE_
#define _3D_CUT_SEQUENCE_

#include "Keyframe.h"

class FrameSequence
{
	public:
		FrameSequence(){ numInBetweens = 10;}
		FrameSequence(bool mode){ numInBetweens = 10; isDynamicMode = mode; }
		~FrameSequence(){}	
		void drawFrame(int frameNo);
		void drawDynamicFrame(int frameNo);
		void dynamicAdvancement(map<Vector3f, SharkVertex*, compareVect3> *sVertices, vector<Quad*> *quads );
		void deleteFrameHeaps();	
		void sNumInBetweens(int d){numInBetweens = d;}
		int gNumInBetweens(){return numInBetweens;}
		bool inDynamicMode(){return isDynamicMode;}
		Keyframe gFrame(int index){return frames[index];}
		void sFrame(Keyframe frame){frames.push_back(frame);}
		size_t size(){return frames.size();}

	private:	

		vector<Keyframe> frames;
		int numInBetweens;
		bool isDynamicMode;	
};

#endif
