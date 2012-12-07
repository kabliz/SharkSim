#include "SharkParticle.h"

void SharkParticle::sPosition(Vector3f p)
{
	position = p;
}

void SharkParticle::update(float dt) {

	//Euler timestep
	//Vector3f eVel = velocity + acceleration * dt;
	velocity = velocity + (acceleration * dt);
	//Vector3f ePos = position + velocity * dt;
	position = position + (velocity * dt);

	//TODO runge-kutta method
	//Vector3f mVel = 

	//velocity = eVel;
	//position = ePos;

	//reset acceleration for reapplication next round
	acceleration = Vector3f(0,0,0);
}

void SharkParticle::applyForce(Vector3f force){
	acceleration = acceleration + (force / mass);
}

void SharkParticle::applyAcceleration(Vector3f accel)
{
	acceleration += accel;	
}

void SharkParticle::applyGravity()
{
	applyForce(Vector3f(0, -9, 0));
}


void SharkParticle::drawSquare(MyMat trans)
{
	glPushMatrix(); {
		GLfloat multMat[16];
		glMultMatrixf(trans.toGLfloat(multMat));

		if(billboard){		
			glBegin(GL_QUADS);
			{
				glBindTexture(GL_TEXTURE_2D, texIndex);
				glColor3f(0, .6, 1);
				glNormal3f(0, 0, -1);
				glTexCoord2f(0.0, 0.0);
				glVertex3f(-1, -1, 0);
				glTexCoord2f(0.0, 1.0);
				glVertex3f(-1, 1, 0);
				glTexCoord2f(1.0, 1.0);
				glVertex3f(1, 1, 0);
				glTexCoord2f(1.0, 0.0);
				glVertex3f(1, -1, 0);
			}glEnd();
		}
		else
		{
			glColor4f(1,1,1,.1);
			glutSolidSphere(.1, 7, 7);
			glColor4f(1,1,1,1);
		}
	}glPopMatrix();
}

/* needs camera position */
void SharkParticle::draw(Vector3f eyepos)
{
	//billboarding axies
	Vector3f right = ((position-eyepos).Cross(Vector3f(0,1,0)));
	right.Normalize();
	Vector3f cam = (position-eyepos).Normalize();
	Vector3f upp = cam.Cross(right);
	upp.Normalize();

	MyMat trans = MyMat(
			right.x, upp.x, cam.x, 0,
			right.y, upp.y, cam.y, 0,	
			right.z, upp.z, cam.z, 0,
			0,0,0,1	
			);
	//apply particle scale
	MyMat m = MyMat();
	m.makeTranslate(Vector3f(position.x, position.y, position.z));
	trans.multRight(m);
	
	m.makeScale(Vector3f(scale,scale,scale));
	//trans.multRight(m);

	drawSquare(trans);
}

/*given a rotation matrix */
void SharkParticle::draw(MyMat rotat)
{
	MyMat m = MyMat();
	m.makeTranslate(Vector3f(position.x,position.y,position.z));
	rotat.multRight(m);
	
	m = MyMat();	
	m.makeScale(Vector3f(scale,scale,scale));
	//rotat.multRight(m);


	drawSquare(rotat);
}
