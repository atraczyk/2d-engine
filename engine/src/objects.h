//-----------------------------------------------------------------------------
//  2d platform game engine and level editor by Andreas Traczyk (2014-15)
//  http://andreastraczyk.com/  email: andreastraczyk@gmail.com
//
//  DESCRIPTION:    2d object roots
//-----------------------------------------------------------------------------

#ifndef OBJECTS_H
#define OBJECTS_H

#include "vector.h"

class StaticObject
{
public:
    Vector2 position;
    Vector2 size;

    StaticObject(Vector2 _position = Vector2(0, 0),
        Vector2 _size = Vector2(0, 0));
    ~StaticObject();
};

class MovingObject : public StaticObject
{
public:
    Vector2 lastPosition;
    Vector2 lastVelocity;
    Vector2 velocity;

    MovingObject(Vector2 _position = Vector2(0, 0),
        Vector2 _size = Vector2(0, 0));
    ~MovingObject();
};

#endif