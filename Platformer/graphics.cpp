//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	opengl and window creation done in GLWindow class 
//-----------------------------------------------------------------------------

#include "graphics.h"
#include <iostream>

#pragma warning(disable:4244)

GLWindow::GLWindow()
{
	hDC		= NULL;
	hRC		= NULL;
	hWnd	= NULL;
}

GLWindow::~GLWindow()
{
	destroyGLWindow();
}

void GLWindow::initialize()
{
	isActive			= true;
	isFullscreen		= false;
	windowedMode		= 1; 
	fullscreenMode		= 0; 
	scaleMode			= 1;

	EnumDisplaySettings (NULL, ENUM_CURRENT_SETTINGS, &nativeMode);

	videoModes[0] = VideoMode(4*nativeMode.dmPelsWidth/4, 4*nativeMode.dmPelsHeight/4, 32);
	videoModes[1] = VideoMode(3*nativeMode.dmPelsWidth/4, 3*nativeMode.dmPelsHeight/4, 32);
	videoModes[2] = VideoMode(2*nativeMode.dmPelsWidth/4, 2*nativeMode.dmPelsHeight/4, 32);
	videoModes[3] = VideoMode(1*nativeMode.dmPelsWidth/4, 1*nativeMode.dmPelsHeight/4, 32);

	virtualModes[0] = VideoMode(1920, 1080, 32);
	virtualModes[1] = VideoMode(1440, 810, 32);
	virtualModes[2] = VideoMode(960, 540, 32);
	virtualModes[3] = VideoMode(480, 270, 32);

	for(int i=1; i<NUM_MODES;i++)
	{
		if(videoModes[i].height % 2 != 0)
			videoModes[i].height = videoModes[i].height-1;
	}
}

void GLWindow::computeScale()
{
	scaleValue = 16.0f;
	scaleMode = 3;
	float w = (float)currentWidth();
	float h = (float)currentHeight();
	if ( (w/h) <  (16.0f/9.0f))
	{
		if(w>=1920)
		{
			scaleMode = 0;
			scaleValue = 64.0f;
		}
		else if(w>=1440)
		{
			scaleMode = 1;
			scaleValue = 48.0f;
		}
		else if(w>=960)
		{
			scaleMode = 2;
			scaleValue = 32.0f;
		}
	}
	else
	{
		if(h>=1080)
		{
			scaleMode = 0;
			scaleValue = 64.0f;
		}
		else if(h>=810)
		{
			scaleMode = 1;
			scaleValue = 48.0f;
		}
		else if(h>=540)
		{
			scaleMode = 2;
			scaleValue = 32.0f;
		}
	}
}

void GLWindow::destroyGLWindow()
{
	destroyGL();

	if ( hDC)
	{
		ReleaseDC( hWnd, hDC);
		hDC = NULL;
	}

	if ( hWnd)
	{
		DestroyWindow( hWnd);
		hWnd = NULL;
	}

	UnregisterClass(__TEXT("OGL"),hInstance);
	hInstance = NULL;	
}

void GLWindow::destroyGL()
{
	if ( isFullscreen)
	{
		ChangeDisplaySettings( NULL, 0);
		ShowCursor( true);
	}

	if ( hRC)
	{
		wglMakeCurrent( NULL, NULL);
		wglDeleteContext( hRC);
		hRC = NULL;
	}
}

void GLWindow::initGL(int bits)
{
	unsigned int		PixelFormat;
	static	PIXELFORMATDESCRIPTOR pfd;

	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = bits;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	hDC = GetDC(hWnd);	
	PixelFormat = ChoosePixelFormat(hDC,&pfd);
	SetPixelFormat(hDC,PixelFormat,&pfd);
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC,hRC);	
}

BOOL GLWindow::createGLWindow(char* title, WNDPROC WndProc)
{
	WNDCLASS	wc;
	DWORD		dwExStyle;	
	DWORD		dwStyle;
	RECT		WindowRect;

	int	width = isFullscreen?videoModes[fullscreenMode].width:videoModes[windowedMode].width;
	int	height = isFullscreen?videoModes[fullscreenMode].height:videoModes[windowedMode].height;
	int	bits = isFullscreen?videoModes[fullscreenMode].bpp:videoModes[windowedMode].bpp;

	windowPosition = Point(nativeMode.dmPelsWidth/2 - width/2, nativeMode.dmPelsHeight/2 - height/2);

	int	x = isFullscreen?0:windowPosition.x;
	int	y = isFullscreen?0:windowPosition.y;

	WindowRect.left = (long) x;
	WindowRect.right = (long) x + width;
	WindowRect.top = (long) y;	
	WindowRect.bottom = (long) y + height;

	hInstance			= GetModuleHandle(NULL);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;	
	wc.lpszMenuName		= NULL;	
	wc.lpszClassName	= __TEXT("OGL");  

	RegisterClass(&wc);

	if (isFullscreen)
	{
		DEVMODE dm;								
		memset(&dm,0,sizeof(dm));
		dm.dmSize=sizeof(dm);
		dm.dmPelsWidth	= width;
		dm.dmPelsHeight	= height;
		dm.dmBitsPerPel	= bits;	
		dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&dm,CDS_FULLSCREEN);		
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor(false);
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		if(windowedMode == 0)
			dwStyle = WS_OVERLAPPED | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU;
		else
			dwStyle = WS_OVERLAPPEDWINDOW | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU;
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);

	hWnd=CreateWindowEx(	dwExStyle,
		__TEXT("OGL"),
		title,
		dwStyle |
		WS_CLIPSIBLINGS |
		WS_CLIPCHILDREN,
		WindowRect.left, WindowRect.top,
		WindowRect.right-WindowRect.left,
		WindowRect.bottom-WindowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL);	

	RECT rcClient,rcWind;
	GetClientRect(hWnd, &rcClient); 
	GetWindowRect(hWnd, &rcWind); 
	borderWidth = 2*((rcWind.right - rcWind.left) - rcClient.right)/2; 
	borderHeight = 2*((rcWind.bottom - rcWind.top) - rcClient.bottom)/2; 

	initGL(bits);

	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	return true;
}

void GLWindow::createGLWindow(bool togglefullscreen)
{

	if(togglefullscreen)
	{
		isFullscreen = !isFullscreen;	
		destroyGL();
	}

	RECT		WindowRect;

	int	width = isFullscreen?videoModes[fullscreenMode].width:videoModes[windowedMode].width;
	int	height = isFullscreen?videoModes[fullscreenMode].height:videoModes[windowedMode].height;
	int	bits = isFullscreen?videoModes[fullscreenMode].bpp:videoModes[windowedMode].bpp;

	windowPosition = Point(nativeMode.dmPelsWidth/2 - width/2, nativeMode.dmPelsHeight/2 - height/2);

	int	x = isFullscreen?0:windowPosition.x;
	int	y = isFullscreen?0:windowPosition.y;

	WindowRect.left = (long) x;
	WindowRect.right = (long) x + width;
	WindowRect.top = (long) y;	
	WindowRect.bottom = (long) y + height;

	if(isFullscreen) 
	{
		SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW );
		SetWindowLongPtr(hWnd, GWL_STYLE, 
			WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
		SetWindowPos(hWnd,HWND_TOP, 0, 0, width, height, SWP_SHOWWINDOW);

		DEVMODE dm;
		dm.dmSize = sizeof(DEVMODE);
		dm.dmPelsWidth = width;
		dm.dmPelsHeight = height;
		dm.dmBitsPerPel = bits;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		ChangeDisplaySettings(&dm,CDS_FULLSCREEN);
		ShowCursor(false);
	}
	else
	{
		DWORD dwStyle;
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	
		if(windowedMode == 0)
			dwStyle = WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU;
		else
			dwStyle = WS_OVERLAPPEDWINDOW | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU;
		SetWindowLongPtr(hWnd, GWL_EXSTYLE,  dwExStyle);
		SetWindowLongPtr(hWnd, GWL_STYLE, dwStyle);
		AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);
		SetWindowPos(hWnd,HWND_TOP, WindowRect.left, WindowRect.top,
			WindowRect.right-WindowRect.left,
			WindowRect.bottom-WindowRect.top, SWP_SHOWWINDOW);
		if(togglefullscreen)
			ChangeDisplaySettings(NULL, 0);
	}

	if(togglefullscreen)
		initGL(bits);
}

int GLWindow::currentWidth()
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient); 
	return rcClient.right - rcClient.left;	
}

int GLWindow::currentHeight()
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient); 
	return rcClient.bottom - rcClient.top;
}

int GLWindow::currentMode()
{
	return isFullscreen?fullscreenMode:windowedMode;
}