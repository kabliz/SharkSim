#ifndef VECTOR_H
#define VECTOR_H
#pragma once


#include <cmath>
#include <iostream>


using namespace std;


class Vector2f {
   public:
      Vector2f();
      Vector2f( float in_x, float in_y );

      Vector2f operator = ( const Vector2f & v );
      Vector2f operator + ( const Vector2f & v );
      Vector2f operator - ( const Vector2f & v );
      Vector2f operator * ( const Vector2f & v );
      Vector2f operator * ( float c );
      Vector2f operator / ( const Vector2f & v );
      Vector2f operator / ( float c );
      void operator += ( const Vector2f & v );
      void operator -= ( const Vector2f & v );
      void operator *= ( const Vector2f & v );
      void operator *= ( float c );
      void operator /= ( const Vector2f & v );
      void operator /= ( float c );

      float Dot( const Vector2f & v );
      float Magnitude();
      Vector2f Normalize();

      void Print();
   public:
      float x, y;
};


class Vector3f {
   public:
      Vector3f();
      Vector3f( float in_x, float in_y, float in_z );

      bool operator == ( const Vector3f & v );
      bool operator != ( const Vector3f & v );
      bool operator > ( const Vector3f & v );
      bool operator < ( const Vector3f & v );
      bool operator >= ( const Vector3f & v );
      bool operator <= ( const Vector3f & v );

      Vector3f operator = ( const Vector3f & v );
      Vector3f operator + ( const Vector3f & v );
      Vector3f operator - ( const Vector3f & v );
      Vector3f operator * ( const Vector3f & v );
      Vector3f operator * ( float c );
      Vector3f operator / ( const Vector3f & v );
      Vector3f operator / ( float c );
      void operator += ( const Vector3f & v );
      void operator -= ( const Vector3f & v );
      void operator *= ( const Vector3f & v );
      void operator *= ( float c );
      void operator /= ( const Vector3f & v );
      void operator /= ( float c );

      Vector3f Cross( const Vector3f & v );
      float Dot( const Vector3f & v );
      float EuclDistance( const Vector3f & v);
      float Magnitude();
      Vector3f Normalize();
      Vector3f Interpolate(Vector3f other, float percentage);
      bool fEquals(Vector3f oth, float epsilon);

      void Print();
   public:
      float x, y, z;
};

class Vector4f {
   public:
      Vector4f();
      Vector4f( float in_x, float in_y, float in_z, float in_w );

      Vector4f operator = ( const Vector4f & v );
      Vector4f operator + ( const Vector4f & v );
      Vector4f operator - ( const Vector4f & v );
      Vector4f operator * ( const Vector4f & v );
      Vector4f operator * ( float c );
      Vector4f operator / ( const Vector4f & v );
      Vector4f operator / ( float c );
      void operator += ( const Vector4f & v );
      void operator -= ( const Vector4f & v );
      void operator *= ( const Vector4f & v );
      void operator *= ( float c );
      void operator /= ( const Vector4f & v );
      void operator /= ( float c );

      float Dot( const Vector4f & v );
      float Magnitude();
      Vector4f Normalize();

      void Print();
   public:
      float x, y, z, w;
};


#endif
