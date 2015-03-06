//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	main header
//-----------------------------------------------------------------------------

#ifndef MAIN_H
#define MAIN_H

#include <windows.h>
#include <GL\glew.h>
#include <stdio.h>
#include "graphics.h"
#include "utils.h"
#include "objects.h"
#include "collisions.h"
#include "tile.h"
#include "textfile.h"
#include "entity.h"
#include "input.h"
#include "sound.h"
#include "texture.h"
#include "text.h"
#include "menu.h"
#include "rendertarget.h"
#include "event.h"
#include "world.h"
#include "time.h"

#include "shared.h"

LRESULT 
	CALLBACK	WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void		P_ToggleFullScreen(HINSTANCE hInstance,HWND* hWnd);
void		P_Init();
void		P_ComputeFPS();
void		P_Cleanup();
void		P_Run();
void		P_Pause(bool pause);

void		R_Resize(int w, int h);
void		R_Draw(HDC hDC);
void		R_DrawScreenDebug();
void		R_DrawWorldDebug();
void		R_SetClientOrthoMatrix();
void		R_SetMenuOrthoMatrix();
void		R_SetPPOrthoMatrix();
void		R_Update();
void		R_DrawMenus();
void		R_DrawSky(RenderTarget target);
void		R_DrawSceneLights(TileMap* ptilemap, float ls);
void		R_DrawSelectedMenuItem(MenuItem item, float tx, float ty, float tw, float th);

void		G_Init();
void		G_Update(double elapsedTime);

void		I_UpdateInGame(Input* pinput);
void		I_UpdateInProgram(Input* pinput);
void		I_UpdateInMenu(Input* pinput);
void		I_ResetControls();
void		I_ZeroGameInput();

void		GL_Init();
void		GL_InitShaders();
int			GL_InitFBOs();
void		GL_InitTextures();
void		GL_LoadShader(unsigned int &window, char* vsfile,char* fsfile);

void		M_Init();

void		E_Init();
bool		E_PlayEvent(std::string name, bool loop);
bool		E_StopEvent(std::string name);

void		U_Load_s1();
void		U_Load_s2();
void		U_Load_demo();

typedef std::vector<Timeline> Events;

class Game
{
public:
	ScreenRect				clientRect;
	ScreenRect				sceneRect;
	float					aspectRatio;

	struct RenderTargets
	{
		RenderTarget	back;
		RenderTarget	menu;
		RenderTarget	menuPanel;
		RenderTarget	lighting;
		RenderTarget	temp0;
		RenderTarget	temp1;
		RenderTarget	temp2;
		RenderTarget	scene;

	} renderTargets;

	struct Textures
	{
		unsigned int	sprites;
		unsigned int	tiles;
		unsigned int	bgs;
		unsigned int	abgs;
		unsigned int	font;
		unsigned int	light;
		unsigned int	cursor;
		unsigned int	slider;

	} textures;

	struct Atlases
	{
		TextureAtlas	sprites;
		TextureAtlas	tiles;
		TextureAtlas	bgs;
		TextureAtlas	abgs;
		TextureAtlas	font;

	} atlases;

	struct Shaders
	{
		unsigned int	vertex;
		unsigned int	fragment;
		unsigned int	horizontalBlur;
		unsigned int	verticalBlur;
		unsigned int	wave;
		unsigned int	bilinear;
		unsigned int	sky;
		unsigned int	curvature;

		int				uniformOffset;
		int				uniformCurvature;
		int				uniformHBlurSize;
		int				uniformVBlurSize;
		int				uniformHBilinearSize;
		int				uniformVBilinearSize;

		float			blurValue;
		float			curvValue;

	} shaders;

	struct Menu : public StaticObject
	{
		int				currentItem;
		MenuList*		current;
		MenuList		start;
		MenuList		pause;
		MenuList		options;
		MenuList		video;
		MenuList		sound;
		bool			sliderCaptured;
		Point			capturePoint;
		float			savedVolume;
		float			menuDarken;

	} menu;

	struct TimeLineClips
	{
		Timeline		demo;
		Timeline		gameStart;
		Timeline		gameQuit;
		Timeline		fadeIn;
		Timeline		fadeOut;
	} clips;
	
	Events				events;

};

#endif