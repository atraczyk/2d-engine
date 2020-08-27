//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	le camera class
//-----------------------------------------------------------------------------

#include "camera.h"
#include "utils.h"
#include <math.h>

#pragma warning(disable:4244)

#define ACCELERATION 1.0f
#define DECELERATION 1.0f
#define VELOCITY_MAX 10.0f

Camera::Camera()
{
	zoom = 0.0f;
	aspect = 0.0f;
	toMove.x = 0;
	toMove.y = 0;
}

Camera::~Camera()
{
}

void Camera::initialize(int x, int y, int width, int height)
{
	position.x	= x;
	position.y	= y;
	size.x		= width;
	size.y		= height;
	velocity      = Vector2(0.0f, 0.0f);
}

void Camera::setOrthoMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((int)(position.x), (int)(position.x + size.x), 
		(int)(position.y), (int)(position.y + size.y),
		-1.0f, 1.0f);
}

void Camera::reshape(float w, float h)
{
	float dw = w - size.x;
	float dh = h - size.y;
	float cx = position.x + size.x / 2;
	float cy = position.y + size.y / 2;
	size.x += dw;
	size.y += dh;
	position.x = cx - size.x / 2;
	position.y = cy - size.y / 2;
}

void Camera::Zoom(float value)
{
	zoom += value;
	float yvalue = value / aspect;
	position.x -= value;
	position.y -= yvalue;
	size.x += value * 2.0f;
	size.y += yvalue * 2.0f;
}

void Camera::ReshapeWithZoomValue()
{
	float yzoom = zoom / aspect;
	position.x -= zoom;
	position.y -= yzoom;
	size.x += zoom * 2.0f;
	size.y += yzoom * 2.0f;
}

void Camera::update(WorldRect constraints)
{

	if (toMove.x == 0) 
	{
		if (velocity.x < 0.0f) 
		{
			velocity.x += DECELERATION;
			if (velocity.x > 0.0f) 
				velocity.x = 0.0f;
		} 
		else if (velocity.x > 0.0f) 
		{
			velocity.x -= DECELERATION;
			if (velocity.x < 0.0f) 
				velocity.x = 0.0f;
		}
	} 
	else 
	{
		if (velocity.x * toMove.x < 0) 
			velocity.x += DECELERATION * toMove.x;
		else 
			velocity.x += ACCELERATION * toMove.x;
		if (velocity.x < -VELOCITY_MAX) 
			velocity.x = -VELOCITY_MAX;
		else if (velocity.x > VELOCITY_MAX) 
			velocity.x = VELOCITY_MAX;
	}

	if (toMove.y == 0) 
	{
		if (velocity.y < 0.0f) 
		{
			velocity.y += DECELERATION;
			if (velocity.y > 0.0f) 
				velocity.y = 0.0f;
		} 
		else if (velocity.y > 0.0f) 
		{
			velocity.y -= DECELERATION;
			if (velocity.y < 0.0f) 
				velocity.y = 0.0f;
		}
	} 
	else 
	{
		if (velocity.y * toMove.y < 0) 
			velocity.y += DECELERATION * toMove.y;
		else 
			velocity.y += ACCELERATION * toMove.y;
		if (velocity.y < -VELOCITY_MAX) 
			velocity.y = -VELOCITY_MAX;
		else if (velocity.y > VELOCITY_MAX) 
			velocity.y = VELOCITY_MAX;
	}

	lastPosition = position;
	position += velocity;

	if (position.x < constraints.fLeft())
		position.x = constraints.fLeft();
	if (position.x > (constraints.fRight() - size.x))
		position.x = constraints.fRight() - size.x;
	if (position.y > (constraints.fTop() - size.y))
		position.y = constraints.fTop() - size.y;
	if (position.y < constraints.fBottom())
		position.y = constraints.fBottom();
}
