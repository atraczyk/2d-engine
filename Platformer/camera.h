//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	camera class 
//-----------------------------------------------------------------------------

#ifndef CAMERA_H
#define CAMERA_H

#include <Windows.h>
#include <GL\gl.h>

#include "vector.h"
#include "objects.h"
#include "rectangle.h"
#include "point.h"

#include "shared.h"

class Camera : public MovingObject
{
public:
	Point			toMove;
	float			aspect;
	bool			isTargeting;
	MovingObject*	pTarget;
	float			scaleValue;

	Camera();
	~Camera();

	void reshape(float w, float h);
	void moveToTarget();
	void initialize(int x, int y, int width, int height);
	void setOrthoMatrix();
	void update(WorldRect* pconstraints);

};

#endif