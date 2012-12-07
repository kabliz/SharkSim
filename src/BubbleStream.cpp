#include "BubbleStream.h"

void BubbleStream::update(float dt)
{
	//add any bubbles to simulation when there aren't enough
	if((bubbles.size() < 12) && (((int) (dt*1000)) % 1000 <= 50))
	{
		SharkParticle nPart = SharkParticle(4);
		nPart.sTextureIndex(texIndex);
		nPart.sScale(1);
		nPart.sPosition(Vector3f(0, 0, 0)); 
		bubbles.push_back(nPart);
	}

	//Impart new accelerations on particles
	for(int i = 0; i < bubbles.size(); i++)
	{
		bubbles[i].applyAcceleration(Vector3f(0,2,0));  //water forcing bubble upward
		//bubbles[i].applyForce(Vector3f(sin(dt+(i/2)),0, cos(dt-i))); 	//random sideways water resistance 
	}

	//Physics updates for delta time.
	int removeIndex = -1;
	for(int i = 0; i < bubbles.size(); i++)
	{
		bubbles[i].update(dt);
		if(bubbles[i].gPosition().y > 10)
		{
			printf("bubbes %f \n", bubbles[i].gPosition().y);
			removeIndex = i;
		}
	}

	if(removeIndex >= 0) //cull bubbles that are too high
	{
		bubbles.erase(bubbles.begin()+removeIndex);
	}
	
}


void BubbleStream::draw(Vector3f eyePos)
{
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);
	//
	for(int i = 0; i < bubbles.size(); i++)
	{
		bubbles[i].draw(eyePos);
	}
	//glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

}
