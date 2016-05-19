//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15)
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	2d integer point
//-----------------------------------------------------------------------------

#include "point.h"
//Point
Point::Point(int _x, int _y)
{
    x = _x;
    y = _y;
}

Point::~Point(void)
{
}

Point& Point::operator += (const Point &rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}

Point& Point::operator = (const Point &rhs)
{
    this->x = rhs.x;
    this->y = rhs.y;
    return *this;
}

Point Point::operator*(float rhs) const
{
    return Point((int)(this->x * rhs), (int)(this->y * rhs));
}

Point Point::operator+(const Point &rhs) const
{
    return Point(this->x + rhs.x, this->y + rhs.y);
}

Point Point::operator-(const Point &rhs) const
{
    return Point(this->x - rhs.x, this->y - rhs.y);
}