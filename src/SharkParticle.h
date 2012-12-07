#ifndef SHARK_PARTICLE_FOR_PHYSICS
#define SHARK_PARTICLE_FOR_PHYSICS

#include "Vector.h"
#include "Texture.h"
#include "MyMat.h"
#include <GL/glut.h>

class SharkParticle
{

	public:
		SharkParticle(float pMass){mass = pMass; billboard = false;}
		~SharkParticle(){}
		void update(float dt);
		void applyForce(Vector3f force);
		void applyGravity();
		void applyAcceleration(Vector3f accel);
		
		void sPosition(Vector3f pos);
		Vector3f gPosition(){return position;}
		void sScale(float ns){scale = ns;}
		void sCollisionRadius(float cs){collisionRadius = cs;}
		void sTextureIndex(int ind){texIndex = ind;}

		void draw(Vector3f eyepos);
		void draw(MyMat rotationMatrix);

	private:
		float mass;
		Vector3f position;
		Vector3f velocity;
		Vector3f acceleration;
		int texIndex; 		//index texture is loaded in GL
		float scale;
		float collisionRadius; 	//spherical collision detection
		bool billboard;

		void drawSquare(MyMat transformComplete); //drawing the texture without transformations
};


#endif
