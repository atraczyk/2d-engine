//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	text/character drawing utilites for opengl
//-----------------------------------------------------------------------------

#ifndef TEXT_H
#define TEXT_H

#include "texture.h"

#include <windows.h>
#include <GL/gl.h>
#include <string>

#include "vector.h"
#include "point.h"
#include "utils.h"
#include "camera.h"

class Font
{
public:
	ScreenRect charsXYWH;
	Camera* pcamera;
	ScreenRect* pscreen;
	TileAtlas* atlas;

	float  scaleValue;
			
	Font();
	~Font();

	void initialize(int charsWide, int charsHigh, int charWidth, int charHeight,
				 float Scale, Camera* pCamera, ScreenRect* pScreen, TileAtlas* ptexatlas);
	void displayText(float x, float y,  float w, float h, 
					float r, float g, float b, const char *string);
	void displayTextNoSpacing(float x, float y, float w, float h, 
					   float r, float g, float b, const char *string);
	void displayScreenText(int x, int y, float r, float g, float b, const char *string);
	void BitmapCharacter(float x, float y, float w, float h, int character);
};

void InitSpacings();

#endif