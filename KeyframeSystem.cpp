#include "KeyframeSystem.h"

void KeyframeSystem::initFrames()
{
	if(isDynamicMode)
	{
		Keyframe initialFrame = Keyframe(&(sMesh->vertices), &(sMesh->faces));
		sequences.push_back(FrameSequence(isDynamicMode)); //initial (and only) sequence of frames	
		sequences[0].frames.push_back(initialFrame); //First three frames are pushed in.
		sequences[0].frames.push_back(initialFrame);
		sequences[0].frames.push_back(initialFrame);
		lastFrame = sequences[0].frames[0];
		nextFrame = sequences[0].frames[1];
	}
	else
	{
		lastFrame = sequences[0].frames[curFrame];
		nextFrame = sequences[0].frames[nextFrameInd]; 
	}
}

void KeyframeSystem::setFrameSpeed(int newrate)
{
	accFrameSpeed = newrate;
	adjNumFrames(newrate);
}

void KeyframeSystem::adjNumFrames(int newRate)
{
	vector<FrameSequence>::iterator isf;
	for(isf = sequences.begin(); isf < sequences.end(); isf++)
	{
		isf->numInBetweens = newRate;
	}
}

void KeyframeSystem::deleteFrameHeaps()
{
	vector<FrameSequence>::iterator isf;
	for(isf = sequences.begin(); isf < sequences.end(); isf++)
	{
		isf->deleteFrameHeaps();
	}
}

/*blank update frame called */
void KeyframeSystem::update()
{
	if(isDynamicMode)
	{
		updateDynamicMode();
	}
	else
	{
		updateStaticMode();
	}
}

/* */
void KeyframeSystem::updateDynamicMode()
{
	curFrame++;
	//printf("frame %d ... btwn %d\n", curFrame, sequences[0].numInBetweens);
	
	//Next Keyframe Reached, update keyframe pointers to the next keyframe
	if((curFrame >= sequences[0].numInBetweens) && (sMesh->hasNewTransform)) 
	{
		//INtroduce randomness to sequence speed
                int adjFrameSpeed = accFrameSpeed +  (rand() % 12 - 6);
                adjNumFrames(adjFrameSpeed);
		
		//Move Current Frame to Last Frame
		//Move Queued Frame to Current Frame
		//Signal next Queued frame to be created.
		sequences[0].dynamicAdvancement(&(sMesh->vertices), &(sMesh->faces));
		lastFrame = sequences[0].frames[0];
		nextFrame = sequences[0].frames[1];
		sMesh->newUpdateApproved = true; 
		sMesh->hasNewTransform = false; //tell mesh to ready next transform
		curFrame = 0;
		
	}
	//Else, it is In-Betweening, and doesnt need updating besides the current Frame, which has already been updated
}


/*Advances the frame and makes checks for loopback and sequence change automatically */
void KeyframeSystem::updateStaticMode() 
{
	curFrame++;
	//frame updating sequences
	//Next Keyframe reached, update keyframe pointers
	if(curFrame >= sequences[curSequence].numInBetweens-1)
	{
		//INtroduce randomness to sequence speed
		int adjFrameSpeed = accFrameSpeed +  (rand() % 12 - 6);
		adjNumFrames(adjFrameSpeed);

		//reset frame
		curFrame = 0;
		frameInd = nextFrameInd;

		//accounts for looping back to the beginning of the sequence
		nextFrameInd = nextFrameInd >= sequences[curSequence].frames.size()-1 ? 0 : nextFrameInd + 1;
		lastFrame = nextFrame;
		//printf("goose\n");
		//shift over sequences if needed
		if(nextSequence != curSequence)
		{
			curSequence = nextSequence;
			nextFrameInd = 0;	
		}
		nextFrame = sequences[curSequence].frames[frameInd];
	}
	//In Between keyframes 
	else
	{
		//non-loopable sequence check
		if((curSequence == a_left_turn || curSequence == a_right_turn || curSequence == a_hard_left ||
					curSequence == a_hard_right || curSequence == a_right_uturn || curSequence == a_left_uturn))
		{
			if(sequences[curSequence].numInBetweens < 10)
			{
				prepareNextSeq(string("fast straight"));
			}
			else
			{
				prepareNextSeq(string("slow straight"));
			}
		}
	}

}

/*draw the appropriate frame */
void KeyframeSystem::draw()
{

	//if(isDynamicMode)
	{
	//	sequences[0].drawDynamicFrame(curFrame);
	}
	//else
	{
		lastFrame.drawInBetween(nextFrame, curFrame, sequences[curSequence].numInBetweens);
	}

}

/* takes the name of the next sequence and sets it as the next sequence to transion to, 
 * changing the sequence as early as possible 
 */
void KeyframeSystem::prepareNextSeq(string seqName)
{
	if(seqName.compare("no change") == 0)
	{
		//printf("-----1\n");
		return; 
	}

	if(seqName.compare("fast straight") == 0)
	{
		//printf("1\n");
		nextSequence = a_fast_straight;
	}
	else if(seqName.compare("left turn") == 0)
	{
		//printf("2\n");
		nextSequence = a_left_turn;
	}
	else if(seqName.compare("right turn") == 0)
	{
		//printf("3\n");
		nextSequence = a_right_turn;
	}
	else if(seqName.compare("hard left") == 0)
	{
		//printf("2\n");
		nextSequence = a_hard_left;
	}
	else if(seqName.compare("hard right") == 0)
	{
		//printf("3\n");
		nextSequence = a_hard_right;
	}
	else if(seqName.compare("uturn right") == 0)
	{
		//printf("4\n");
		nextSequence = a_right_uturn;
	}
	else if(seqName.compare("uturn left") == 0)
	{
		//printf("4\n");
		nextSequence = a_left_uturn;
	}
	else if(seqName.compare("stiff") == 0)
	{
		//printf("0\n");
		nextSequence = a_stiff;
	}
	else if (seqName.compare("slow straight") == 0)
	{
		//printf("1\n");
		nextSequence = a_slow_straight;
	}
}
