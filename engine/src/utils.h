//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	misc functions
//-----------------------------------------------------------------------------

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <vector>
#include "point.h"
#include "vector.h"
#include "camera.h"
#include "rectangle.h"

Point worldToScreen(Vector2 world_point, Camera camera, ScreenRect screen);
Vector2 screenToWorld(Point screen_point, Camera camera, ScreenRect screen);
Point worldToTile(Vector2 position, Vector2 size);
bool isInRange(int x, int min, int max);
int roundUp(int number, int fixedBase);
float roundUp(float number, float fixedBase);
wchar_t *convertCharArrayToLPCWSTR(const char* charArray);

template <typename T>
void remove(std::vector<T>& vec, size_t pos)
{
    std::vector<T>::iterator it = vec.begin();
    std::advance(it, pos);
    vec.erase(it);
};

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
};

#endif
