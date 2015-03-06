//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	2d object roots 
//-----------------------------------------------------------------------------

#include "objects.h"

//StaticObject
StaticObject::StaticObject(Vector2 _position, Vector2 _size)
{
	position = _position;
	size = _size;
}

StaticObject::~StaticObject()
{
}

//MovingObject
MovingObject::MovingObject(Vector2 _position, Vector2 _size)
{
	position = _position;
	size = _size;
}

MovingObject::~MovingObject()
{
}