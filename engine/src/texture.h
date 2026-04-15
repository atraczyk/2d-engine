//-----------------------------------------------------------------------------
//  2d platform game engine and level editor
//    email: 
//
//  DESCRIPTION:    load texture functions and atlas class
//-----------------------------------------------------------------------------

#ifndef TEXTURE_H
#define TEXTURE_H

// Include OpenGL headers
#include <GL/glew.h>

// Then include standard C/C++ headers
#include <stdio.h>
#include <vector>

// Finally include our own headers
#include "point.h"
#include "rectangle.h"
#include "vector.h"

class TileAtlas {
public:
  unsigned int *texture;
  int totalFrames;
  Point frames;
  Point imageSize;
  Point imageFrameSize;
  Vector2 texel;
  Vector2 textureFrameSize;
  ScreenRect *sourceRectangles;

  TileAtlas();
  ~TileAtlas();

  void initialize(unsigned int *ptex, int frames_wide, int frames_high,
                  int frame_width, int frame_height);
};

unsigned int loadBMPTexture_ARGB(const char *filename);
std::vector<unsigned char> loadTextureARGBfromfile(const char *filename, int &w,
                                                   int &h);

#endif
