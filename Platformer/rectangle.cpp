//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	rectangle object top-down and top-up
//-----------------------------------------------------------------------------

#include "rectangle.h"

//ScreenRect 
ScreenRect::ScreenRect(float _x, float _y, float _w, float _h)
{
	x = _x; y = _y; w = _w; h = _h;
}

ScreenRect::~ScreenRect(void)
{
}

int ScreenRect::iTop()       {   return ((int)(y));         }
int ScreenRect::iLeft()      {   return ((int)(x));         }
int ScreenRect::iBottom()    {   return ((int)(y + h));     }
int ScreenRect::iRight()     {   return ((int)(x + w));     }

float ScreenRect::fTop()     {   return (y);                }
float ScreenRect::fLeft()    {   return (x);                }
float ScreenRect::fBottom()  {   return (y + h);            }
float ScreenRect::fRight()   {   return (x + w);            }

//WorldRect
WorldRect::WorldRect(float _x, float _y, float _w, float _h)
{
	x = _x; y = _y; w = _w; h = _h;
}

WorldRect::~WorldRect(void)
{
}

int WorldRect::iTop()       {   return (int)((y + h));      }
int WorldRect::iLeft()      {   return (int)((x));			}
int WorldRect::iBottom()    {   return (int)((y));			}
int WorldRect::iRight()     {   return (int)((x + w));		}

float WorldRect::fTop()     {	return (y + h);				}
float WorldRect::fLeft()    {	return (x);					}
float WorldRect::fBottom()  {	return (y);					}
float WorldRect::fRight()   {	return (x + w);				}

