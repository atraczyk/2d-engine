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