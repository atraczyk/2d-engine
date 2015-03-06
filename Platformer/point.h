//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	2d integer point 
//-----------------------------------------------------------------------------

#ifndef POINT_H
#define POINT_H

class Point
{
public:
	int x, y;

	Point(int _x = 0, int _y = 0);	
	~Point();

	Point& operator = (const Point &rhs);
	Point& operator += (const Point &rhs);
	Point operator * (float rhs) const; 
	Point operator + (const Point &rhs) const;
	Point operator - (const Point &rhs) const;

};

#endif