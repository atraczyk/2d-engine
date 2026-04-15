/*
Copyright 2015 Andreas Traczyk

This file is part of Platformer.

Platformer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Platformer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Platformer.  If not, see <http://www.gnu.org/licenses/>.
*/

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
