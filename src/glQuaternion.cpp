// glQuaternion.cpp: implementation of the glQuaternion class.
//from: http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=Quaternion_Camera_Class
//////////////////////////////////////////////////////////////////////

#include "glQuaternion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

glQuaternion::glQuaternion()
{
	m_x = m_y = m_z = 0.0f;
	m_w = 1.0f;
}

glQuaternion::~glQuaternion()
{

}

float glQuaternion::Magnitude()
{
	float denom = sqrt((m_x*m_x) + (m_y*m_y) + (m_z*m_z) + (m_w*m_w));
	return denom;  
}

void glQuaternion::Normalize()
{
	float mag = Magnitude();
	m_w = m_w / mag;
	m_x = m_x / mag;
	m_y = m_y / mag;
	m_z = m_z / mag;
}

Vector3f glQuaternion::gVector()
{
	return Vector3f(m_x, m_y, m_z);
}
void glQuaternion::sVector(Vector3f n)
{
	m_x = n.x;
	m_y = n.y;
	m_z = n.z;

}
glQuaternion glQuaternion::multiply(glQuaternion other)
{
	glQuaternion result;
	Vector3f c1 = Vector3f(m_x, m_y, m_z);
	Vector3f c2 = Vector3f(other.gX(), other.gY(), other.gZ());
	result.sW(m_w*other.gW() - c1.Dot(c2));
        Vector3f mod = (c2*m_w) + (c1*other.gW()) + c1.Cross(c2);
	result.sVector(mod);
	return result;
}

void glQuaternion::CreateFromAxisAngle(GLfloat x, GLfloat y, GLfloat z, GLfloat degrees)
{
	// First we want to convert the degrees to radians 
	// since the angle is assumed to be in radians
	float angle = GLfloat((degrees / 180.0f) * PI);

	// Here we calculate the sin( theta / 2) once for optimization
	float result = (GLfloat)sin( angle / 2.0f );
		
	// Calcualte the w value by cos( theta / 2 )
	m_w = (float)cos( angle / 2.0f );

	// Calculate the x, y and z of the quaternion
	m_x = float(x * result);
	m_y = float(y * result);
	m_z = float(z * result);
}


/*fills a previously allocated pMatrix with a new rotation matrix based on this quaternion. 
 * Quaternion's angle and axis should be set already. */
void glQuaternion::CreateMatrix(GLfloat *pMatrix)
{
	// Make sure the matrix has allocated memory to store the rotation data
	if(!pMatrix) return;

	// First row
	pMatrix[ 0] = 1.0f - 2.0f * ( m_y * m_y + m_z * m_z ); 
	pMatrix[ 1] = 2.0f * (m_x * m_y + m_z * m_w);
	pMatrix[ 2] = 2.0f * (m_x * m_z - m_y * m_w);
	pMatrix[ 3] = 0.0f;  

	// Second row
	pMatrix[ 4] = 2.0f * ( m_x * m_y - m_z * m_w );  
	pMatrix[ 5] = 1.0f - 2.0f * ( m_x * m_x + m_z * m_z ); 
	pMatrix[ 6] = 2.0f * (m_z * m_y + m_x * m_w );  
	pMatrix[ 7] = 0.0f;  

	// Third row
	pMatrix[ 8] = 2.0f * ( m_x * m_z + m_y * m_w );
	pMatrix[ 9] = 2.0f * ( m_y * m_z - m_x * m_w );
	pMatrix[10] = 1.0f - 2.0f * ( m_x * m_x + m_y * m_y );  
	pMatrix[11] = 0.0f;  

	// Fourth row
	pMatrix[12] = 0;  
	pMatrix[13] = 0;  
	pMatrix[14] = 0;  
	pMatrix[15] = 1.0f;

	// Now pMatrix[] is a 4x4 homogeneous matrix that can be applied to an OpenGL Matrix
}


glQuaternion glQuaternion::operator *(glQuaternion q)
{
	glQuaternion r;

	r.m_w = m_w*q.m_w - m_x*q.m_x - m_y*q.m_y - m_z*q.m_z;
	r.m_x = m_w*q.m_x + m_x*q.m_w + m_y*q.m_z - m_z*q.m_y;
	r.m_y = m_w*q.m_y + m_y*q.m_w + m_z*q.m_x - m_x*q.m_z;
	r.m_z = m_w*q.m_z + m_z*q.m_w + m_x*q.m_y - m_y*q.m_x;
	
	return(r);
}
