//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	vectors 2,3,4
//-----------------------------------------------------------------------------

#include "vector.h"

#pragma warning(disable:4244)

//Vector2
Vector2::Vector2(float _x, float _y)
{
    x = _x;
    y = _y;
}

Vector2::~Vector2(void)
{
}

Vector2& Vector2::operator += (const Vector2 &rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}

Vector2& Vector2::operator -= (const Vector2 &rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}

Vector2& Vector2::operator = (const Vector2 &rhs)
{
    this->x = rhs.x;
    this->y = rhs.y;
    return *this;
}

Vector2 Vector2::operator * (float rhs) const
{
    return Vector2(this->x * rhs, this->y * rhs);
}

Vector2 Vector2::operator / (float rhs) const
{
    return Vector2(this->x / rhs, this->y / rhs);
}

Vector2 Vector2::operator + (const Vector2 &rhs) const
{
    return Vector2(this->x + rhs.x, this->y + rhs.y);
}

Vector2 Vector2::operator - (const Vector2 &rhs) const
{
    return Vector2(this->x - rhs.x, this->y - rhs.y);
}

Vector2 Vector2::Integer()
{
    return Vector2((int)this->x, (int)this->y);
}

void Vector2::Rotate(float angle)
{
    float xt = (this->x * cosf(angle)) - (this->y * sinf(angle));
    float yt = (this->y * cosf(angle)) + (this->x * sinf(angle));
    this->x = xt;
    this->y = yt;
}

float Vector2::CrossProduct(const Vector2 &rhs) const
{
    return (this->x * rhs.y) - (this->y * rhs.x);
}

float Vector2::Magnitude()
{
    return sqrtf(this->x * this->x + this->y * this->y);
}

void Vector2::Normalize()
{
    float mag = sqrtf(this->x * this->x + this->y * this->y);
    this->x = this->x / mag;
    this->y = this->y / mag;
}

float Vector2::DotProduct(const Vector2 &rhs) const
{
    return (this->x * rhs.x) + (this->y * rhs.y);
}

//Vector3
Vector3::Vector3(float _x, float _y, float _z)
{
    x = _x;
    y = _y;
    z = _z;
}

Vector3::~Vector3(void)
{
}

Vector3& Vector3::operator += (const Vector3 &rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
}

Vector3& Vector3::operator = (const Vector3 &rhs)
{
    this->x = rhs.x;
    this->y = rhs.y;
    this->z = rhs.z;
    return *this;
}

Vector3 Vector3::operator * (float rhs) const
{
    return Vector3(this->x * rhs, this->y * rhs, this->z * rhs);
}

Vector3 Vector3::operator / (float rhs) const
{
    return Vector3(this->x / rhs, this->y / rhs, this->z / rhs);
}

Vector3 Vector3::operator + (const Vector3 &rhs) const
{
    return Vector3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
}

Vector3 Vector3::operator - (const Vector3 &rhs) const
{
    return Vector3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
}

//Vector4
Vector4::Vector4(float _x, float _y, float _z, float _w)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

Vector4::~Vector4(void)
{
}

Vector4& Vector4::operator += (const Vector4 &rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    this->w += rhs.w;
    return *this;
}

Vector4& Vector4::operator = (const Vector4 &rhs)
{
    this->x = rhs.x;
    this->y = rhs.y;
    this->z = rhs.z;
    this->w = rhs.w;
    return *this;
}

Vector4 Vector4::operator * (float rhs) const
{
    return Vector4(this->x * rhs, this->y * rhs, this->z * rhs, this->w * rhs);
}

Vector4 Vector4::operator / (float rhs) const
{
    return Vector4(this->x / rhs, this->y / rhs, this->z / rhs, this->w / rhs);
}

Vector4 Vector4::operator + (const Vector4 &rhs) const
{
    return Vector4(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w);
}

Vector4 Vector4::operator - (const Vector4 &rhs) const
{
    return Vector4(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z, this->w - rhs.w);
}