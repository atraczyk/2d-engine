//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	le main header
//-----------------------------------------------------------------------------

#ifndef MAIN_H
#define MAIN_H

#pragma warning(disable: 4996)
#pragma warning(disable: 4244)
#pragma warning(disable: 4018)

#include <windows.h>
#include <GL\glew.h>
#include <stdio.h>
#include "graphics.h"
#include "utils.h"
#include "objects.h"
#include "collisions.h"
#include "tile.h"
#include "textfile.h"
#include "input.h"
#include "texture.h"
#include "text.h"
#include "rendertarget.h"
#include "resource.h"

#define		S_PER_GAME_UPDATE		(1.0f / 60.0f)
#define		S_PER_FRAME				(1.0f / 120.0f)
#define		MS_PER_FRAME			(int)(S_PER_FRAME*1000.0f)
#define		CLAMP(x,MIN,MAX)		min(max(MIN,x),MAX)	

LRESULT 
CALLBACK	WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT 
CALLBACK	P_DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void		P_computeFPS();
void		P_cleanup();
void		P_run();

void		R_draw(HDC hDC);
void		R_updateUniforms();
void		R_setPostProcessingOrtho();
void		R_drawScreenDebug();
void		R_drawWorldDebug();
void		R_setScreenOrtho();
void		R_DrawHorizontalLine(float x, float y, float w, float thickness);
void		R_DrawVerticalLine(float x, float y, float h, float thickness);

double		T_GetCurrentTime();
int			T_GetCurrentMilliseconds();

void		G_update(double elapsedTime);
void		G_initializeGame();

void		I_updateGameInput();
void		I_updateProgramInput(Input* pinput);

void		GL_initializeGLObjects();
int			GL_initializeFrameBuffers();

void		D_OpenTileMapDialog(HWND hwnd, int layer);
void		D_SaveTileMapDialog(HWND hwnd, int layer);

#endif