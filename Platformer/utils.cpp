//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	misc functions
//-----------------------------------------------------------------------------

#include "utils.h"

Point worldToScreen(Vector2 world_point, Camera camera, ScreenRect screen)
{
	int x = (int)(((world_point.x - camera.position.x) / camera.size.x) * (float)screen.w);
	int y = (int)(((world_point.y - camera.position.y) / camera.size.y) * -1 * ((float)screen.h) + (float)screen.h);
	return Point(x, y);
}

Vector2 screenToWorld(Point screen_point, Camera camera, ScreenRect screen)
{
	float x = (float)(((screen_point.x * camera.size.x) / screen.w) + camera.position.x);
	float y = (float)(((screen_point.y - screen.h) / screen.h) * -1 * +camera.size.y + camera.position.y);
	return Vector2(x, y);
}

Point worldToTile(Vector2 position, Vector2 size)
{
	return Point((int)(position.x / size.x), (int)(position.y / size.y));
}

bool isInRange(int x, int min, int max)
{
	return (x >= min && x <= max);
}

int roundUp(int number, int fixedBase) 
{
	return (number + fixedBase - 1) & ~(fixedBase - 1);
}

float roundUp( float number, float fixedBase ) 
{
	if (fixedBase != 0 && number != 0)
	{
		float sign = number>0.0f?1.0f:-1.0f;
		number*=sign;
		number/=fixedBase;
		int fixedPoint = (int)ceil(number);
		number = fixedPoint*fixedBase;
		number*=sign;
	}
	return number;
}

wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
    wchar_t* wString=new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}