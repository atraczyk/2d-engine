//-----------------------------------------------------------------------------
//  2d platform game engine and level editor
//
//  DESCRIPTION:    program/window handling
//-----------------------------------------------------------------------------

#include "program.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif

GLWindow::GLWindow ()
{
  window = nullptr;
  glContext = nullptr;
  borderWidth = 0;
  borderHeight = 0;
}

GLWindow::~GLWindow ()
{
  destroyGLWindow ();
}

void
GLWindow::initialize ()
{
  isActive = true;
  isFullscreen = false;
  windowedMode = 1;
  fullscreenMode = 0;
  scaleMode = 1;

  SDL_DisplayMode nativeMode;
  SDL_GetCurrentDisplayMode (0, &nativeMode);

  videoModes[0] = VideoMode (4 * nativeMode.w / 4, 4 * nativeMode.h / 4, 32);
  videoModes[1] = VideoMode (3 * nativeMode.w / 4, 3 * nativeMode.h / 4, 32);
  videoModes[2] = VideoMode (2 * nativeMode.w / 4, 2 * nativeMode.h / 4, 32);
  videoModes[3] = VideoMode (1 * nativeMode.w / 4, 1 * nativeMode.h / 4, 32);

  // Initialize virtual modes for compatibility
  virtualModes[0] = VideoMode (1920, 1080, 32);
  virtualModes[1] = VideoMode (1440, 810, 32);
  virtualModes[2] = VideoMode (960, 540, 32);
  virtualModes[3] = VideoMode (480, 270, 32);

  for (int i = 1; i < NUM_MODES; i++)
    {
      if (videoModes[i].height % 2 != 0)
        videoModes[i].height = videoModes[i].height - 1;
    }
}

void
GLWindow::computeScale ()
{
  scaleValue = 16.0f;
  scaleMode = 3;
  float w = (float) currentWidth ();
  float h = (float) currentHeight ();
  if ((w / h) < (16.0f / 9.0f))
    {
      if (w >= 1920)
        {
          scaleMode = 0;
          scaleValue = 64.0f;
        }
      else if (w >= 1440)
        {
          scaleMode = 1;
          scaleValue = 48.0f;
        }
      else if (w >= 960)
        {
          scaleMode = 2;
          scaleValue = 32.0f;
        }
    }
  else
    {
      if (h >= 1080)
        {
          scaleMode = 0;
          scaleValue = 64.0f;
        }
      else if (h >= 810)
        {
          scaleMode = 1;
          scaleValue = 48.0f;
        }
      else if (h >= 540)
        {
          scaleMode = 2;
          scaleValue = 32.0f;
        }
    }
}

void
GLWindow::destroyGLWindow ()
{
  destroyGL ();

  if (glContext)
    {
      SDL_GL_DeleteContext (glContext);
      glContext = nullptr;
    }

  if (window)
    {
      SDL_DestroyWindow (window);
      window = nullptr;
    }
}

void
GLWindow::destroyGL ()
{
  if (isFullscreen)
    {
      SDL_SetWindowFullscreen (window, 0);
      SDL_ShowCursor (SDL_ENABLE);
    }
}

bool
GLWindow::initGL (int bits)
{
#ifdef __APPLE__
  SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif
  SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute (SDL_GL_ALPHA_SIZE, 8);

  glContext = SDL_GL_CreateContext (window);
  if (!glContext)
    {
      std::cerr << "Failed to create OpenGL context: " << SDL_GetError ()
                << std::endl;
      return false;
    }

  if (SDL_GL_MakeCurrent (window, glContext) < 0)
    {
      std::cerr << "Failed to make OpenGL context current: " << SDL_GetError ()
                << std::endl;
      return false;
    }

  // Initialize GLEW with experimental flag
  glewExperimental = GL_TRUE;
  GLenum err = glewInit ();
  if (err != GLEW_OK)
    {
      std::cerr << "Failed to initialize GLEW: " << glewGetErrorString (err)
                << std::endl;
      return false;
    }

  return true;
}

bool
GLWindow::createGLWindow (const char *title)
{
  int width = isFullscreen ? videoModes[fullscreenMode].width
                           : videoModes[windowedMode].width;
  int height = isFullscreen ? videoModes[fullscreenMode].height
                            : videoModes[windowedMode].height;
  int bits = isFullscreen ? videoModes[fullscreenMode].bpp
                          : videoModes[windowedMode].bpp;

  SDL_DisplayMode currentMode;
  SDL_GetCurrentDisplayMode (0, &currentMode);
  windowPosition =
    Point (currentMode.w / 2 - width / 2, currentMode.h / 2 - height / 2);

  int x = isFullscreen ? 0 : windowPosition.x;
  int y = isFullscreen ? 0 : windowPosition.y;

  Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
  if (isFullscreen)
    {
      flags |= SDL_WINDOW_FULLSCREEN;
      SDL_ShowCursor (SDL_DISABLE);
    }

  window = SDL_CreateWindow (title, x, y, width, height, flags);
  if (!window)
    {
      std::cerr << "Failed to create window: " << SDL_GetError () << std::endl;
      return false;
    }

  if (!initGL (bits))
    {
      SDL_ShowSimpleMessageBox (SDL_MESSAGEBOX_ERROR, "Error",
                                "OpenGL initialization failed.", window);
      SDL_DestroyWindow (window);
      window = nullptr;
      return false;
    }

  SDL_RaiseWindow (window);
  return true;
}

void
GLWindow::createGLWindow (bool togglefullscreen)
{
  if (togglefullscreen)
    {
      isFullscreen = !isFullscreen;
      destroyGL ();
    }

  int width = isFullscreen ? videoModes[fullscreenMode].width
                           : videoModes[windowedMode].width;
  int height = isFullscreen ? videoModes[fullscreenMode].height
                            : videoModes[windowedMode].height;

  if (isFullscreen)
    {
      SDL_SetWindowFullscreen (window, SDL_WINDOW_FULLSCREEN);
      SDL_ShowCursor (SDL_DISABLE);
    }
  else
    {
      SDL_SetWindowFullscreen (window, 0);
      SDL_SetWindowSize (window, width, height);
      SDL_SetWindowPosition (window, windowPosition.x, windowPosition.y);
      SDL_ShowCursor (SDL_ENABLE);
    }
}

int
GLWindow::currentWidth ()
{
  int width;
  SDL_GetWindowSize (window, &width, nullptr);
  return width;
}

int
GLWindow::currentHeight ()
{
  int height;
  SDL_GetWindowSize (window, nullptr, &height);
  return height;
}

int
GLWindow::currentMode ()
{
  return isFullscreen ? fullscreenMode : windowedMode;
}
