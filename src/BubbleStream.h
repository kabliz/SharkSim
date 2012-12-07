#ifndef BUBBLES_IN_A_STREAM_FLOATING_UP
#define BUBBLES_IN_A_STREAM_FLOATING_UP

#include "SharkParticle.h"
#include <vector>
#include <cmath>
#include <cstdio>

class BubbleStream
{
	public:
		BubbleStream(int tInd){texIndex = tInd;}
		~BubbleStream(){}
		void update(float dt);
		void sTextureIndex(int tInd){texIndex = tInd;}
		void draw(Vector3f eyePos);
	private:
		vector<SharkParticle> bubbles;
		int texIndex;
};

#endif
