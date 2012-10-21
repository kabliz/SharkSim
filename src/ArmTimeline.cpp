#include "ArmTimeline.h"

void ArmTimeline::buildKeyframes(string filename)
{
	//open file
		//for each item, read the new filename and build the ArmFrame with it
		//save new ArmFrame into the map
	//close file
	
	length = frames.at(frames.size()-1)->gTime();	
}

/*wipes out the heap memeory for this set of frames */
void ArmTimeline::clearHeap()
{
}

void ArmTimeline::update(float dt)
{
	ready = false; 

	elapsedTime += dt;
	if(mode == LOOP && elapsedTime > length)  //go back to the beginning
	{
		elapsedTime - length;
		nextFrame = futureFrame;
		futureFrame = 1;
	}
	//odo bounce
	if(elapsedTime > gNextKeyTime())
	{
		//change over
		curFrame = nextFrame;
		nextFrame = futureFrame;
		futureFrame++;

		//set flag to export frame now?
		ready = true;
	}

}
