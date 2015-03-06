//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	vectors 2,3,4
//-----------------------------------------------------------------------------

#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

class Vector2
{
public:
	float x, y;

	Vector2(float _x = 0, float _y = 0);	
	~Vector2();

	Vector2& operator = (const Vector2 &rhs);
	Vector2& operator += (const Vector2 &rhs);
	Vector2& operator -= (const Vector2 &rhs);
	Vector2 operator * (float rhs) const; 
	Vector2 operator / (float rhs) const; 
	Vector2 operator + (const Vector2 &rhs) const;
	Vector2 operator - (const Vector2 &rhs) const;
	Vector2 Integer();
	void Rotate(float angle);
	float CrossProduct(const Vector2 &rhs) const;
	float Magnitude();
	void Normalize();
	float DotProduct(const Vector2 &rhs) const;

};

class Vector3
{
public:
	float x, y, z;

	Vector3(float _x = 0, float _y = 0, float _z = 0);	
	~Vector3();

	Vector3& operator = (const Vector3 &rhs);
	Vector3& operator += (const Vector3 &rhs);
	Vector3 operator * (float rhs) const; 
	Vector3 operator / (float rhs) const; 
	Vector3 operator + (const Vector3 &rhs) const;
	Vector3 operator - (const Vector3 &rhs) const;

};

class Vector4
{
public:
	float x, y, z, w;

	Vector4(float _x = 0, float _y = 0, float _z = 0, float _w = 0);	
	~Vector4();

	Vector4& operator = (const Vector4 &rhs);
	Vector4& operator += (const Vector4 &rhs);
	Vector4 operator * (float rhs) const; 
	Vector4 operator / (float rhs) const; 
	Vector4 operator + (const Vector4 &rhs) const;
	Vector4 operator - (const Vector4 &rhs) const;

};

#endif 