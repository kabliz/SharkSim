/*
	Katherine Blizard
	Advisor: Zoe Wood

	Code used from: 
		+Jeff Molofee's Basecode Example @ nehe.gamedev.net
		+GLUI example: http://www.eng.cam.ac.uk/help/tpl/graphics/using_glui.html
		+Greg Ostrowski's senior project
*/
#include "Globals.h"
bool isLight;

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
	FILE * fp;
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
				fscanf(fp, "%f,",&Shark.segmentRot[Shark.sequencesParsed][i][Shark.segments-1-j]);
			}
			else
			{
				fscanf(fp, "%f,",&Shark.segmentRot[Shark.sequencesParsed][j][Shark.segments-1-i]);
			}
		}
	}
	fclose (fp);
	Shark.sequencesParsed++;
}

/*default shark animation sequence creator. Makes a shark that stays stiff */
void defSequence()
{
	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < Shark.segments; j++)
		{
			Shark.segmentRot[0][i][j] = 0;
		}
	}
	Shark.sequencesParsed++;

}

void Initialize ()					// Any GL Init Code & User Initialiazation Goes Here
{

 	srand(time(NULL));
	
	quadratic=gluNewQuadric();			// Create A Pointer To The Quadric Object
	gluQuadricNormals(quadratic, GLU_SMOOTH);	// Create Smooth Normals
	gluQuadricTexture(quadratic, GL_TRUE);		// Create Texture Coords

	initLighting();

	//read blender mesh
	mesh.init_blender();
	//initialize segLength
	for(int i = 0; i < Shark.segMax; i++)
	{
		Shark.segLengthInput[i] = 0;
	}
	Shark.segUpdate();
	//put in stiff (defualt case )sequence
	defSequence();	
	//read in recorded sequences, in the order they are enumerated in KeyframeSystem
	//readMovementData("straight10s2.csv");
	
	readMovementData("ani_slowstraight_4.csv");
	readMovementData("ani_faststraight_1.csv");
	readMovementData("ani_leftturn_2.csv");		
	readMovementData("ani_rightturn_2.csv");
	readMovementData("ani_right_uturn_1.csv");		
	//MATparser.parseFile("velandaccData.mat");
	//MATparser.parseFile("xytData.mat");
	//MATparser.parseFile("oxyMaps.mat");
	
	//world1.ereader.parseFile("Shovelnose.csv");
	//world1.gatherZPoints();
	//world1.mreader.parseFile("xytData.mat");
	//world1.gatherDTPoints();
	world1.initSpline();
	universalMesh = SharkMesh();
	
	Shark.skeleton = (&universalMesh);
	int tSegments = Shark.segments;
	Shark.skeleton.buildSkeleton(&mesh, tSegments, (float*) Shark.segLength);
	Shark.genKeyframes(dynaMode, &universalMesh);
	frameSpeed = frameSpeedSlow;
	Shark.kfSys.setFrameSpeed(10);
	


	//load textures
	TexLoad = Texture();
	TexLoad.LoadAll();

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
    if (w <= h)
      glOrtho(-2.5, 2.5, -2.5*(GLfloat)h/(GLfloat)w,
         2.5*(GLfloat)h/(GLfloat)w, -10.0, 1000.0);
    else
		//gluPerspective(60.0, (float)w/h, 0.5, 15.0);
      glOrtho(-2.5*(GLfloat)w/(GLfloat)h, 2.5*(GLfloat)w/(GLfloat)h, -2.5, 2.5, -10.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();					// Reset The Modelview Matrix

    ArcBall.setBounds((GLfloat)w, (GLfloat)h);          //*NEW* Update mouse bounds for arcball
}

void myGlutIdle( void )
{
  /* According to the GLUT specification, the current window is
     undefined during an idle callback.  So we need to explicitly change
     it if necessary */
  if ( glutGetWindow() != main_window )
    glutSetWindow(main_window);

  glutPostRedisplay();
}

void Deinitialize (void)// Any Application DeInitialization Goes Here
{
	gluDeleteQuadric(quadratic);
	Shark.deinitialize();	
	universalMesh.deleteHeap();
	world1.sPath.deleteHeap();
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
	case 's': case 'S':
		if(Shark.showSpine)
			Shark.showSpine = 0;
		else
			Shark.showSpine = 1;
		glutPostRedisplay();
		break;
	case 'k': case 'K':
		if(Shark.showSkin)
			Shark.showSkin = 0;
		else
			Shark.showSkin = 1;
		glutPostRedisplay();
		break;
	case 'm': case 'M':
		if(Shark.moving)
		{
			Shark.moving = 0;
			Shark.kfSys.prepareNextSeq(string("stiff"));
		}
		else
			Shark.moving = 1;
		break;
	case ',':
		frameSpeed = frameSpeedSlow;
		Shark.kfSys.decreaseFrames();
		break;
	case '.':
		frameSpeed = frameSpeedFast;
		Shark.kfSys.increaseFrames();
		break;
	case '[':
		frame++;
		if(frame >= Shark.segments)
		frame = 0;
		break;
	case ']':
		frame--;
		if(frame < 0)
		frame = Shark.segments-1;
		break;
	case 'p': case 'P':
		if(Shark.play)
			Shark.play = 0;
		else 
			Shark.play = 1;
		break;
	case 'l':case'L':
		toggleLighting();
		break;
	case 'n':case'N':  //cycle through animations
		if(Shark.kfSys.nextSequence+1 >= Shark.sequencesParsed)
			Shark.kfSys.nextSequence = 0;
		else
			Shark.kfSys.nextSequence++;
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

void gluiUpdate() {
	Shark.segUpdate();
}


/* A timed update system, that is called a few times per second. the paramater is required by glutTimerFunc. it doesnt matter what is in it
 * */
void TUpdate(int timed)
{
	//FutureFeature: rotate worlds around?
	Shark.timedUpdate(world1.deriveRailAngle());
	if(Shark.moving){					//increment movement frame
		//frame += frameSpeed;
		//Shark.kfSys.curFrame++;
		//Shark.kfSys.checkLoop();
		
		//if(frame >= Shark.segments) //?? TODO
		//	frame = 0;
		if(showWorld) 
		{ 
			world1.updateWorld(); 
			Shark.updateVelocity(world1.sPath.points[world1.curPoint], world1.sPath.points[world1.curPoint+1], 
						world1.sPath.dts[world1.curPoint]);
			Shark.kfSys.prepareNextSeq(world1.animationLoop);
		}
	}
		
	glutTimerFunc(16.6666, TUpdate, 1);
	glutPostRedisplay();
}

void Draw ()
{
	
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
			world1.displayWorld();                                  //ocean around the shark
		}
		glPushMatrix();
		{	
			//yaw rotation										
			glQuaternion Quat;
			Quat.CreateFromAxisAngle(0, 1, 0, world1.rotation.x*180/3.14159265);
			GLfloat Matrix[16];
			Quat.CreateMatrix(Matrix);
			glMultMatrixf(Matrix);
			//scale down
			if(showWorld){ glScalef(.5,.5,.5);	}
			Shark.drawShark(frame, quadratic);			//draw shark		
		}glPopMatrix();
		
    	}glPopMatrix();
    }glPopMatrix();

	glutSwapBuffers();
}



int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(600, 400);   //can alter initial window dimensions here 
   glutInitWindowPosition(50, 0);
   glutCreateWindow("Shark Motion: Cal Poly");
   Initialize();

   //register glut callback functions
   glutDisplayFunc(Draw); 
   glutTimerFunc(33.3333 , TUpdate, 1) ;
   glutReshapeFunc(reshape);
   glutMotionFunc(mouseClickMove);
   glutMouseFunc(mouseClick);
   glutKeyboardFunc(keyboard);

    //shark init
    Shark.moving = 1;

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


   glui->set_main_gfx_window( main_window );
   // We register the idle callback with GLUI, *not* with GLUT 
   //GLUI_Master.set_glutIdleFunc( myGlutIdle );  //causes spam warnings in console when not on GLUI window
	*/
	
   glutMainLoop();  //begin glut functions
   return 0;
}

