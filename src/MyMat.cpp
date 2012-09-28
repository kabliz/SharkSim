/*
 * Helper Matrix class with 4x4 matrix
 * provided by ZJ Wood for CSC 473
 * Heavily built on matrix class from Inventor SbMatrix class
 * available from Coin 3D (http://www.coin3d.org/)
 * modified 1/2010
 *       
 */

#include "MyMat.h"
#include <string.h>
#include <iostream>
#include <cfloat>
#include <math.h>

/* default constructor makes an identity matrix */
MyMat::MyMat(void) {
	int i,j;

	for (i=0;i<4;i++){
		for (j=0;j<4;j++){
			if (i==j)
				m_Elem[i][j]=1.0;
			else
				m_Elem[i][j]=0.0;
		}
	}
}

/* constructor to a specific matrix */
MyMat::MyMat(const float a11, const float a12,
		const float a13, const float a14,
		const float a21, const float a22,
		const float a23, const float a24,
		const float a31, const float a32,
		const float a33, const float a34,
		const float a41, const float a42,
		const float a43, const float a44) {
	m_Elem[0][0] = a11;
	m_Elem[0][1] = a12;
	m_Elem[0][2] = a13;
	m_Elem[0][3] = a14;
	m_Elem[1][0] = a21;
	m_Elem[1][1] = a22;
	m_Elem[1][2] = a23;
	m_Elem[1][3] = a24;
	m_Elem[2][0] = a31;
	m_Elem[2][1] = a32;
	m_Elem[2][2] = a33;
	m_Elem[2][3] = a34;
	m_Elem[3][0] = a41;
	m_Elem[3][1] = a42;
	m_Elem[3][2] = a43;
	m_Elem[3][3] = a44;
}

/*TODO... what was the major of GLfloats again? */
void MyMat::readGLfloat(GLfloat* oth)
{
	m_Elem[0][0] = oth[0];
	m_Elem[0][1] = oth[1];
	m_Elem[0][2] = oth[2];
	m_Elem[0][3] = oth[3];
	m_Elem[1][0] = oth[4];
	m_Elem[1][1] = oth[5];
	m_Elem[1][2] = oth[6];
	m_Elem[1][3] = oth[7];
	m_Elem[2][0] = oth[8];
	m_Elem[2][1] = oth[9];
	m_Elem[2][2] = oth[10];
	m_Elem[2][3] = oth[11];
	m_Elem[3][0] = oth[12];
	m_Elem[3][1] = oth[13];
	m_Elem[3][2] = oth[14];
	m_Elem[3][3] = oth[15];
}

GLfloat* MyMat::toGLfloat(GLfloat* mat)
{
	mat[0] = m_Elem[0][0];
	mat[1] = m_Elem[0][1];
	mat[2] = m_Elem[0][2];
	mat[3] = m_Elem[0][3];
	mat[4] = m_Elem[1][0];
	mat[5] = m_Elem[1][1];
	mat[6] = m_Elem[1][2];
	mat[7] = m_Elem[1][3];
	mat[8] = m_Elem[2][0];
	mat[9] = m_Elem[2][1];
	mat[10] = m_Elem[2][2];
	mat[11] = m_Elem[2][3];
	mat[12] = m_Elem[3][0];
	mat[13] = m_Elem[3][1];
	mat[14] = m_Elem[3][2];
	mat[15] = m_Elem[3][3];
	return mat;
}

void MyMat::makeScale(Vector3f input)
{
	m_Elem[0][0] = 1.0/input.x;
	m_Elem[0][1] = 0;
	m_Elem[0][2] = 0;
	m_Elem[0][3] = 0;
	m_Elem[1][0] = 0;
	m_Elem[1][1] = 1.0/input.y;
	m_Elem[1][2] = 0;
	m_Elem[1][3] = 0;
	m_Elem[2][0] = 0;
	m_Elem[2][1] = 0;
	m_Elem[2][2] = 1.0/input.z;
	m_Elem[2][3] = 0;
	m_Elem[3][0] = 0;
	m_Elem[3][1] = 0;
	m_Elem[3][2] = 0;
	m_Elem[3][3] = 1.0;
}

void MyMat::makeTranslate(Vector3f input)
{
	m_Elem[0][0] = 1.0;
	m_Elem[0][1] = 0;
	m_Elem[0][2] = 0;
	m_Elem[0][3] = input.x;//0;//input.x;
	m_Elem[1][0] = 0;
	m_Elem[1][1] = 1.0;
	m_Elem[1][2] = 0;
	m_Elem[1][3] = input.y;//0;//input.y;
	m_Elem[2][0] = 0;
	m_Elem[2][1] = 0;
	m_Elem[2][2] = 1.0;
	m_Elem[2][3] = input.z;0;//input.z;
	m_Elem[3][0] = 0;//-input.x; //0;
	m_Elem[3][1] = 0;//-input.y;//0;
	m_Elem[3][2] = 0;//-input.z; //0;
	m_Elem[3][3] = 1.0;
}

void MyMat::makeRotate(Vector3f input)
{
	MyMat x, y, z;
	Vector3f in;
	in.x = input.x * 3.14159265 / 180.0;
	in.y = input.y * 3.14159265 / 180.0;
	in.z = input.z * 3.14159265 / 180.0;
	if(in.x <= -.001 || in.x >= 0.001)
	{
		x.m_Elem[0][0] = 1.0;
		x.m_Elem[0][1] = 0;
		x.m_Elem[0][2] = 0;
		x.m_Elem[0][3] = 0;
		x.m_Elem[1][0] = 0;
		x.m_Elem[1][1] = cos(in.x);
		x.m_Elem[1][2] = -sin(in.x);
		x.m_Elem[1][3] = 0;
		x.m_Elem[2][0] = 0;
		x.m_Elem[2][1] = sin(in.x);
		x.m_Elem[2][2] = cos(in.x);
		x.m_Elem[2][3] = 0;
		x.m_Elem[3][0] = 0;
		x.m_Elem[3][1] = 0;
		x.m_Elem[3][2] = 0;
		x.m_Elem[3][3] = 1.0;
	}

	if(in.y <= -.001 || in.y >= 0.001)
	{
		y.m_Elem[0][0] = cos(in.y);
		y.m_Elem[0][1] = 0;
		y.m_Elem[0][2] = sin(in.y);
		y.m_Elem[0][3] = 0;
		y.m_Elem[1][0] = 0;
		y.m_Elem[1][1] = 1.0;
		y.m_Elem[1][2] = 0;
		y.m_Elem[1][3] = 0;
		y.m_Elem[2][0] = -sin(in.y);
		y.m_Elem[2][1] = 0;
		y.m_Elem[2][2] = cos(in.y);
		y.m_Elem[2][3] = 0;
		y.m_Elem[3][0] = 0;
		y.m_Elem[3][1] = 0;
		y.m_Elem[3][2] = 0;
		y.m_Elem[3][3] = 1.0;
	}
	
	if(in.z <= -.001 || in.z >= 0.001)
	{
		z.m_Elem[0][0] = cos(in.z);
		z.m_Elem[0][1] = -sin(in.z);
		z.m_Elem[0][2] = 0; 
		z.m_Elem[0][3] = 0;
		z.m_Elem[1][0] = sin(in.z);
		z.m_Elem[1][1] = cos(in.z);
		z.m_Elem[1][2] = 0;
		z.m_Elem[1][3] = 0;
		z.m_Elem[2][0] = 0;
		z.m_Elem[2][1] = 0;
		z.m_Elem[2][2] = 1.0;
		z.m_Elem[2][3] = 0;
		z.m_Elem[3][0] = 0;
		z.m_Elem[3][1] = 0;
		z.m_Elem[3][2] = 0;
		z.m_Elem[3][3] = 1.0;
	}
	//hope this is in the right order
	*this = (x.multRight(y)).multRight(z);
}

MyMat MyMat::transpose(void)
{
	MyMat z;
	z.m_Elem[0][0] = m_Elem[0][0];
	z.m_Elem[0][1] = m_Elem[1][0];
	z.m_Elem[0][2] = m_Elem[2][0]; 
	z.m_Elem[0][3] = m_Elem[3][0];
	z.m_Elem[1][0] = m_Elem[0][1];
	z.m_Elem[1][1] = m_Elem[1][1];
	z.m_Elem[1][2] = m_Elem[2][1];
	z.m_Elem[1][3] = m_Elem[3][1];
	z.m_Elem[2][0] = m_Elem[0][2];
	z.m_Elem[2][1] = m_Elem[1][2];
	z.m_Elem[2][2] = m_Elem[2][2];
	z.m_Elem[2][3] = m_Elem[3][2];
	z.m_Elem[3][0] = m_Elem[0][3];
	z.m_Elem[3][1] = m_Elem[1][3];
	z.m_Elem[3][2] = m_Elem[2][3];
	z.m_Elem[3][3] = m_Elem[3][3];
	return z;
}

/* make the matrix the identity  - e.g. reset*/
void MyMat::makeIdentity(void) {
	int i,j;

	for (i=0;i<4;i++){
		for (j=0;j<4;j++){
			if (i==j)
				m_Elem[i][j]=1.0;
			else
				m_Elem[i][j]=0.0;
		}
	}
}

Vector3f MyMat::multVec(Vector3f v, bool isPoint)
{
	float hg = (isPoint ? 1.0 : 0.0);
	Vector3f newvec;
	/*newvec.x = v.x*m_Elem[0][0] + v.y*m_Elem[1][0] + v.z*m_Elem[2][0] + hg*m_Elem[3][0];
	newvec.y = v.x*m_Elem[0][1] + v.y*m_Elem[1][1] + v.z*m_Elem[2][1] + hg*m_Elem[3][1];
	newvec.z = v.x*m_Elem[0][2] + v.y*m_Elem[1][2] + v.z*m_Elem[2][2] + hg*m_Elem[3][2];*/
	
	newvec.x = v.x*m_Elem[0][0] + v.y*m_Elem[0][1] + v.z*m_Elem[0][2] + hg*m_Elem[0][3];
	newvec.y = v.x*m_Elem[1][0] + v.y*m_Elem[1][1] + v.z*m_Elem[1][2] + hg*m_Elem[1][3];
	newvec.z = v.x*m_Elem[2][0] + v.y*m_Elem[2][1] + v.z*m_Elem[2][2] + hg*m_Elem[2][3];
	return newvec;
}

/*   similar to SbMatrix
 *   Return matrix components in the SbMat structure.
 */
const SbMat & MyMat::getValue(void) const
{
	return this->m_Elem;
}

/* set the values of the matrix to match input matrix m */
void MyMat::setValue(const MyMat &m) {
	const SbMat &temp_m = m.getValue();
	m_Elem[0][0] = temp_m[0][0];
	m_Elem[0][1] = temp_m[0][1];
	m_Elem[0][2] = temp_m[0][2];
	m_Elem[0][3] = temp_m[0][3];
	m_Elem[1][0] = temp_m[1][0];
	m_Elem[1][1] = temp_m[1][1];
	m_Elem[1][2] = temp_m[1][2];
	m_Elem[1][3] = temp_m[1][3];
	m_Elem[2][0] = temp_m[2][0];
	m_Elem[2][1] = temp_m[2][1];
	m_Elem[2][2] = temp_m[2][2];
	m_Elem[2][3] = temp_m[2][3];
	m_Elem[3][0] = temp_m[3][0];
	m_Elem[3][1] = temp_m[3][1];
	m_Elem[3][2] = temp_m[3][2];
	m_Elem[3][3] = temp_m[3][3];
}














/*   similar to SbMatrix
 *   Returns pointer to the 4 element array representing a matrix row.
 *   i should be within [0, 3].
 *   given the pointer to the row, [] can be used to access specific component
 */
float * MyMat::operator [](int i)
{
	return this->m_Elem[i];
}

/*   similar to SbMatrix
 *   Returns pointer to the 4 element array representing a matrix row.
 *   i should be within [0, 3].
 *   given the pointer to the row, [] can be used to access specific component
 */
const float * MyMat::operator [](int i) const
{
	return this->m_Elem[i];
}

/*   similar to SbMatrix
 *   Let this matrix be right-multiplied by \a m. Returns reference to
 *     self.
 *
 *       This is the most common multiplication / concatenation operation
 *         when using column-order matrices, as SbMatrix instances are, by
 *           definition.
 *             */
MyMat & MyMat::multRight(const MyMat & m)
{
	SbMat & tfm = this->m_Elem;
	const SbMat & mfm = m.m_Elem;

	SbMat tmp;
	(void)memcpy(tmp, tfm, 4*4*sizeof(float));

	for (int i=0; i < 4; i++) {
		for (int j=0; j < 4; j++) {
			tfm[i][j] =
				tmp[i][0] * mfm[0][j] +
				tmp[i][1] * mfm[1][j] +
				tmp[i][2] * mfm[2][j] +
				tmp[i][3] * mfm[3][j];
		}
	}

	return *this;
}

/*   similar to SbMatrix
 *   Let this matrix be left-multiplied by \a m. Returns reference to
 *     self.
 *
 *       (Be aware that it is more common to use the SbMatrix::multRight()
 *         operation, when doing concatenation of transformations, as SbMatrix
 *           instances are by definition in column-order, and uses
 *             post-multiplication for common geometry operations.)
 *               */
MyMat& MyMat::multLeft(const MyMat & m)
{
	SbMat & tfm = this->m_Elem;
	const SbMat & mfm = m.m_Elem;


	SbMat tmp;
	(void)memcpy(tmp, tfm, 4*4*sizeof(float));

	for (int i=0; i < 4; i++) {
		for (int j=0; j < 4; j++) {
			tfm[i][j] =
				tmp[0][j] * mfm[i][0] +
				tmp[1][j] * mfm[i][1] +
				tmp[2][j] * mfm[i][2] +
				tmp[3][j] * mfm[i][3];
		}
	}
	return *this;
}

/*   from SbMatrix
 *   Return a new matrix which is the inverse matrix of this.
 *
 *     The user is responsible for checking that this is a valid operation
 *       to execute, by first making sure that the result of SbMatrix::det4()
 *         is not equal to zero.
 *          */
MyMat MyMat::inverse(void) const
{

	MyMat result;

	//local pointers for speed
	float (*dst)[4];
	dst = reinterpret_cast<float (*)[4]>(result.m_Elem[0]);
	const float (*src)[4];
	src = reinterpret_cast<const float (*)[4]>(this->m_Elem[0]);

	//check for affine case - from Kevin, WU, 'Fast Matrix Inversion
	if (src[0][3] == 0.0f && src[1][3] == 0.0f &&
			src[2][3] == 0.0f && src[3][3] == 1.0f) {
		float det_1;
		float pos, neg, temp;
#define ACCUMULATE     \
		if (temp >= 0.0f)  \
		pos += temp;     \
		else               \
		neg += temp

		/*
		 * Calculate the determinant of submatrix A and determine if the
		 * the matrix is singular as limited by floating-point data
		 * representation.
		 */
		pos = neg = 0.0f;
		temp =  src[0][0] * src[1][1] * src[2][2];
		ACCUMULATE;
		temp =  src[0][1] * src[1][2] * src[2][0];
		ACCUMULATE;
		temp =  src[0][2] * src[1][0] * src[2][1];
		ACCUMULATE;
		temp = -src[0][2] * src[1][1] * src[2][0];
		ACCUMULATE;
		temp = -src[0][1] * src[1][0] * src[2][2];
		ACCUMULATE;
		temp = -src[0][0] * src[1][2] * src[2][1];
		ACCUMULATE;
		det_1 = pos + neg;

#undef ACCUMULATE

		/* Is the submatrix A singular? */
		if ((det_1 == 0.0f) || (fabs(det_1 / (pos - neg)) < FLT_EPSILON)) {
			/* Matrix M has no inverse */
			std::cout <<  "Matrix is singular." << std::endl;
			return *this;
		}
		else {
			det_1 = 1.0f / det_1;
			dst[0][0] = (src[1][1] * src[2][2] -
					src[1][2] * src[2][1]) * det_1;
			dst[1][0] = - (src[1][0] * src[2][2] -
					src[1][2] * src[2][0]) * det_1;
			dst[2][0] = (src[1][0] * src[2][1] -
					src[1][1] * src[2][0]) * det_1;
			dst[0][1] = - (src[0][1] * src[2][2] -
					src[0][2] * src[2][1]) * det_1;
			dst[1][1] = (src[0][0] * src[2][2] -
					src[0][2] * src[2][0]) * det_1;
			dst[2][1] = - (src[0][0] * src[2][1] -
					src[0][1] * src[2][0]) * det_1;
			dst[0][2] =  (src[0][1] * src[1][2] -
					src[0][2] * src[1][1]) * det_1;
			dst[1][2] = - (src[0][0] * src[1][2] -
					src[0][2] * src[1][0]) * det_1;
			dst[2][2] =  (src[0][0] * src[1][1] -
					src[0][1] * src[1][0]) * det_1;

			/* Calculate -C * inverse(A) */
			dst[3][0] = - (src[3][0] * dst[0][0] +
					src[3][1] * dst[1][0] +
					src[3][2] * dst[2][0]);
			dst[3][1] = - (src[3][0] * dst[0][1] +
					src[3][1] * dst[1][1] +
					src[3][2] * dst[2][1]);
			dst[3][2] = - (src[3][0] * dst[0][2] +
					src[3][1] * dst[1][2] +
					src[3][2] * dst[2][2]);
			/* Fill in last column */
			dst[0][3] = dst[1][3] = dst[2][3] = 0.0f;
			dst[3][3] = 1.0f;
		}
	} /*else non-affine*/
	else {
		//std::cout << "Non-affine!!!" << std::endl;
		float max, sum, tmp, inv_pivot;
		int p[4];
		int i, j, k;

		// algorithm from: Schwarz, "Numerische Mathematik"
		result = *this;

		for (k = 0; k < 4; k++) {
			max = 0.0f;
			p[k] = 0;

			for (i = k; i < 4; i++) {
				sum = 0.0f;
				for (j = k; j < 4; j++)
					sum += fabs(dst[i][j]);
				if (sum > 0.0f) {
					tmp = fabs(dst[i][k]) / sum;
					if (tmp > max) {
						max = tmp;
						p[k] = i;
					}
				}
			}

			if (max == 0.0f) {
				std::cout <<  "ERROR Matrix is singular." << std::endl;
				return *this;
			}

			if (p[k] != k) {
				for (j = 0; j < 4; j++) {
					tmp = dst[k][j];
					dst[k][j] = dst[p[k]][j];
					dst[p[k]][j] = tmp;
				}
			}

			inv_pivot = 1.0f / dst[k][k];
			for (j = 0; j < 4; j++) {
				if (j != k) {
					dst[k][j] = - dst[k][j] * inv_pivot;
					for (i = 0; i < 4; i++) {
						if (i != k) dst[i][j] += dst[i][k] * dst[k][j];
					}
				}
			}

			for (i = 0; i < 4; i++) dst[i][k] *= inv_pivot;
			dst[k][k] = inv_pivot;
		}

		for (k = 2; k >= 0; k--) {
			if (p[k] != k) {
				for (i = 0; i < 4; i++) {
					tmp = dst[i][k];
					dst[i][k] = dst[i][p[k]];
					dst[i][p[k]] = tmp;
				}
			}
		}
	}
	return result;
}

ostream& operator<<( ostream& os, const MyMat& m )
{
	int i,j;

	for (i=0;i<4;i++){
		for (j=0;j<4;j++){
			cout << m.m_Elem[i][j] << " ";
		}
		cout << endl;
	}
	return(os);
}

