//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	main header
//-----------------------------------------------------------------------------

#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <GL\glew.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
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

void		P_toggleFullScreen(HINSTANCE hInstance,HWND* hWnd);
void		P_computeFPS();
void		P_cleanup();
void		P_run();
void		P_pause(bool pause);

void		R_resize(int w, int h);
void		R_render(HDC hDC);
void		R_drawScreenDebug();
void		R_drawWorldDebug();
void		R_setClientOrthoMatrix();
void		R_setMenuOrthoMatrix();
void		R_setPPOrthoMatrix();
void		R_updateUniforms();
void		R_drawMenus();
void		R_drawSky(RenderTarget target);
void		R_drawSceneLights(TileMap* ptilemap, float ls);
void		R_drawSelectedMenuItem(MenuItem item, float tx, float ty, float tw, float th);

void		G_initializeGame();
void		G_update(double elapsedTime);
void		G_initializeEvents();
bool		G_playEvent(std::string name, bool loop);
bool		G_stopEvent(std::string name);
void		G_initializeMenus();

void		I_updateGameInput(Input* pinput);
void		I_updateProgramInput(Input* pinput);
void		I_updateMenuInput(Input* pinput);
void		I_resetControls();
void		I_zeroGameInput();

void		GL_initializeGLObjects();
void		GL_initializeShaders();
int			GL_initializeFrameBuffers();
void		GL_initializeTextures();
void		GL_loadShader(unsigned int &window, char* vsfile,char* fsfile);

void		U_Load_s1();
void		U_Load_s2();
void		U_Load_s3();
void		U_Load_demo();

typedef std::vector<Timeline> Events;

struct AtlasInfo
{
	string file;
	int framesWide;
	int framesHigh;
	int frameWidth;
	int frameHeight;
};

class Game
{
public:
	ScreenRect				clientRect;
	ScreenRect				sceneRect;
	float					aspectRatio;

	struct 
	{
		AtlasInfo sprites;
		AtlasInfo tiles;
		AtlasInfo bgs;

	} atlasInfo;

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
		unsigned int	font;
		unsigned int	light;
		unsigned int	cursor;
		unsigned int	slider;

	} textures;

	struct Atlases
	{
		SpriteAtlas test;
		TileAtlas	sprites;
		TileAtlas	tiles;
		TileAtlas	bgs;
		TileAtlas	font;

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

	void loadConfig(const char* filename);
	void initializeTextures();

	Game();
	~Game(){};
};

#endif