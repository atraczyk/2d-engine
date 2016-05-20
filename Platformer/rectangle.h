//-----------------------------------------------------------------------------
//  2d platform game engine and level editor by Andreas Traczyk (2014-15)
//  http://andreastraczyk.com/  email: andreastraczyk@gmail.com
//
//  DESCRIPTION:    rectangle object top-down and top-up
//-----------------------------------------------------------------------------

#ifndef RECTANGLE_H
#define RECTANGLE_H

class ScreenRect
{
public:
    float x, y, w, h;

    ScreenRect(float _x = 0, float _y = 0, float _w = 0, float _h = 0);
    ~ScreenRect(void);

    int iTop();
    int iLeft();
    int iBottom();
    int iRight();

    float fTop();
    float fLeft();
    float fBottom();
    float fRight();
};

class WorldRect
{
public:
    float x, y, w, h;

    WorldRect(float _x = 0, float _y = 0, float _w = 0, float _h = 0);
    ~WorldRect(void);

    int iTop();
    int iLeft();
    int iBottom();
    int iRight();

    float fTop();
    float fLeft();
    float fBottom();
    float fRight();
};

#endif
