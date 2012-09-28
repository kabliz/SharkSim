#ifndef SHARK_SIMULATOR_MAIN_H_AND_GENERALLY_IMPORTANT_STUFF
#define SHARK_SIMULATOR_MAIN_H_AND_GENERALLY_IMPORTANT_STUFF


//#include <windows.h>				// Header File For Windows
//#include <gl\gl.h>				// Header File For The OpenGL32 Library
//#include <gl\glu.h>			// Header File For The GLu32 Library
//#include <gl\glut.h>
//#include <gl\glui.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glut.h>
//#include<GL/glui.h>
#include "glui/include/GL/glui.h"
#include<ctime>
#include<string>

#include "math.h"			    // Needed For Sqrtf
#include "ArcBall.h"			// ArcBall Header
#include "Shark.h"
#include "MATreader.h"
#include "SharkWorld.h"
#include "Texture.h"
#include "SharkMesh.h"
#include "Frustum.h"

#pragma comment( lib, "opengl32.lib" )		// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )		// Search For GLu32.lib While Linking
#pragma comment( lib, "glaux.lib" )		// Search For GLaux.lib While Linking
#pragma comment( lib, "glut32.lib" )
#pragma comment( lib, "glui32.lib" )

#ifndef CDS_FULLSCREEN				// CDS_FULLSCREEN Is Not Defined By Some
#define CDS_FULLSCREEN 4			// Compilers. By Defining It This Way,
#endif						// We Can Avoid Errors

extern ArcBallT ArcBall;                     //*NEW* ArcBall instance
extern Point2fT MousePt;                     //*NEW* Current mouse point
extern bool isClicked;                   //*NEW* Clicking the mouse?
extern bool isRClicked;                  //*NEW* Clicking the right mouse button?
bool isLight;
int currentButton;

//mouse trackball controls
ArcBallT ArcBall(640.0f, 480.0f);			// NEW: ArcBall Instance
Point2fT MousePt;					// NEW: Current Mouse Point

Frustum frustum_;
//update variable
int prevTime = 0;  //the previous update's timestamp, in miliseconds. Used to calculate how much time has passed since the last update

// User Defined Variables
GLUquadricObj *quadratic;		// Used For Our Quadric


Matrix4fT   Transform   = {  1.0f,  0.0f,  0.0f,  0.0f,	// NEW: Final Transform
                             0.0f,  1.0f,  0.0f,  0.0f,
                             0.0f,  0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  0.0f,  1.0f };

Matrix3fT   LastRot     = {  1.0f,  0.0f,  0.0f,	// NEW: Last Rotation
                             0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  1.0f };

Matrix3fT   ThisRot     = {  1.0f,  0.0f,  0.0f,	// NEW: This Rotation
                             0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  1.0f };

//shark globals
Mesh mesh;  //points and meshReader system. Used to parse blender files.
SharkMesh universalMesh;   //a "smart mesh" that's made of SharkVertices and closes its own gaps.
Shark	Shark;   //the basis for the Shark itself
MATreader MATparser;  //parsers
SharkWorld world1; //SharkWorld(&frustum_, "xytData.mat"); //The world in which points are drawn



//texture stuff
extern Image *TextureImage;
extern RGB myimage[64][64];
extern RGB* pixel;
Texture TexLoad;

//baic animation things
double	frame = 0;   //deprecated, for manually skimming through frames
double	frameSpeed = 0;  //deprecated
double	frameSpeedSlow = 0.001; //deprecated
double	frameSpeedFast = 0.01; //deprecated

//shark external files
static const int numArtistAnimations = 5;
string artistDrivenAnimations[numArtistAnimations]= {"ani_slowstraight_4.csv", 
						     "ani_faststraight_1.csv", 
						     "ani_leftturn_2.csv",
       			        		     "ani_rightturn_2.csv", 
						     "ani_right_uturn_1.csv"};  //for non-dynamic animation mode


bool	showWorld = true; //for closeups verses looking at world
bool    dynaMode = true; //TODO. broken backwards compatibility
int	wireframe = 0;  //for wireframe mode.
int	main_window;

#endif
