//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	le opengl & window handling
//-----------------------------------------------------------------------------

#include "graphics.h"
#include "resource.h"
#include <Uxtheme.h>

#pragma warning(disable:4244)

GLWindow::GLWindow()
{
	hDC = NULL;
	hRC = NULL;
	hWnd = NULL;
}

GLWindow::~GLWindow()
{
	destroyGLWindow();
}

void GLWindow::initialize()
{
	isActive			= true;
	isFullscreen		= false;
	windowedMode	= 1; 
	fullscreenMode = 0; 

	EnumDisplaySettings (NULL, ENUM_CURRENT_SETTINGS, &nativeMode);

	videoModes[0] = VideoMode(4*nativeMode.dmPelsWidth/4, 4*nativeMode.dmPelsHeight/4, 32);
	videoModes[1] = VideoMode(3*nativeMode.dmPelsWidth/4, 3*nativeMode.dmPelsHeight/4, 32);
	videoModes[2] = VideoMode(2*nativeMode.dmPelsWidth/4, 2*nativeMode.dmPelsHeight/4, 32);
}

void GLWindow::destroyGLWindow()
{
	destroyGL();
	ShowTaskBar(true);
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
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW | WS_MINIMIZEBOX  | WS_SYSMENU | WS_VISIBLE;
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);

	HMENU		hMenu;
	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(ID_MENU));

	HBRUSH hBrush = CreateSolidBrush(RGB(150, 150, 150)); 
	MENUINFO mi = { 0 }; 
	mi.cbSize = sizeof(mi); 
	mi.fMask = MIM_BACKGROUND|MIM_APPLYTOSUBMENUS; 
	mi.hbrBack = hBrush;
	SetMenuInfo(hMenu, &mi);

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
		hMenu,
		hInstance,
		NULL);	

	if(!isFullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	
		dwStyle = WS_OVERLAPPEDWINDOW | WS_MINIMIZEBOX  | WS_SYSMENU | WS_VISIBLE;
		SetWindowLongPtr(hWnd, GWL_EXSTYLE,  dwExStyle);
		SetWindowLongPtr(hWnd, GWL_STYLE, dwStyle);
	}
	
	SetWindowTheme(hWnd, L" ", L" ");

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
	}
	else
	{
		if(currentMode() == 0)
			SystemParametersInfo( SPI_GETWORKAREA, 0, &WindowRect, 0 );
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	
		DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_MINIMIZEBOX  | WS_SYSMENU | WS_VISIBLE;
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
	return isFullscreen?videoModes[fullscreenMode].width:videoModes[windowedMode].width;
}

int GLWindow::currentHeight()
{
	return isFullscreen?videoModes[fullscreenMode].height:videoModes[windowedMode].height;
}

int GLWindow::currentMode()
{
	return isFullscreen?fullscreenMode:windowedMode;
}

wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString=new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

void ShowTaskBar(bool show)
{
	HWND taskbar = FindWindow("Shell_TrayWnd", NULL);
	HWND start = FindWindow("Button", NULL);

	if (taskbar != NULL) 
	{
		ShowWindow(taskbar, show ? SW_SHOW : SW_HIDE);
		UpdateWindow(taskbar);
	}  
	if (start != NULL) 
	{ 
		ShowWindow(start, show ? SW_SHOW : SW_HIDE);
		UpdateWindow(start);
	}
}
