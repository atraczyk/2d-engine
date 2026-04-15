//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15)
// GPL 	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	misc functions
//-----------------------------------------------------------------------------

#ifndef UTILS_H
#define UTILS_H

#include "camera.h"
#include "point.h"
#include "rectangle.h"
#include "vector.h"

#include <algorithm>
#include <cstddef>
#include <stdio.h>
#include <string>
#include <vector>

Point worldToScreen(Vector2 world_point, Camera camera, ScreenRect screen);
Vector2 screenToWorld(Point screen_point, Camera camera, ScreenRect screen);
Point worldToTile(Vector2 position, Vector2 size);
bool isInRange(int x, int min, int max);
int roundUp(int number, int fixedBase);
float roundUp(float number, float fixedBase);
wchar_t *convertCharArrayToLPCWSTR(const char *charArray);
std::string resolveResourcePath(const std::string &relativePath);
std::string resolveResourcePath(const char *relativePath);

template <typename T> void remove(std::vector<T> &vec, size_t pos) {
  typename std::vector<T>::iterator it = vec.begin();
  std::advance(it, pos);
  vec.erase(it);
};

template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); };

// Simple safe min/max macros to use instead of Windows.h ones
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

// Helper macro for clamping values
#ifndef CLAMP
#define CLAMP(x, min_val, max_val) (MIN(MAX((x), (min_val)), (max_val)))
#endif

#endif
