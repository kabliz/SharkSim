#ifndef SHARK_MAIN_SYSTEM
#define SHARK_MAIN_SYSTEM

//#include <GL\glui.h>
//#include <GL/glui.h>
#include "glui/include/GL/glui.h"
#include "Mesh.h"
#include "glQuaternion.h"
#include "Keyframe.h"
#include "FrameSequence.h"
#include "KeyframeSystem.h"
#include "SharkSkeleton.h"

typedef struct materialStruct {
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  GLfloat shininess[1];
} materialStruct;


/*Uniting class of all shark subcomponents. This is where the simulation (SharkSkeleton) and playback (KeyframeSystem) both interface.
 * It keeps track of all the controls that are set. */
class Shark
{
public:
	Shark();
	void drawSharkSeg(float rotate, float start, float end, int index, float length);
	void drawSkin(int frame);
	void drawSegment(int index, float rot, GLUquadricObj *quadratic);
	void drawSpine(int frame, GLUquadricObj *quadratic);
	void drawShark(int frame, GLUquadricObj *quadratic);
	void timedUpdate(int railAngle);
	void materials(materialStruct materials);
	void segUpdate();
	KeyframeSystem genKeyframes(bool dynamicMode, SharkMesh *sharkmesh);
	void deinitialize(){kfSys.deleteFrameHeaps();}
	void updateVelocity(Vector3f start, Vector3f end, double dt);

public:
	float totalLength;		//total length of shark (length of skin)
	int showSpine;			//toggle to draw the spine
	int showSkin;			//toggle to draw the skin
	int moving;				//toggle shark movement
	int play;				//toggle shark movement frame by frame

	//used to draw spine
	static const int segments = 10;	//hardcode number of segments for how many segments we have data for
	double	segPercent;			//percent of the totalLength 1 segment is (used in segUpdate)
	double	lengthLeft;			//amount of shark not with specified length (used in segUpdate)
	static const int sequences = 10;                  //number of sequences  (animation loops) the shark has
	int sequencesParsed;		//notes how many sequences has been parsed so far

	/* GLUI Globals */
	int segMin;				//minimum amount of segments allowed
	static const int segMax = 20;	//maximum amount of segments allowed
	
	//used in GLUI
	int tempSegments;		//used in segUpdate and GLUI interface
	int segNum;				//counter in segUpdate and GLUI
	int tempSegPercent;		//used in segUpdate

	//segment information. Passed onto the Skeleton system
	GLfloat	segmentRot[sequences][segments][segments];		//contains movement data which we get from a file
	GLfloat	segLengthInput[segMax];				//used to tell if user has specified segment lengths
	GLfloat	segLength[segMax];					//used to draw spine
	KeyframeSystem kfSys;
	SharkSkeleton skeleton;

private:
	Mesh	mesh;
	glQuaternion glQuat;
	
        
};


#endif
