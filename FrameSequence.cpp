#include "FrameSequence.h"

void FrameSequence::deleteFrameHeaps()
{
	vector<Keyframe>::iterator ikf;
	for(ikf = frames.begin(); ikf < frames.end(); ikf++ )
	{
		ikf->deleteHeap();
	}
}

void FrameSequence::drawFrame(int frameNo)
{
	if(numInBetweens > 0)
	{
		int keyframeNo = (frameNo) / (numInBetweens);
		if(keyframeNo >= frames.size()) 
		{
			//keyframeNo = keyframeNo - 1;
			keyframeNo = 0;
		}
		int nextNo = keyframeNo + 1 >= frames.size()? 0 : keyframeNo + 1;
		//printf("<%d %d %d >\n", frameNo, keyframeNo, numInBetweens);
		frames[keyframeNo].drawInBetween(frames[nextNo], frameNo % numInBetweens, numInBetweens);
	}
	else
	{
		frames[frameNo].draw();
	}
}

void FrameSequence::drawDynamicFrame(int frameNo)
{
	if(numInBetweens > 0)
	{
		
		//printf("fish frame: %d, outOf %d \n", frameNo%numInBetweens, numInBetweens);
		frames[0].drawInBetween(frames[1], frameNo , numInBetweens);
	}
	else
	{
		frames[0].draw();
	}
}

/*call only when the mesh is ready to have info taken from it. */
void FrameSequence::dynamicAdvancement(map<Vector3f, SharkVertex*, compareVect3> *sVertices, vector<Quad*> *quads )
{
	if(isDynamicMode)
	{
		
		//printf("nextKeyframe\n");
		frames.erase(frames.begin()); //Moves all the other frames up.

		/*Now, create a new Keyframe from the shark mesh*/
		Keyframe newQueueFrame = Keyframe(sVertices, quads);
		frames.push_back(newQueueFrame);
		//printf("dd %f verses %f\n", frames[0].faces[0]->verts[0]->transformed.z, frames[2].faces[2]->verts[2]->transformed.z);
		//TODO. figure out this problem. buggy. frames appear to be qeual, and hence there is not interpolation going on.
	}
	else
	{
		printf("Dynamic mode not set when trying to advance.\n");
	}
}
