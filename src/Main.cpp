/*
   Katherine Blizard

Code used from: 
+Jeff Molofee's Basecode Example @ nehe.gamedev.net
+GLUI example: http://www.eng.cam.ac.uk/help/tpl/graphics/using_glui.html
+Greg Ostrowski's senior project
*/
#include "Globals.h"
//bool isLight;

void initLighting(void) 
{
	GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { -0.5, -1.0, -2.0, 0.0 };
	GLfloat model_ambient[] = { 1.0 ,1.0 ,1.0 , 1.0 };
	GLfloat model_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, model_specular);
	isLight = 1;
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);

}


void toggleLighting(void)
{
	if(isLight == 1)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		isLight = 0;
	}
	else
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		isLight = 1;
	}
}
//fast way to check if Glut encountered an error
void chkGlError()
{
	char* err = (char*) gluErrorString(glGetError());
	if(string(err).compare("no error") != 0)
	{
		printf("%s\n", err );
	}
}
/* parses CSV files containing animation data. Not all CSV's are animation. Some of them are world data points. */
void readMovementData(const char* file)
{
	Shark.readMovementData(file, dynaMode);

	/*FILE * fp;
	  fp = fopen(file, "r");
	  if(fp == NULL)
	  {
	  printf("can't open file %s\n", file);
	  exit(-1);
	  }
	  printf("opening file: %s\n", file);
	  for(int i = 0; i < Shark.segments; i++)
	  {
	  for(int j = 0; j < Shark.segments; j++)
	  {
	//read in file. Note that CALShark writes segment data (j value) backwards
	if(!dynaMode)
	{
	fscanf(fp, "%f,",&Shark.gSegmentRotation(Shark.gParsedSoFar(), i, Shark.segments-1-j));
	}
	else
	{
	fscanf(fp, "%f,",&Shark.gSegmentRotation(Shark.gParsedSoFar(),j,Shark.segments-1-i));
	}
	}
	}
	fclose (fp);
	Shark.incrementSequences();*/
}

/*default shark animation sequence creator. Makes a shark that stays stiff */
void defSequence()
{
	Shark.defSequence();
	/*for(int i = 0; i < 2; i++)
	  {
	  for(int j = 0; j < Shark.segments; j++)
	  {
	  Shark.segmentRot[0][i][j] = 0;
	  }
	  }
	  Shark.incrementSequences();
	  */
}

/*Checks for the current window */
void checkWindow(){
	/* According to the GLUT specification, the current window is
	 *          *      undefined during an idle callback.  So we need to explicitly change
	 *                   *           it if necessary */
	if(glutGetWindow() == 0) {
		printf("Window destroyed\n");
	}
	if ( glutGetWindow() != main_window ){
		glutSetWindow(main_window);
	}
}

/*Called only once. This initializes everything in the program */
void Initialize (string splineFile )					// Any GL Init Code & User Initialiazation Goes Here
{

	srand(time(NULL));

	quadratic=gluNewQuadric();			// Create A Pointer To The Quadric Object
	gluQuadricNormals(quadratic, GLU_SMOOTH);	// Create Smooth Normals
	gluQuadricTexture(quadratic, GL_TRUE);		// Create Texture Coords

	initLighting();
	frustum_ = Frustum();

	//read blender mesh
	//mesh.init_blender();
	//initialize segLength
	/*for(int i = 0; i < Shark.segMax; i++)
	{
		Shark.sSegmentLength(i, 0);
	}*/
	//Shark.segUpdate();
	//put in stiff (defualt case )sequence
	defSequence();	
	//read in recorded sequences, in the order they are enumerated in KeyframeSystem
	//readMovementData("straight10s2.csv");

	/*Manual Animation data */
	for(int i = 0; i < numArtistAnimations; i++) {
		readMovementData(artistDrivenAnimations[i].c_str());
	}	
	
	//MATparser.parseFile("velandaccData.mat");
	//MATparser.parseFile("xytData.mat");
	//MATparser.parseFile("oxyMaps.mat");

	//world1.ereader.parseFile("Shovelnose.csv");
	//world1.gatherZPoints();
	//world1.mreader.parseFile("xytData.mat");
	//world1.gatherDTPoints();
	world1 = SharkWorld(&frustum_, splineFile );
	//world1 = SharkWorld(&frustum_, "xytData.mat" );
	universalMesh = new SharkMesh();

	Shark.sMesh(universalMesh);
	int tSegments = Shark.segments;
	//Shark.buildSkeleton(&mesh, tSegments);
	#ifdef VAR_NOSEG
	Shark.buildSkeleton("Model/LeopardShark.aobj");
	#else
	Shark.buildSkeleton("Model/triakisMoreSpine.aobj");
	#endif
	Shark.genKeyframes(dynaMode, universalMesh);
	frameSpeed = frameSpeedSlow;
	Shark.sFrameSpeed(10);

	

	//load textures
	TexLoad = Texture();
	TexLoad.LoadAll();

	b1 = BubbleStream(3);

	//Fog features here
	GLfloat fdensity = 0.03;
	GLfloat fogColor[4] = {0.0, 0.1, 0.2, 0.1}; 
	glEnable (GL_FOG);
	glFogi (GL_FOG_MODE, GL_LINEAR);
	glFogfv (GL_FOG_COLOR, fogColor);
	glFogf (GL_FOG_DENSITY, fdensity);
	glFogf (GL_FOG_START, 1);
	glFogf (GL_FOG_END, 145);
	glHint (GL_FOG_HINT, GL_NICEST);
	//
}

void reshape (int w, int h)		// Reshape The Window When It's Moved Or Resized
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h) {
		glOrtho(-2.5, 2.5, -2.5*(GLfloat)h/(GLfloat)w,
				2.5*(GLfloat)h/(GLfloat)w, -10.0, 1000.0);
	}
	else {
		//gluPerspective(60.0, (float)w/h, 0.5, 15.0);
		glOrtho(-2.5*(GLfloat)w/(GLfloat)h, 2.5*(GLfloat)w/(GLfloat)h, -2.5, 2.5, -10.0, 1000.0);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();					// Reset The Modelview Matrix

	ArcBall.setBounds((GLfloat)w, (GLfloat)h);          //*NEW* Update mouse bounds for arcball
}

/*The glut idle function is the update function for the rest of the program, excluding draw functions */
void idle( void )
{
	checkWindow();

	//Framerate calculation
	//how much time has elapsed     
	int time = glutGet(GLUT_ELAPSED_TIME);
	int dt = time - prevTime;
	prevTime = time;


	//update calls 	
	Shark.timedUpdate(dt, world1.deriveRailAngle(), world1.gVelocity());  //TODO factor in dt in Shark update  
	if(Shark.isMoving()){					//increment movement frame
		if(showWorld) 
		{
			world1.updateWorld(dt); 
			Shark.updateVelocity(world1.gCurrentPoint(), world1.gNextPoint(),  //TODO ???????? put Shark update in one call w/ dt
						world1.gCurrentDTS());
			Shark.prepareNextSeq(world1.gAnimationLoop());  
			//b1.update(dt/1000.0);
		}
	}
		

	glutPostRedisplay();
}

/*glut visible function toggles when the window gains or loses focus */
void visible(int vis)
{
	if (vis == GLUT_VISIBLE)
	{
		glutIdleFunc(idle);
	}
	else {
		glutIdleFunc(NULL);
	}
}


void Deinitialize (void)// Any Application DeInitialization Goes Here
{
	gluDeleteQuadric(quadratic);
	Shark.deinitialize();	
	universalMesh->deleteHeap();
	world1.deleteHeap();
	delete universalMesh;
}

void keyboard(unsigned char key, int x, int y)
{
	switch ( key )
	{
	case 27: //ESC
		exit(0);
		Deinitialize();
		break;
	case 'w': case 'W':
		if(wireframe)
			wireframe = 0;
		else
			wireframe = 1;
		glutPostRedisplay();
		break;
	case 'r': case 'R':
		world1.resetTime();
		break;
	case 's': case 'S':
		if(Shark.isSpine())
			Shark.toggleSpine(false);
		else
			Shark.toggleSpine(true);
		glutPostRedisplay();
		break;
	case 'v': case'V':
		globalWorldScale *= 0.8;
		break;
	case 'b': case'B':
		globalWorldScale *=1.2;
		break;
	case 'k': case 'K':
		if(Shark.isSkin())
			Shark.toggleSkin(false);
		else
			Shark.toggleSkin(true);
		glutPostRedisplay();
		break;
	case 'm': case 'M':
		if(Shark.isMoving())
		{
			Shark.toggleMoving(false);
			//Shark.kfSys.prepareNextSeq(string("stiff"));
		}
		else
			Shark.toggleMoving(true);
		break;
	case ',':
		//frameSpeed = frameSpeedSlow;
		//Shark.decreaseFrames();  //TODO investigate increase/decreaseFrames for proper speed
		world1.slowDown();
		break;
	case '.':
		//frameSpeed = frameSpeedFast;
		//Shark.increaseFrames();
		world1.speedUp();
		break;
	case '[':
		//frame++;
		//if(frame >= Shark.segments)
		//	frame = 0;
		break;
	case ']':
		//frame--;
		//if(frame < 0)
		//	frame = Shark.segments-1;
		break;
	case 'p': case 'P':
		if(Shark.isPlay())
			Shark.togglePlay(false);
		else 
			Shark.togglePlay(true);
		break;
	case 'l':case'L':
		toggleLighting();
		break;
	case 'n':case'N':  //cycle through animations
		//if(Shark.gLoopSequence()+1 >= Shark.gParsedSoFar())
		//	Shark.sLoopSequence(0);
	//	else
	//		Shark.nextLoopSequence();
		break;
	case 'z':case'Z':
		showWorld = !showWorld;	
	}

}

void mouseClick(int button, int state, int x, int y)
{
	currentButton = button;

	if (currentButton == GLUT_LEFT_BUTTON)		// First Click
    {
		if(state == GLUT_DOWN)
		{
			MousePt.s.X = (GLfloat)x;	//Update Mouse Position
			MousePt.s.Y = (GLfloat)y;
			LastRot = ThisRot;		// Set Last Static Rotation To Last Dynamic One
			ArcBall.click(&MousePt);
		}
    }
	if (currentButton == GLUT_RIGHT_BUTTON)		// If Right Mouse Clicked, Reset All Rotations
    {
		Matrix3fSetIdentity(&LastRot);								// Reset Rotation
		Matrix3fSetIdentity(&ThisRot);								// Reset Rotation
        Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);		// Reset Rotation
    }
	glutPostRedisplay();
}

void mouseClickMove(int x, int y)
{
	Quat4fT     ThisQuat;

	MousePt.s.X = (GLfloat)x;		//Update Mouse Position
	MousePt.s.Y = (GLfloat)y;
    ArcBall.drag(&MousePt, &ThisQuat);				// Update End Vector And Get Rotation As Quaternion
    Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);		// Convert Quaternion Into Matrix3fT
    Matrix3fMulMatrix3f(&ThisRot, &LastRot);			// Accumulate Last Rotation Into This One
    Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One

	glutPostRedisplay();

}

//void gluiUpdate() {
	//Shark.segUpdate();
//}


/* A timed update system, that is called a few times per second. the paramater is required by glutTimerFunc. it doesnt matter what is in it
 * */
void TUpdate(int timed)
{
	printf("framerate %d\n", FPScount);
	FPScount = 0;
	glutTimerFunc(1000, TUpdate, 1);
}

void Draw ()
{

	FPScount++;	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();					// Reset The Current Modelview Matrix

	if(wireframe)
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );	//draw as a wireframe
	else
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );	//draw filled polygons

    glPushMatrix();
    {

	glMultMatrixf(Transform.M);
	glTranslatef(1, 0, 0);					//center the shark
	glPushMatrix();
	{
		if(showWorld)
		{
			glPushMatrix();
			{
				glScalef(globalWorldScale, globalWorldScale, globalWorldScale);
				world1.displayWorld();                  //ocean around the shark
			}glPopMatrix();
		}
		glPushMatrix();
		{	
			//yaw rotation										
			glQuaternion Quat;
			Vector3f ax = world1.gRotationAxis();
			//Quat.CreateFromAxisAngle(ax.x, ax.y, ax.z, world1.gRotationDegrees());
			Quat.CreateFromAxisAngle(0, 1, 0, world1.gRotationDegrees());
			GLfloat Matrix[16];
			Quat.CreateMatrix(Matrix);
			glMultMatrixf(Matrix);
			//scale down
			//if(showWorld){ glScalef(.5,.5,.5);	}  //TODO better scale
			if(showWorld){ 
				float sc = Shark.scaleRatio() * globalWorldScale;
				glScalef(sc, sc, sc);	
			}  
			else{ 
				glScalef(1.5,1.5,1.5); }
			//b1.draw(Vector3f(1,0,-1));
			Shark.drawShark(frame, quadratic);			//draw shark		
		}glPopMatrix();
		
    	}glPopMatrix();
    }glPopMatrix();

	glutSwapBuffers();
}



/*first argument is the file to the spline path */
int main(int argc, char** argv)
{
   if(argc <= 1 ) {
   	printf("Usage: runshark [spline filename]\n");
	return -1;
   }

   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(800, 600);   //can alter initial window dimensions here 
   glutInitWindowPosition(50, 0);
   main_window = glutCreateWindow("Shark Motion: Cal Poly");
   
   Initialize(string(argv[1]));  //initialize the animation system, passing down the spline file

   //register glut callback functions
   glutDisplayFunc(Draw); 
   glutTimerFunc(1000, TUpdate, 1) ;
   glutVisibilityFunc(visible);
   glutReshapeFunc(reshape);
   glutMotionFunc(mouseClickMove);
   glutMouseFunc(mouseClick);
   glutKeyboardFunc(keyboard);

    //shark init
    Shark.toggleMoving(true);

   //GLUI Code to make the interface
   /*GLUI *glui = GLUI_Master.create_glui( "Options" );
	
   GLUI_Checkbox *checkbox_wire = glui->add_checkbox( "Wireframe", &wireframe );	//creates checkbox to draw as wireframe
   GLUI_Spinner *segment_spinner = glui->add_spinner( "Segments:", GLUI_SPINNER_INT, &Shark.tempSegments );
   segment_spinner->set_int_limits( Shark.segMin, Shark.segMax );			//creates spinner for # of segments
   glui->add_separator();

   GLUI_Panel *seg_panel = glui->add_panel("Segment length");	//segment length panel
   GLUI_Spinner *num_segment_spinner = glui->add_spinner_to_panel(seg_panel,"Segment #:", GLUI_SPINNER_INT, &Shark.segNum);
   num_segment_spinner->set_int_limits(1, Shark.segMax);
   GLUI_Spinner *segment_percent_spinner = glui->add_spinner_to_panel(seg_panel, "Segment %:", GLUI_SPINNER_INT, &Shark.tempSegPercent);
   segment_percent_spinner->set_int_limits(0, 100-Shark.segMin); //every segment must be atleast 1% long

   glui->add_button( "Update", 0, (GLUI_Update_CB)gluiUpdate);
 */

   //glui->set_main_gfx_window( main_window );
   // We register the idle callback with GLUI, *not* with GLUT 
   //GLUI_Master.set_glutIdleFunc( myGlutIdle );  //causes spam warnings in console when not on GLUI window
	//*/

   prevTime = glutGet(GLUT_ELAPSED_TIME);
   glutMainLoop();  //begin glut functions
   return 0;
}

