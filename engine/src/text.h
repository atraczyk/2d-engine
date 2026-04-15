//-----------------------------------------------------------------------------
//  2d platform game engine and level editor
//    email: 
//
//  DESCRIPTION:    text/character drawing utilites for opengl
//-----------------------------------------------------------------------------

#ifndef TEXT_H
#define TEXT_H

// clang-format off
// OpenGL headers in correct order
#include <GL/glew.h>
#include <GL/gl.h>
// clang-format on

// Then include standard C/C++ headers
#include <string>

// Finally include our own headers
#include "camera.h"
#include "point.h"
#include "texture.h"
#include "utils.h"
#include "vector.h"

class Font {
public:
  ScreenRect charsXYWH;
  Camera *pcamera;
  ScreenRect *pscreen;
  TileAtlas *atlas;

  float scaleValue;

  Font();
  ~Font();

  void initialize(int charsWide, int charsHigh, int charWidth, int charHeight,
                  float Scale, Camera *pCamera, ScreenRect *pScreen,
                  TileAtlas *ptexatlas);
  void displayText(float x, float y, float w, float h, float r, float g,
                   float b, const char *string);
  void displayTextNoSpacing(float x, float y, float w, float h, float r,
                            float g, float b, const char *string);
  void displayScreenText(int x, int y, float r, float g, float b,
                         const char *string);
  void BitmapCharacter(float x, float y, float w, float h, int character);
};

void InitSpacings();

#endif
