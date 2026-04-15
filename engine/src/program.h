//-----------------------------------------------------------------------------
//  2d platform game engine and level editor
//
//  DESCRIPTION:    program/window handling
//-----------------------------------------------------------------------------

#ifndef PROGRAM_H
#define PROGRAM_H

// Include SDL2 headers
#include <SDL2/SDL.h>

// Standard library headers
#include <math.h>
#include <stdio.h>

// Project headers
#include "point.h"
#include "rectangle.h"

#define NUM_MODES 4

class VideoMode
{
public:
  int width;
  int height;
  int bpp;

  VideoMode (){};
  VideoMode (int w, int h, int bd)
  {
    width = w;
    height = h;
    bpp = bd;
  };
  ~VideoMode (){};
};

class GLWindow
{
public:
  SDL_Window *window;
  SDL_GLContext glContext;
  VideoMode videoModes[NUM_MODES];
  VideoMode virtualModes[NUM_MODES];
  int borderWidth;
  int borderHeight;
  Point windowPosition;
  int windowedMode;
  int fullscreenMode;
  int scaleMode;
  bool isActive;
  bool isFullscreen;
  float scaleValue;

  GLWindow ();
  ~GLWindow ();

  void
  initialize ();
  void
  destroyGLWindow ();
  void
  destroyGL ();
  bool
  initGL (int bits);
  bool
  createGLWindow (const char *title);
  void
  createGLWindow (bool togglefullscreen);
  int
  currentWidth ();
  int
  currentHeight ();
  int
  currentMode ();
  void
  computeScale ();
};

#endif
