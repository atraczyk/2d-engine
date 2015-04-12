//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	load texture functions and atlas class
//-----------------------------------------------------------------------------

#ifndef TEXTURE_H
#define TEXTURE_H

#include <Windows.h>
#include <GL/glew.h>

#include <stdio.h>
#include "point.h"
#include "vector.h"
#include "rectangle.h"

class TileAtlas
{
public:
	unsigned int* texture;
	int totalFrames;
	Point frames;
	Point imageSize;
	Point imageFrameSize;
	Vector2 texel;
	Vector2 textureFrameSize;
	ScreenRect* sourceRectangles;

	TileAtlas();
	~TileAtlas();

	void initialize(unsigned int* ptex, int frames_wide, int frames_high, int frame_width, int frame_height);
};

unsigned int	loadBMPTexture_ARGB( const char * filename);
unsigned char*	loadTextureARGBfromfile(const char * filename, int& w, int& h);

#endif