#include "FrameSequence.h"

void FrameSequence::deleteFrameHeaps()
{
	vector<Keyframe*>::iterator ikf;
	for(ikf = frames.begin(); ikf < frames.end(); ikf++ )
	{
		(*ikf)->deleteHeap();
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
		frames[keyframeNo]->drawInBetween(*frames[nextNo], frameNo % numInBetweens, numInBetweens);
	}
	else
	{
		frames[frameNo]->draw();
	}
}

void FrameSequence::drawDynamicFrame(int frameNo)
{
	//printf("dd %f verses %f\n", frames[0].faces[0]->verts[0]->transformed.x, frames[1].faces[1]->verts[1]->transformed.x);
	if(numInBetweens > 0)
	{
		//frames are not advancing down the queue correctly.
		//printf("fish frame: %d, outOf %d \n", frameNo%numInBetweens, numInBetweens);
		frames[0]->drawInBetween(*frames[1], frameNo , numInBetweens);
	}
	else
	{
		frames[0]->draw();
	}
}

/*call only when the mesh is ready to have info taken from it. */
void FrameSequence::dynamicAdvancement(map<Vector3f, SharkVertex*, compareVect3> *sVertices, vector<Quad*> *quads )
{
	if(isDynamicMode)
	{
		
		//printf("nextKeyframe\n");
		

		/*Now, create a new Keyframe from the shark mesh*/
		Keyframe * newQueueFrame = new Keyframe(sVertices, quads);
		//pointer problem?

		delete *frames.begin();	//dealocate the old keyframe
		frames.erase(frames.begin()); //Moves all the other frames up.
		frames.push_back(newQueueFrame);
		
	}
	else
	{
		printf("Dynamic mode not set when trying to advance.\n");
	}
}
