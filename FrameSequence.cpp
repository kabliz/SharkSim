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
	//printf("dd %f verses %f\n", frames[0].faces[0]->verts[0]->transformed.x, frames[1].faces[1]->verts[1]->transformed.x);
	if(numInBetweens > 0)
	{
		//frames are not advancing down the queue correctly.
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
		

		/*Now, create a new Keyframe from the shark mesh*/
		Keyframe newQueueFrame = Keyframe(sVertices, quads);
		//pointer problem?
		frames.erase(frames.begin()); //Moves all the other frames up.
		frames.push_back(newQueueFrame);
		

		//printf("dd %f,%f \n11 %f,%f \n22 %f,%f \n\n", frames[0].faces[0]->verts[0]->transformed.x, 
		//				frames[0].faces[0]->verts[0]->transformed.z,
		//				frames[1].faces[0]->verts[0]->transformed.x,
		//				frames[1].faces[0]->verts[0]->transformed.z,
		//				frames[2].faces[0]->verts[0]->transformed.x,
		//				frames[2].faces[0]->verts[0]->transformed.z);
		//TODO. figure out this problem. buggy. frames appear to NOT cascade down the list. they are equal. queue always size 3
		//TODO. reorganize objects to self contain themselves. no more dot trainwrecks
	}
	else
	{
		printf("Dynamic mode not set when trying to advance.\n");
	}
}
