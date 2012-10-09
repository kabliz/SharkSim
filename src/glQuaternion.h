// glQuaternion.h: interface for the glQuaternion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLQUATERNION_H__52FB84BB_6D1B_4314_833F_5A8737A35DAB__INCLUDED_)
#define AFX_GLQUATERNION_H__52FB84BB_6D1B_4314_833F_5A8737A35DAB__INCLUDED_

//#include <windows.h>		// Header File For Windows
//#include <gl\gl.h>			// Header File For The OpenGL32 Library
//#include <gl\glu.h>			// Header File For The GLu32 Library
//#include <gl\glut.h>
#include<GL/glut.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include <math.h>
#define PI			3.14159265358979323846

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class glQuaternion  
{
public:
	glQuaternion operator *(glQuaternion q);
	void CreateMatrix(float *pMatrix);
	void CreateFromAxisAngle(float x, float y, float z, float degrees);
	void setRadians(GLfloat angle);
	void setDegrees(GLfloat angle);
	void setAxis(GLfloat x, GLfloat y, GLfloat z);	
	glQuaternion();
	virtual ~glQuaternion();

private:
	float m_w;
	float m_z;
	float m_y;
	float m_x;
};

#endif // !defined(AFX_GLQUATERNION_H__52FB84BB_6D1B_4314_833F_5A8737A35DAB__INCLUDED_)
