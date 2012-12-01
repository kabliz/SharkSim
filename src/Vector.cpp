#include "Vector.h"


Vector2f::Vector2f() {
	x = y = 0.0;
}

Vector2f::Vector2f( float in_x, float in_y ) {
	x = in_x;
	y = in_y;
}

Vector2f Vector2f::operator = ( const Vector2f & v ) {
	x = v.x;
	y = v.y;
	return (* this);
}

Vector2f Vector2f::operator + ( const Vector2f & v ) {
	Vector2f t;
	t.x = x + v.x;
	t.y = y + v.y;
	return t;
}

Vector2f Vector2f::operator - ( const Vector2f & v ) {
	Vector2f t;
	t.x = x - v.x;
	t.y = y - v.y;
	return t;
}
Vector2f Vector2f::operator * ( const Vector2f & v ) {
	Vector2f t;
	t.x = x * v.x;
	t.y = y * v.y;
	return t;
}

Vector2f Vector2f::operator * ( float c ) {
	Vector2f t;
	t.x = x * c;
	t.y = y * c;
	return t;
}

Vector2f Vector2f::operator / ( const Vector2f & v ) {
	Vector2f t;
	t.x = x / v.x;
	t.y = y / v.y;
	return t;
}

Vector2f Vector2f::operator / ( float c ) {
	Vector2f t;
	t.x = x / c;
	t.y = y / c;
	return t;
}

void Vector2f::operator += ( const Vector2f & v ) {
	x += v.x;
	y += v.y;
}

void Vector2f::operator -= ( const Vector2f & v ) {
	x -= v.x;
	y -= v.y;
}

void Vector2f::operator *= ( const Vector2f & v ) {
	x *= v.x;
	y *= v.y;
}

void Vector2f::operator *= ( float c ) {
	x *= c;
	y *= c;
}

void Vector2f::operator /= ( const Vector2f & v ) {
	x /= v.x;
	y /= v.y;
}

void Vector2f::operator /= ( float c ) {
	x /= c;
	y /= c;
}

float Vector2f::Dot( const Vector2f & v ) {
	return (x*v.x + y*v.y);
}

float Vector2f::Magnitude() {
	return sqrt( x*x + y*y );
}

Vector2f Vector2f::Normalize() {
	Vector2f t;
	float n = this->Magnitude();
	t = (* this);
	t = t / n;
	return t;
}

void Vector2f::Print() {
	cout << "< " << x << ", " << y << " >" << endl;
}


Vector3f::Vector3f() {
	x = y = z = 0.0;
}

Vector3f::Vector3f( float in_x, float in_y, float in_z ) {
	x = in_x;
	y = in_y;
	z = in_z;
}

bool Vector3f::operator == ( const Vector3f & v ) {
	if ( (x != v.x) ||
			(y != v.y) ||
			(z != v.z) ) {
		return false;
	}
	return true;
}

bool Vector3f::operator != ( const Vector3f & v ) {
	if ( (x != v.x) ||
			(y != v.y) ||
			(z != v.z) ) {
		return true;
	}
	return false;
}

bool Vector3f::operator > ( const Vector3f & v ) {
	if ( (x > v.x) &&
			(y > v.y) &&
			(z > v.z) ) {
		return true;
	}
	return false;
}

bool Vector3f::operator < ( const Vector3f & v ) {
	if ( (x < v.x) &&
			(y < v.y) &&
			(z < v.z) ) {
		return true;
	}
	return false;
}
bool Vector3f::operator >= ( const Vector3f & v ) {
	if ( (x >= v.x) &&
			(y >= v.y) &&
			(z >= v.z) ) {
		return true;
	}
	return false;
}
bool Vector3f::operator <= ( const Vector3f & v ) {
	if ( (x <= v.x) &&
			(y <= v.y) &&
			(z <= v.z) ) {
		return true;
	}
	return false;
}

Vector3f Vector3f::operator = ( const Vector3f & v ) {
	x = v.x;
	y = v.y;
	z = v.z;
	return (* this);
}

Vector3f Vector3f::operator + ( const Vector3f & v ) {
	Vector3f t;
	t.x = x + v.x;
	t.y = y + v.y;
	t.z = z + v.z;
	return t;
}

Vector3f Vector3f::operator - ( const Vector3f & v ) {
	Vector3f t;
	t.x = x - v.x;
	t.y = y - v.y;
	t.z = z - v.z;
	return t;
}
Vector3f Vector3f::operator * ( const Vector3f & v ) {
	Vector3f t;
	t.x = x * v.x;
	t.y = y * v.y;
	t.z = z * v.z;
	return t;
}

Vector3f Vector3f::operator * ( float c ) {
	Vector3f t;
	t.x = x * c;
	t.y = y * c;
	t.z = z * c;
	return t;
}

Vector3f Vector3f::operator / ( const Vector3f & v ) {
	Vector3f t;
	t.x = x / v.x;
	t.y = y / v.y;
	t.z = z / v.z;
	return t;
}

Vector3f Vector3f::operator / ( float c ) {
	Vector3f t;
	t.x = x / c;
	t.y = y / c;
	t.z = z / c;
	return t;
}

void Vector3f::operator += ( const Vector3f & v ) {
	x += v.x;
	y += v.y;
	z += v.z;
}

void Vector3f::operator -= ( const Vector3f & v ) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

void Vector3f::operator *= ( const Vector3f & v ) {
	x *= v.x;
	y *= v.y;
	z *= v.z;
}

void Vector3f::operator *= ( float c ) {
	x *= c;
	y *= c;
	z *= c;
}

void Vector3f::operator /= ( const Vector3f & v ) {
	x /= v.x;
	y /= v.y;
	z /= v.z;
}

void Vector3f::operator /= ( float c ) {
	x /= c;
	y /= c;
	z /= c;
}

Vector3f Vector3f::Cross( const Vector3f & v ) {
	Vector3f t;
	t.x = y*v.z - z*v.y;
	t.y = z*v.x - x*v.z;
	t.z = x*v.y - y*v.x;
	return t;
}

float Vector3f::Dot( const Vector3f & v ) {
	return (x*v.x + y*v.y + z*v.z);
}

float Vector3f::EuclDistance( const Vector3f & v) {
	return sqrt(((x-v.x)*(x-v.x)) + ((y-v.y)*(y-v.y)) + ((z-v.z)*(z-v.z)) );
}

float Vector3f::Magnitude() {
	return sqrt( x*x + y*y + z*z );
}

Vector3f Vector3f::Normalize() {
	Vector3f t;
	float n = this->Magnitude();
	if ( n == 0.0f ) {
		n = 1.0f;  
	}
	t = (* this);
	t = t / n;
	return t;
}

Vector3f Vector3f::Interpolate(Vector3f other, float percentage)
{
	Vector3f self = *this;
	Vector3f fin = self;
	fin = fin + (other-self)*(percentage);
	return fin;
}

Vector3f Vector3f::EaseIn(Vector3f oth, float dProp, float strength)
{
	return Interpolate(oth, pow(dProp,strength));
}

Vector3f Vector3f::EaseOut(Vector3f oth, float dProp, float strength)
{
	if(strength == 0)
	{
		return Vector3f(0,0,0);
	}
	return Interpolate(oth, pow(dProp, 1.0/strength));
}

bool Vector3f::fEquals(Vector3f oth, float epsilon) {
	return (x <= oth.x + epsilon && x >= oth.x - epsilon) && 
		(y <= oth.y + epsilon && y >= oth.y - epsilon) && 
		(z <= oth.z + epsilon && z >= oth.z - epsilon);  
}

float Vector3f::AngleCos(Vector3f pre, Vector3f post)  //returns in radians
{
	float r = (pre-*this).Dot(post-*this);
	return acos(r/((pre-*this).Magnitude()*(post-*this).Magnitude()));
}
float Vector3f::AngleTan(Vector3f pre, Vector3f post) //ignores y for now
{
	Vector3f firstBranch = (pre - *this).Normalize();
	Vector3f secondBranch = (post - *this).Normalize();

	//arcos dot product/productof magnitudes = angle in radians
	float Angle;
	Angle = atan2(secondBranch.x, secondBranch.z) - atan2(firstBranch.x, firstBranch.z);

	//accidentally measures 2pi sometimes when very small angles are being measured.
	//if(Angle > 5.5) { Angle -= 6.283185; }
	//if(Angle < -5.5) { Angle += 6.283185; }
	return Angle;
}

void Vector3f::Print() {
	cout << "< " << x << ", " << y << ", " << z << " >" << endl;
}


Vector4f::Vector4f() {
	x = y = z = w = 0.0;
}

Vector4f::Vector4f( float in_x, float in_y, float in_z, float in_w ) {
	x = in_x;
	y = in_y;
	z = in_z;
	w = in_w;
}

Vector4f Vector4f::operator = ( const Vector4f & v ) {
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return (* this);
}

Vector4f Vector4f::operator + ( const Vector4f & v ) {
	Vector4f t;
	t.x = x + v.x;
	t.y = y + v.y;
	t.z = z + v.z;
	t.w = w + v.w;
	return t;
}

Vector4f Vector4f::operator - ( const Vector4f & v ) {
	Vector4f t;
	t.x = x - v.x;
	t.y = y - v.y;
	t.z = z - v.z;
	t.w = w - v.w;
	return t;
}
Vector4f Vector4f::operator * ( const Vector4f & v ) {
	Vector4f t;
	t.x = x * v.x;
	t.y = y * v.y;
	t.z = z * v.z;
	t.w = w * v.w;
	return t;
}

Vector4f Vector4f::operator * ( float c ) {
	Vector4f t;
	t.x = x * c;
	t.y = y * c;
	t.z = z * c;
	t.w = w * c;
	return t;
}

Vector4f Vector4f::operator / ( const Vector4f & v ) {
	Vector4f t;
	t.x = x / v.x;
	t.y = y / v.y;
	t.z = z / v.z;
	t.w = w / v.w;
	return t;
}

Vector4f Vector4f::operator / ( float c ) {
	Vector4f t;
	t.x = x / c;
	t.y = y / c;
	t.z = z / c;
	t.w = w / c;
	return t;
}

void Vector4f::operator += ( const Vector4f & v ) {
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}

void Vector4f::operator -= ( const Vector4f & v ) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}

void Vector4f::operator *= ( const Vector4f & v ) {
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
}

void Vector4f::operator *= ( float c ) {
	x *= c;
	y *= c;
	z *= c;
	w *= c;
}

void Vector4f::operator /= ( const Vector4f & v ) {
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;
}

void Vector4f::operator /= ( float c ) {
	x /= c;
	y /= c;
	z /= c;
	w += c;
}

float Vector4f::Dot( const Vector4f & v ) {
	return (x*v.x + y*v.y + z*v.z + w*v.w);
}

float Vector4f::Magnitude() {
	return sqrt( x*x + y*y + z*z + w*w );
}

Vector4f Vector4f::Normalize() {
	Vector4f t;
	float n = this->Magnitude();
	t = (* this);
	t = t / n;
	return t;
}



void Vector4f::Print() {
	cout << "< " << x << ", " << y << ", " << z << ", " << w << " >" << endl;
}
