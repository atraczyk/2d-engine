#ifndef PROGRAM_H
#define PROGRAM_H

#include <windows.h>
#include <GL\gl.h>	
#include "rectangle.h"
#include "point.h"

#define NUM_MODES 3

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
	bool		isActive;	
	bool		isFullscreen;

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
};

wchar_t*	convertCharArrayToLPCWSTR(const char* charArray);
void		ShowTaskBar(bool show);

#endif