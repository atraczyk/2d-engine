//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	opengl and window creation done in GLWindow class 
//-----------------------------------------------------------------------------

#ifndef PROGRAM_H
#define PROGRAM_H

#include <windows.h>
#include <GL\gl.h>	
#include <math.h>

#include "rectangle.h"
#include "point.h"

#define NUM_MODES 4

class VideoMode
{
public:
	int width;
	int height;
	int bpp;

	VideoMode(){};
	VideoMode(int w, int h, int bd)
	{width = w; height = h; bpp = bd;};	
	~VideoMode(){};
};

class GLWindow
{
public:

	HDC			hDC;
	HGLRC		hRC;
	HWND		hWnd;
	HINSTANCE	hInstance;
	DEVMODE		nativeMode;
	VideoMode	videoModes[NUM_MODES];
	VideoMode	virtualModes[NUM_MODES];
	int			borderWidth;
	int			borderHeight;
	Point		windowPosition;
	int			windowedMode; 
	int			fullscreenMode;
	int			scaleMode;
	bool		isActive;	
	bool		isFullscreen;
	float		scaleValue;

	GLWindow();	
	~GLWindow();

	void	initialize();
	void	destroyGLWindow();
	void	destroyGL();
	void	initGL(int bits);
	BOOL	createGLWindow(char* title, WNDPROC WndProc);
	void	createGLWindow(bool togglefullscreen);
	int		currentWidth();
	int		currentHeight();
	int		currentMode();
	void	computeScale();
};

#endif