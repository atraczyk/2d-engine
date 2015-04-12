//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	le main entry point in winmain
//-----------------------------------------------------------------------------

#include "main.h"

//settings 
bool		bdebugtext			= true;
bool		btogglefullscreen	= false;
bool		bmodechange			= false;
bool		bdoublebuffer		= false;
bool		bpause				= false;
bool		bdrawbglayer01		= true;
bool		bdrawbglayer02		= true;
bool		bdrawbglayer03		= true;
bool		blayer01loaded		= false;
bool		blayer02loaded		= false;
bool		beditcollisions		= true;
bool		beditimage			= true;

//rendertargets & textures
RenderTarget	rt_back;
RenderTarget	debug_rendertarget;
RenderTarget	rt_temp0;
RenderTarget	rt_temp1;
RenderTarget	rt_scene;
unsigned int	texture_sprite,texture_tile,texture_bg,texture_bga;

//time
int			fps_frame=0,fps_time,last_fps_time=0;
float		fps = 0.0f;
float		fps_polling_time = 250.0f;
int			currentFrame = 0;
double		pause_time = 0.0f;
double		movesize_time = 0.0f, last_movesize_time = 0.0f;
double		start_time = T_GetCurrentTime();
double		game_time = 0.0f, remaining_game_time = 0.0f, last_game_time = 0.0f;
float		theta = 0.0f;
float		fps_mult = 1.0;
LARGE_INTEGER frequency;

//window
WorldRect		world_constraints;
ScreenRect		client_rect;
ScreenRect		scene_rect;
ScreenRect		s_srcRect;
ScreenRect		s_dstRect;
ScreenRect		lmenuRect;
ScreenRect		rmenuRect;
float			aspect_ratio;

//game
GLWindow		window;
Camera			camera;

TileAtlas		atlas_tile;
TileAtlas		atlas_collision;
TileAtlas		atlas_font;

TileMap			tilemap_back;
TileMap			tilemap_front;
TileMap*		pselected_tilemap = &tilemap_back;

Point			rclick;
Point			highlightedTile(0,0);
int				highlightedBrush = 0;
int				highlightedCBrush = 0;
int				selectedCBrush = 0;
Point			selectedTile(0,0);
int				selectedBrush = 0;
bool			bdraggingworld = false;
bool			bdraggingrmenu = false;
float			rmenuyoffset = 0.0f;
Point			tilethumbsize;
int				tilesperrow = 7;
float			lmenuw = 256;
float			rmenuw = 256;
Input			input;
Font			smallfont,bigfont;
bool			hascollisionmap = false;

extern TileAnimation tileAnimations[]; 

void G_initializeGame()
{
	window.initialize();

	scene_rect = ScreenRect(0,0,window.videoModes[0].width,window.videoModes[0].height);
	client_rect = ScreenRect(0,0,window.currentWidth(),window.currentHeight());
	lmenuRect = ScreenRect(0,0,lmenuw,client_rect.h);
	rmenuRect = ScreenRect(client_rect.w - rmenuw,0,rmenuw,client_rect.h);
	aspect_ratio = client_rect.w / client_rect.h;

	camera.initialize(-(256 + 16) * (scene_rect.w/client_rect.w), -16, scene_rect.w , scene_rect.h );

	atlas_tile.initialize(&texture_sprite, 16, 16, 16, 16);
	atlas_collision.initialize(&texture_bga, 4, 4, 64, 64);
	atlas_font.initialize(&texture_bg, 16, 16, 8, 8);

	tilethumbsize = atlas_tile.imageFrameSize*2.0f;

	initializeTileAnimations();

	world_constraints = WorldRect(-10000, -10000,81000, 81000);

	smallfont.initialize(16, 16, 8, 8, 1.0f, &camera, &client_rect, &atlas_font);
	bigfont.initialize(16, 16, 8, 8, 2.0f, &camera, &client_rect, &atlas_font);
	InitSpacings();
}

void GL_initializeTextures()
{
	texture_sprite = loadBMPTexture_ARGB("textures/tiles03.bmp");
	texture_tile = loadBMPTexture_ARGB("textures/s_tiles.bmp"); 
	texture_bg = loadBMPTexture_ARGB("textures/font_8x8.bmp");
	texture_bga = loadBMPTexture_ARGB("textures/collision_icons.bmp");
}

void GL_initializeGLObjects() 
{
	glewInit();
	GL_initializeTextures();
	GL_initializeFrameBuffers();	
}

void P_computeFPS()
{
	fps_frame++;
	fps_time= T_GetCurrentMilliseconds();

	if (fps_time - last_fps_time > fps_polling_time) 
	{
		fps = fps_frame*1000.0/(fps_time-last_fps_time);
		last_fps_time = fps_time;
		fps_frame = 0;
	}
}

double T_GetCurrentTime() 
{
	static LARGE_INTEGER frequency;
	LARGE_INTEGER currentTime;

	if (frequency.QuadPart == 0) 
	{
		QueryPerformanceFrequency(&frequency);
	}
	QueryPerformanceCounter(&currentTime);

	return (double) currentTime.QuadPart / frequency.QuadPart;
}

int T_GetCurrentMilliseconds() 
{
	return (int)(1000*T_GetCurrentTime());
}

bool PointinRect(Point p, ScreenRect r)
{
	return p.x >= r.fLeft() && p.x <= r.fRight() && p.y<=r.fBottom() && p.y >= r.fTop();
}

void G_update(double elapsed_time)
{
	input.update();
	I_updateProgramInput(&input);
	I_updateGameInput();

	game_time += elapsed_time;

	currentFrame++;
	theta += 0.5f;

	tilemap_back.update();
	tilemap_front.update();

	camera.update(world_constraints);

}

template <typename T>
void remove(std::vector<T>& vec, size_t pos)
{
	std::vector<T>::iterator it = vec.begin();
	std::advance(it, pos);
	vec.erase(it);
}

void I_updateGameInput()
{		
	if(PointinRect(Point(input.mouse.lX,input.mouse.lY),rmenuRect))
	{
		if( input.mouse.buttonPressed(MLBUTTON))
			selectedBrush = highlightedBrush;
	}
	else if(PointinRect(Point(input.mouse.lX,input.mouse.lY),s_dstRect))
	{
		if(input.mouse.buttonDown(MLBUTTON))
		{
			if(!input.keyboard.keyDown(VK_CONTROL))
				selectedTile = highlightedTile;
			else 
			{
				if(pselected_tilemap->t_mapSize.x && pselected_tilemap->t_mapSize.y && beditimage)
					pselected_tilemap->imageLayer[highlightedTile.x][highlightedTile.y].tileIndex = selectedBrush;
				if(pselected_tilemap->has_c_Map && beditcollisions)
					pselected_tilemap->collisionLayer[highlightedTile.x][highlightedTile.y].tileIndex = selectedCBrush;
				if(isAnimatedTile(selectedBrush))
				{
					pselected_tilemap->animatedTiles.push_back(AnimatedTile(Point(highlightedTile.x,highlightedTile.y),tileAnimations[selectedBrush - 240]));
					pselected_tilemap->imageLayer[highlightedTile.x][highlightedTile.y].atileIndex = selectedBrush;
				}
				else
				{
					for(int i=0;i<pselected_tilemap->animatedTiles.size();i++)
					{
						if(pselected_tilemap->animatedTiles[i].position.x == highlightedTile.x && pselected_tilemap->animatedTiles[i].position.y == highlightedTile.y)
							remove(pselected_tilemap->animatedTiles,i);
					}
					pselected_tilemap->imageLayer[highlightedTile.x][highlightedTile.y].atileIndex = 0;
				}
			}
		}
	}
	else if(PointinRect(Point(input.mouse.lX,input.mouse.lY),lmenuRect))
	{
		if( input.mouse.buttonPressed(MLBUTTON))
			selectedCBrush = highlightedCBrush;
	}

	if(	input.keyboard.keyDown('Q'))
		camera.Zoom(10.0f);
	if(	input.keyboard.keyDown('E'))
		camera.Zoom(-10.0f);
	if(	input.keyboard.keyPressed('D'))
		camera.toMove.x = 1;
	if(	input.keyboard.keyPressed('A'))
		camera.toMove.x = -1;
	if(	input.keyboard.keyPressed('W'))
		camera.toMove.y = 1;
	if(	input.keyboard.keyPressed('S'))
		camera.toMove.y = -1;
	if(	input.keyboard.keyReleased('D'))
		camera.toMove.x==1?camera.toMove.x=0:NULL;
	if(	input.keyboard.keyReleased('A'))
		camera.toMove.x==-1?camera.toMove.x=0:NULL;
	if(	input.keyboard.keyReleased('W'))
		camera.toMove.y==1?camera.toMove.y=0:NULL;
	if(	input.keyboard.keyReleased('S'))
		camera.toMove.y==-1?camera.toMove.y=0:NULL;

}

void I_updateProgramInput(Input* pinput)
{
	//options
	if(	pinput->keyboard.keyReleased('1')) 
		bdrawbglayer01 = !bdrawbglayer01;
	if(	pinput->keyboard.keyReleased('2')) 
		bdrawbglayer02 = !bdrawbglayer02;
	if(	pinput->keyboard.keyReleased('3')) 
		bdrawbglayer03 = !bdrawbglayer03;

	if(	pinput->keyboard.keyReleased('M')) 
		pselected_tilemap = &tilemap_front;
	if(	pinput->keyboard.keyReleased('N')) 
		pselected_tilemap = &tilemap_back;
	if(	pinput->keyboard.keyReleased('C')) 
		beditcollisions = !beditcollisions;
	if(	pinput->keyboard.keyReleased('V')) 
		beditimage = !beditimage;

	if(	pinput->keyboard.keyReleased('J')) 
	{
		if(!window.isFullscreen && window.windowedMode < NUM_MODES - 1)
		{
			bmodechange = true;
			window.windowedMode = CLAMP(window.windowedMode + 1,0,NUM_MODES -1);
		}
	}
	if(	pinput->keyboard.keyReleased('K')) 
	{
		if(!window.isFullscreen && window.windowedMode > 0)
		{
			bmodechange = true;
			window.windowedMode = CLAMP(window.windowedMode - 1,0,NUM_MODES -1);
		}
	}

	if(	pinput->keyboard.keyReleased(VK_F1)) 
		btogglefullscreen = true;
	if(	pinput->keyboard.keyReleased(VK_F2)) 
		bdebugtext = !bdebugtext;
	if(	pinput->keyboard.keyReleased(VK_F6)) 
		bdoublebuffer = window.isFullscreen?bdoublebuffer:!bdoublebuffer;

	if(	pinput->keyboard.keyPressed(VK_ESCAPE) ||
		pinput->gamepad.buttonPressed(GPB_SL))
		PostQuitMessage(0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, int iCmdShow)
{	
	MSG msg;
	BOOL quit = FALSE;

	G_initializeGame();
	window.createGLWindow("Level Editor", WndProc);
	GL_initializeGLObjects();

	ShowWindow(window.hWnd,SW_SHOW);

	last_game_time = T_GetCurrentTime();

	while ( !quit )
	{
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )  )
		{
			if ( msg.message == WM_QUIT ) 
			{
				quit = TRUE;
			} 
			else 
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		} 
		else 
		{
			P_run();
		}
	}

	P_cleanup();
	window.destroyGLWindow();
	return msg.wParam;
}

int GL_initializeFrameBuffers()
{
	debug_rendertarget.Create(client_rect.w, client_rect.h, false);
	rt_temp0.Create(client_rect.w, client_rect.h, false);
	rt_temp1.Create(client_rect.w, client_rect.h, false);
	rt_scene.Create(scene_rect.w,scene_rect.h, false);
	rt_back.fbo = 0;

	return 1;
}

void P_cleanup()
{
	debug_rendertarget.Delete();
	rt_temp0.Delete();
	rt_temp1.Delete();
	rt_scene.Delete();

	glDeleteTextures(1, &texture_sprite);
	glDeleteTextures(1, &texture_tile);
	glDeleteTextures(1, &texture_bg);
}

void R_drawScreenDebug()
{	
	if(bdebugtext)
	{
		int x = 0;
		int y = 0;

		char buffer[128];
		Vector4 color;

		int ty = y + 8; int dy = smallfont.charsXYWH.h * smallfont.scaleValue;
		color = Vector4(0.0f,0.0f,0.0f,1.0f);
		sprintf (buffer, "fps    = %0.2f", fps);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "ttime  = %0.4f", (T_GetCurrentMilliseconds()/1000.0f) - start_time);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "gtime  = %0.4f", game_time);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "client = %d x %d", (int)client_rect.w,(int)client_rect.h);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "render = %d x %d", (int)scene_rect.w,(int)scene_rect.h);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "camera = (%0.1f,%0.1f)", camera.position.x,camera.position.y);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "ratio  = %f", aspect_ratio);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "ratio2 = %f", scene_rect.w/scene_rect.h);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "layers = (%d,%d)", (int)(bdrawbglayer01),(int)(bdrawbglayer02));
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "mouse  = (%d,%d)", input.mouse.lX,input.mouse.lY);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "m.lb   = (%d)", (int)input.mouse.buttonDown(MLBUTTON));
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "sel    = (%d,%d)", highlightedTile.x,highlightedTile.y);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "tilel1   = (%d,%d)", tilemap_back.t_mapSize.x,tilemap_back.t_mapSize.y);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "l1cmap   = (%d,%d)", tilemap_back.t_cmapSize.x,tilemap_back.t_cmapSize.y);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "tilel2   = (%d,%d)", tilemap_front.t_mapSize.x,tilemap_front.t_mapSize.y);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "t has c  = %d", (int)tilemap_back.has_c_Map);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
		sprintf (buffer, "mapxy  = (%d,%d)", (int)tilemap_back.position.x,(int)tilemap_back.position.y);
		ty += dy; smallfont.displayScreenText(x + 8,ty,color.x,color.y,color.z,buffer);
	}
}

void DrawRightPanel()
{
	float x,y,w,h;
	float border = 2.0f;

	x = rmenuRect.x + border;
	y = rmenuRect.y + border - 1;
	w = rmenuRect.w - 2*border;
	h = rmenuRect.h - 2*border + 1;
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.5882f, 0.5882f, 0.5882f, 1.0f);
	glBegin(GL_QUADS);
	{
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);		
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, *(atlas_tile.texture));
	w = tilethumbsize.x;
	h = tilethumbsize.y;
	x = rmenuRect.x + border + 8.0f;
	y = (rmenuRect.y + border - 1 + 2.0f) + rmenuyoffset + h;	
	ScreenRect srcRect,dstRect,hl_dstRect,sel_dstRect;

	for(int i=0;i<atlas_tile.totalFrames;)
	{
		glBegin(GL_QUADS);
		{
			srcRect = atlas_tile.sourceRectangles[i];
			dstRect = ScreenRect(x,y,w,h);
			glTexCoord2d(srcRect.fLeft(), srcRect.fTop());	glVertex2f(dstRect.fLeft(), dstRect.fTop());
			glTexCoord2d(srcRect.fRight(), srcRect.fTop());	glVertex2f(dstRect.fRight(), dstRect.fTop());
			glTexCoord2d(srcRect.fRight(), srcRect.fBottom());	glVertex2f(dstRect.fRight(),  dstRect.fBottom());
			glTexCoord2d(srcRect.fLeft(), srcRect.fBottom());	glVertex2f(dstRect.fLeft(),  dstRect.fBottom());

		}
		glEnd();
		if(i == highlightedBrush)
			hl_dstRect = dstRect;
		if(i == selectedBrush)
			sel_dstRect = dstRect;
		x += w + 2.0f;
		i++;
		if(!fmod(i,tilesperrow))
		{
			x = rmenuRect.x + border + 8.0f;
			y += h + 2.0f;
		}		
	}

	//highlightedBrush
	float lw = 1.0f;
	glDisable(GL_TEXTURE_2D);
	//selected
	glColor4f(0.5f, 1.0f, 0.5f, 1.0f);
	R_DrawHorizontalLine(sel_dstRect.x - lw, sel_dstRect.y, sel_dstRect.w + 2*lw, lw);
	R_DrawVerticalLine(sel_dstRect.x, sel_dstRect.y , sel_dstRect.h, lw);
	R_DrawHorizontalLine(sel_dstRect.x - lw, sel_dstRect.y + sel_dstRect.h, sel_dstRect.w + 2*lw, lw);
	R_DrawVerticalLine(sel_dstRect.x + sel_dstRect.w, sel_dstRect.y + sel_dstRect.h, -sel_dstRect.h, lw);
	if(PointinRect(Point(input.mouse.lX,input.mouse.lY),rmenuRect))
	{
		x = rmenuRect.x + border + 8.0f;
		y = (rmenuRect.y + border - 1 + 2.0f) + rmenuyoffset + h;
		Point cursor(CLAMP(input.mouse.lX,rmenuw,client_rect.w),input.mouse.lY);
		cursor.x -= x;
		cursor.y -= y;
		highlightedBrush = ((int)(cursor.x/(w + 2.0f)) + tilesperrow*((int)((cursor.y)/(h + 2.0f))));
		highlightedBrush = CLAMP(highlightedBrush,0,atlas_tile.totalFrames-1);

		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		R_DrawHorizontalLine(hl_dstRect.x - lw, hl_dstRect.y, hl_dstRect.w + 2*lw, lw);
		R_DrawVerticalLine(hl_dstRect.x, hl_dstRect.y , hl_dstRect.h, lw);
		R_DrawHorizontalLine(hl_dstRect.x - lw, hl_dstRect.y + hl_dstRect.h, hl_dstRect.w + 2*lw, lw);
		R_DrawVerticalLine(hl_dstRect.x + hl_dstRect.w, hl_dstRect.y + hl_dstRect.h, -hl_dstRect.h, lw);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);

	x = rmenuRect.x + border;
	y = rmenuRect.y + border - 1;
	w = rmenuRect.w - 2*border;
	h = smallfont.charsXYWH.h * smallfont.scaleValue * 3;
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.4882f, 0.4882f, 0.4882f, 1.0f);
	glBegin(GL_QUADS);
	{
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);		
		glColor4f(0.2941f, 0.2941f, 0.2941f, 1.0f);
		glVertex2f(x, y - 1);
		glVertex2f(x + w, y - 1);
		glVertex2f(x + w, y);
		glVertex2f(x, y);
		glVertex2f(x, y + h - 2);
		glVertex2f(x + w, y + h - 2);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);

	char buffer[128];
	Vector4 color;

	int ty = y + 8; int dy = smallfont.charsXYWH.h * smallfont.scaleValue;
	color = Vector4(0.0f,0.0f,0.0f,1.0f);
	sprintf (buffer, "Tile Set = %s", "tiles03.bmp");
	ty += dy; smallfont.displayScreenTextSpacing(x + 8,ty,color.x,color.y,color.z,buffer);
}

void DrawLeftPanel()
{
	float x,y,w,h;
	float border = 2.0f;
	x = lmenuRect.x + border;
	y = lmenuRect.y + border - 1;
	w = lmenuRect.w - 2*border;
	h = lmenuRect.h - 2*border + 1;
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.5882f, 0.5882f, 0.5882f, 1.0f);
	glBegin(GL_QUADS);
	{
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);		
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);

	ScreenRect srcRect,dstRect;
	float top = 210;
	float dx = 32;
	float dw = 120;
	float dh = 128;

	w = tilethumbsize.x*2;
	h = tilethumbsize.y*2;	
	x = lmenuRect.x + border + dx;
	y = lmenuRect.y + border - 1 + top;
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, *(atlas_tile.texture));
	glBegin(GL_QUADS);
	{
		//brush
		srcRect = atlas_tile.sourceRectangles[selectedBrush];
		dstRect = ScreenRect(x,y,w,h);
		glTexCoord2d(srcRect.fLeft(), srcRect.fTop());	glVertex2f(dstRect.fLeft(), dstRect.fTop());
		glTexCoord2d(srcRect.fRight(), srcRect.fTop());	glVertex2f(dstRect.fRight(), dstRect.fTop());
		glTexCoord2d(srcRect.fRight(), srcRect.fBottom());	glVertex2f(dstRect.fRight(),  dstRect.fBottom());
		glTexCoord2d(srcRect.fLeft(), srcRect.fBottom());	glVertex2f(dstRect.fLeft(),  dstRect.fBottom());
	}
	glEnd();
	//cbrush
	glBindTexture(GL_TEXTURE_2D, *(atlas_collision.texture));
	glBegin(GL_QUADS);
	{
		srcRect = atlas_collision.sourceRectangles[selectedCBrush];
		dstRect = ScreenRect(x + dw, y ,w,h);
		glTexCoord2d(srcRect.fLeft(), srcRect.fTop());	glVertex2f(dstRect.fLeft(), dstRect.fTop());
		glTexCoord2d(srcRect.fRight(), srcRect.fTop());	glVertex2f(dstRect.fRight(), dstRect.fTop());
		glTexCoord2d(srcRect.fRight(), srcRect.fBottom());	glVertex2f(dstRect.fRight(),  dstRect.fBottom());
		glTexCoord2d(srcRect.fLeft(), srcRect.fBottom());	glVertex2f(dstRect.fLeft(),  dstRect.fBottom());
	}
	glEnd();

	float lw = 4.0f;
	glDisable(GL_TEXTURE_2D);
	x += -8.0f;
	w += 16.0f;
	y += -8.0f;
	h += 16.0f;
	//tile outline
	glColor4f(0.0f, 0.4f, 0.9f, 1.0f);
	R_DrawHorizontalLine(x - lw, y, w + 2*lw,lw);
	R_DrawVerticalLine(x, y , h, lw);
	R_DrawHorizontalLine(x - lw, y + h, w + 2*lw, lw);
	R_DrawVerticalLine(x + w, y + h, -h, lw);
	//brush outline
	glColor4f(0.0f, 0.9f, 0.4f, 1.0f);
	x += dw;
	R_DrawHorizontalLine(x - lw, y, w + 2*lw,lw);
	R_DrawVerticalLine(x, y , h, lw);
	R_DrawHorizontalLine(x - lw, y + h, w + 2*lw, lw);
	R_DrawVerticalLine(x + w, y + h, -h, lw);
	x -= dw;
	glEnable(GL_TEXTURE_2D);

	char buffer[128];
	Vector4 color;
	int ty = lmenuRect.y + border - 1 + top - 32; int dy = smallfont.charsXYWH.h * smallfont.scaleValue;
	color = Vector4(0.0f,0.0f,0.0f,1.0f);
	sprintf (buffer, "%s", "Image Brush");
	smallfont.displayScreenTextSpacing(x - 16,ty,color.x,color.y,color.z,buffer);
	sprintf (buffer, "index = %d", selectedBrush);
	smallfont.displayScreenTextSpacing(x - 16,ty +dy,color.x,color.y,color.z,buffer);
	sprintf (buffer, "%s", "Collision Brush");
	smallfont.displayScreenTextSpacing(x - 16 + dw, ty,color.x,color.y,color.z,buffer);
	sprintf (buffer, "index = %d", selectedCBrush);
	smallfont.displayScreenTextSpacing(x - 16 + dw, ty + dy,color.x,color.y,color.z,buffer);

	//CBrush icons
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, *(atlas_collision.texture));

	w = 48.0f;
	h = 48.0f;
	x = lmenuRect.x + border + 8.0f;
	y = lmenuRect.y + border - 1 + top + dh;

	ScreenRect hl_dstRect,sel_dstRect;

	for(int i=0;i<atlas_collision.totalFrames;)
	{
		glBegin(GL_QUADS);
		{
			srcRect = atlas_collision.sourceRectangles[i];
			dstRect = ScreenRect(x,y,w,h);
			glTexCoord2d(srcRect.fLeft(), srcRect.fTop());	glVertex2f(dstRect.fLeft(), dstRect.fTop());
			glTexCoord2d(srcRect.fRight(), srcRect.fTop());	glVertex2f(dstRect.fRight(), dstRect.fTop());
			glTexCoord2d(srcRect.fRight(), srcRect.fBottom());	glVertex2f(dstRect.fRight(),  dstRect.fBottom());
			glTexCoord2d(srcRect.fLeft(), srcRect.fBottom());	glVertex2f(dstRect.fLeft(),  dstRect.fBottom());

		}
		glEnd();
		if(i == highlightedCBrush)
			hl_dstRect = dstRect;
		if(i == selectedCBrush)
			sel_dstRect = dstRect;
		x += w + 8.0f;
		i++;
		if(!fmod(i,4))
		{
			x = lmenuRect.x + border + 8.0f;
			y += h + 8.0f;
		}		
	}

	//highlightedCBrush
	lw = 1.0f;
	glDisable(GL_TEXTURE_2D);
	//selected
	glColor4f(0.5f, 1.0f, 0.5f, 1.0f);
	R_DrawHorizontalLine(sel_dstRect.x - lw, sel_dstRect.y, sel_dstRect.w + 2*lw, lw);
	R_DrawVerticalLine(sel_dstRect.x, sel_dstRect.y , sel_dstRect.h, lw);
	R_DrawHorizontalLine(sel_dstRect.x - lw, sel_dstRect.y + sel_dstRect.h, sel_dstRect.w + 2*lw, lw);
	R_DrawVerticalLine(sel_dstRect.x + sel_dstRect.w, sel_dstRect.y + sel_dstRect.h, -sel_dstRect.h, lw);
	if(PointinRect(Point(input.mouse.lX,input.mouse.lY),lmenuRect))
	{
		x = lmenuRect.x + border + 8.0f;
		y = lmenuRect.y + border - 1 + top + dh;
		Point cursor(CLAMP(input.mouse.lX,0,lmenuRect.fRight()),input.mouse.lY);
		cursor.x -= x;
		cursor.y -= y;
		highlightedCBrush = ((int)(cursor.x/(w + 8.0f)) + 4*((int)((cursor.y)/(h + 8.0f))));
		highlightedCBrush = CLAMP(highlightedCBrush,0,atlas_collision.totalFrames-1);

		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		R_DrawHorizontalLine(hl_dstRect.x - lw, hl_dstRect.y, hl_dstRect.w + 2*lw, lw);
		R_DrawVerticalLine(hl_dstRect.x, hl_dstRect.y , hl_dstRect.h, lw);
		R_DrawHorizontalLine(hl_dstRect.x - lw, hl_dstRect.y + hl_dstRect.h, hl_dstRect.w + 2*lw, lw);
		R_DrawVerticalLine(hl_dstRect.x + hl_dstRect.w, hl_dstRect.y + hl_dstRect.h, -hl_dstRect.h, lw);
	}

	glEnable(GL_TEXTURE_2D);
	R_drawScreenDebug();
}

void DrawClientBorder()
{
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.3921f, 0.3921f, 0.3921f, 1.0f);
	float linewidth = 1.0f;
	glLineWidth(linewidth);
	glBegin(GL_LINES);
	{
		glVertex2f(client_rect.w, 0 - 1);
		glVertex2f(client_rect.w, client_rect.h - 1);

		glVertex2f(client_rect.w, client_rect.h - 1);
		glVertex2f(0, client_rect.h - 1);

		glVertex2f(0 + 1, client_rect.h - 1);
		glVertex2f(0 + 1, 0 - 1);
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void R_DrawHorizontalLine(float x, float y, float w, float thickness)
{
	glBegin(GL_QUADS);
	{
		glVertex2f(x, y - thickness);
		glVertex2f(x + w, y - thickness);
		glVertex2f(x + w, y + thickness);
		glVertex2f(x, y + thickness);		
	}
	glEnd();
}

void R_DrawVerticalLine(float x, float y, float h, float thickness)
{
	glBegin(GL_QUADS);
	{
		glVertex2f(x - thickness, y);
		glVertex2f(x + thickness, y);
		glVertex2f(x + thickness, y + h);
		glVertex2f(x - thickness, y + h);		
	}
	glEnd();
}

void R_drawWorldDebug()
{	
	glUseProgramObjectARB(NULL);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glDisable(GL_TEXTURE_2D);

	float x0,y0,x1,y1,w,h;

	//axis
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	{

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		x0 = max(-10000,camera.position.x);
		x1 = min(10000,camera.position.x+camera.size.x);
		glVertex2f(x0, 0);
		glVertex2f(x1, 0);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		y0 = max(-10000,camera.position.y);
		y1 = min(10000,camera.position.y+camera.size.y);
		glVertex2f(0, y0);
		glVertex2f(0, y1);
	}
	glEnd();

	//grid
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	{
		w = tilemap_back.w_tileSize.x;
		h = tilemap_back.w_tileSize.y;
		for (int y = tilemap_back.Min.y; y < tilemap_back.Max.y; y++)
		{
			for (int x = tilemap_back.Min.x; x < tilemap_back.Max.x; x++)
			{
				x0 = tilemap_back.position.x + x * w;
				y0 = tilemap_back.position.y + y * h;

				glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
				glVertex2f(x0, y0);
				glVertex2f(x0, y0 + h);
				glVertex2f(x0, y0);
				glVertex2f(x0 + w, y0);

				glColor4f(1.0f, 0.0f, 1.0f, 0.5f);
				if(y == tilemap_back.Max.y-1)
				{
					glVertex2f(x0, y0 + h);
					glVertex2f(x0 + w, y0 + h);
				}
				glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
				if(x == tilemap_back.Max.x-1)
				{
					glVertex2f(x0 + w, y0);
					glVertex2f(x0 + w, y0 + h);
				}
			}
		}
	}
	glEnd();

	//highlightedTile
	if(PointinRect(Point(input.mouse.lX,input.mouse.lY),s_dstRect))
	{
		Point cursor(CLAMP(input.mouse.lX,lmenuw,client_rect.w - rmenuw),input.mouse.lY);
		Vector2 worldcursor = screenToWorld(cursor,camera,client_rect);
		highlightedTile = worldToTile(worldcursor - pselected_tilemap->position, pselected_tilemap->w_tileSize);
		highlightedTile.x = CLAMP(highlightedTile.x,0,pselected_tilemap->t_mapSize.x-1);
		highlightedTile.y = CLAMP(highlightedTile.y,0,pselected_tilemap->t_mapSize.y-1);
	}

	float lw = 2.0f;
	ScreenRect srcRect,dstRect;

	w = pselected_tilemap->w_tileSize.x;
	h = pselected_tilemap->w_tileSize.y;		
	x0 = pselected_tilemap->position.x + highlightedTile.x * w;
	y0 = pselected_tilemap->position.y + highlightedTile.y * h;

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	srcRect = atlas_tile.sourceRectangles[selectedBrush];
	dstRect = ScreenRect(x0,y0,w,h);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, *(atlas_tile.texture));
	glBegin(GL_QUADS);
	{
		glTexCoord2d(srcRect.fLeft(), srcRect.fTop());	glVertex2f(dstRect.fLeft(), dstRect.fBottom());
		glTexCoord2d(srcRect.fRight(), srcRect.fTop());	glVertex2f(dstRect.fRight(), dstRect.fBottom());
		glTexCoord2d(srcRect.fRight(), srcRect.fBottom());	glVertex2f(dstRect.fRight(),  dstRect.fTop());
		glTexCoord2d(srcRect.fLeft(), srcRect.fBottom());	glVertex2f(dstRect.fLeft(),  dstRect.fTop());
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//image map
	if(blayer01loaded)
	{
		glColor4f(0.0f, 0.5f, 1.0f, 1.0f);
		x0 = pselected_tilemap->position.x + selectedTile.x * w;
		y0 = pselected_tilemap->position.y + selectedTile.y * h;

		R_DrawHorizontalLine(x0 - lw, y0, w + 2*lw, lw);
		R_DrawVerticalLine(x0, y0 , h, lw);
		R_DrawHorizontalLine(x0 - lw, y0 + h, w + 2*lw, lw);
		R_DrawVerticalLine(x0 + w, y0 + h, -h, lw);

		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		x0 = pselected_tilemap->position.x + highlightedTile.x * w;
		y0 = pselected_tilemap->position.y + highlightedTile.y * h;

		R_DrawHorizontalLine(x0 - lw, y0, w + 2*lw, lw);
		R_DrawVerticalLine(x0, y0 , h, lw);
		R_DrawHorizontalLine(x0 - lw, y0 + h, w + 2*lw, lw);
		R_DrawVerticalLine(x0 + w, y0 + h, -h, lw);
	}

	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void R_setPostProcessingOrtho()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
}

void R_setScreenOrtho()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, client_rect.w, client_rect.h, 0, -1, 1);
}

void SceneOrtho()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, rt_scene.width,rt_scene.height, 0,-1.0f, 1.0f);
}

void DrawCollisionMap()
{
	ScreenRect dstRect,srcRect;

	float transx = (int)(camera.position.x + camera.size.x/2) * 0.0f;
	float transy = (int)(camera.position.y + camera.size.y/2) * 0.0f;
	Vector2 trans = Vector2(transx, transy);
	Vector2 bl = camera.position - tilemap_back.position;
	Vector2 tr;
	tr.x = camera.position.x + camera.size.x - tilemap_back.position.x;
	tr.y = camera.position.y + camera.size.y - tilemap_back.position.y;
	Point BottomLeft = worldToTile(bl - trans, tilemap_back.w_tileSize);
	Point TopRight = worldToTile(tr - trans, tilemap_back.w_tileSize);

	Point min;
	Point max;
	min.x = max(0, BottomLeft.x - 1);
	max.x = min(TopRight.x + 1, tilemap_back.t_mapSize.x);
	min.y = max(0, BottomLeft.y - 1);            
	max.y = min(TopRight.y + 1, tilemap_back.t_mapSize.y);

	tilemap_back.Min = min;
	tilemap_back.Max = max;

	dstRect.w = tilemap_back.w_tileSize.x;
	dstRect.h = tilemap_back.w_tileSize.y;

	glBindTexture(GL_TEXTURE_2D, *(atlas_collision.texture));
	glPushMatrix();
	glTranslatef(transx, transy, 0.0f);

	for (int y = min.y; y < max.y; y++)
	{
		for (int x = min.x; x < max.x; x++)
		{
			if (tilemap_back.collisionLayer[x][y].tileIndex < 0 )
				continue;
			dstRect.x = tilemap_back.position.x + x * tilemap_back.w_tileSize.x;
			dstRect.y = tilemap_back.position.y + y * tilemap_back.w_tileSize.y;
			srcRect = atlas_collision.sourceRectangles[tilemap_back.collisionLayer[x][y].tileIndex];
			glBegin(GL_QUADS);
			{
				glTexCoord2d(srcRect.fLeft(), srcRect.fTop());	glVertex2f(dstRect.fLeft(), dstRect.fBottom());
				glTexCoord2d(srcRect.fRight(), srcRect.fTop());	glVertex2f(dstRect.fRight(), dstRect.fBottom());
				glTexCoord2d(srcRect.fRight(), srcRect.fBottom());	glVertex2f(dstRect.fRight(),  dstRect.fTop());
				glTexCoord2d(srcRect.fLeft(), srcRect.fBottom());	glVertex2f(dstRect.fLeft(),  dstRect.fTop());
			}
			glEnd();
		}
	}

	glPopMatrix();
}

void R_draw(HDC hDC)
{
	glGetUniformLocation(NULL,NULL);
	rt_back.Clear(0.2941f, 0.2941f, 0.2941f, 1.0f);
	rt_scene.Clear(0.3921f, 0.3921f, 0.3921f, 1.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, rt_scene.fbo);
	glViewport(0, 0, rt_scene.width, rt_scene.height);
	camera.setOrthoMatrix();

	if(bdrawbglayer01)
		tilemap_back.draw(&camera);
	if(bdrawbglayer02)
		tilemap_front.draw(&camera);
	if(bdrawbglayer03 && tilemap_back.has_c_Map)
		DrawCollisionMap();

	R_drawWorldDebug();

	glViewport(0, 0, client_rect.w, client_rect.h);
	R_setScreenOrtho();
	s_srcRect = ScreenRect(lmenuw/client_rect.w,0,1.0 - ((lmenuw + rmenuw)/client_rect.w),1);
	s_dstRect = ScreenRect(lmenuw, 0, client_rect.w - (lmenuw + rmenuw), client_rect.h);
	DrawQuadDS(rt_back, rt_scene,&s_dstRect,&s_srcRect, NULL);

	glViewport(0, 0, client_rect.w, client_rect.h);
	glBindFramebuffer(GL_FRAMEBUFFER, debug_rendertarget.fbo);
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	R_setScreenOrtho();

	DrawLeftPanel();
	DrawRightPanel();	
	DrawClientBorder();

	R_setPostProcessingOrtho();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	DrawFullscreenQuad(rt_back, debug_rendertarget, NULL);

	glFlush();
	bdoublebuffer ? SwapBuffers( hDC ) : glDrawBuffer(GL_FRONT);
}

void R_resize(int w, int h)
{
	client_rect.w = (float) w;
	client_rect.h = (float) h;
	aspect_ratio = client_rect.w / client_rect.h;
	camera.aspect = aspect_ratio;
	camera.size.y = camera.size.x / aspect_ratio;

	lmenuRect = ScreenRect(0,0,lmenuw,client_rect.h);
	rmenuRect = ScreenRect(client_rect.w - rmenuw,0,rmenuw,client_rect.h);

	debug_rendertarget.Resize(client_rect.w, client_rect.h);
	rt_temp0.Resize(client_rect.w, client_rect.h);
	rt_temp1.Resize(client_rect.w, client_rect.h);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_ACTIVATE:
		{
			KillTimer(hWnd, 0);
			if (!HIWORD(wParam))
				window.isActive = true;
			else
				window.isActive = false;
		}
		return 0;

	case WM_CREATE:
		{
			RAWINPUTDEVICE rid;

			rid.usUsagePage = 0x01;
			rid.usUsage     = 0x05;
			rid.dwFlags     = 0x00;
			rid.hwndTarget  = hWnd;

			if(!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE)))
				return -1;
		}
		return 0;

	case WM_INPUT:
		{
			PRAWINPUT pRawInput;
			UINT      bufferSize;
			HANDLE    hHeap;

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));

			hHeap     = GetProcessHeap();
			pRawInput = (PRAWINPUT)HeapAlloc(hHeap, 0, bufferSize);
			if(!pRawInput)
				return 0;

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, pRawInput, &bufferSize, sizeof(RAWINPUTHEADER));

			if(pRawInput->header.dwType == RIM_TYPEHID) 
			{
				ReadGamePadInput(pRawInput,input.gamepad);
			}

			HeapFree(hHeap, 0, pRawInput);
		}
		return 0;

	case WM_MOUSEWHEEL:
		{
			short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			if(zDelta > 0)
				selectedBrush++;
			else
				selectedBrush--;
			selectedBrush = CLAMP(selectedBrush,0,atlas_tile.totalFrames - 1);
			return 0;
		}

	case WM_MOUSEMOVE:
		POINTS point;
		point = MAKEPOINTS(lParam); 
		input.mouse.lX = point.x; 
		input.mouse.lY = point.y; 
		if(bdraggingworld)
		{
			float rx = camera.size.x / client_rect.w;
			float ry = camera.size.y / client_rect.h;
			camera.position += Vector2(-((float)input.mouse.lX - rclick.x) * rx , ((float)input.mouse.lY - rclick.y) * ry);
			rclick = Point(input.mouse.lX, input.mouse.lY);
		}
		else if(bdraggingrmenu)
		{
			float ry = rmenuRect.h / client_rect.h;
			rmenuyoffset += ((float)input.mouse.lY - rclick.y) * ry;
			int min = rmenuRect.h-(ceilf(((float)atlas_tile.totalFrames/tilesperrow))+4)*32.0f;
			rmenuyoffset = CLAMP(rmenuyoffset,min,0.0f);
			rclick = Point(input.mouse.lX, input.mouse.lY);
		}
		return 0;

	case WM_LBUTTONDOWN:
		input.mouse.bButtons[MLBUTTON] = true;
		return 0;
	case WM_LBUTTONUP:
		input.mouse.bButtons[MLBUTTON] = false;
		return 0;

	case WM_MBUTTONDOWN:
		input.mouse.bButtons[MMBUTTON] = true;
		return 0;
	case WM_MBUTTONUP:
		input.mouse.bButtons[MMBUTTON] = false;
		return 0;

	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
		rclick = Point(input.mouse.lX,input.mouse.lY);
		if(PointinRect(rclick,s_dstRect))
			bdraggingworld = true;
		else if(PointinRect(rclick,rmenuRect))
			bdraggingrmenu = true;
		input.mouse.bButtons[MRBUTTON] = true;
		return 0;
	case WM_RBUTTONUP:
		ReleaseCapture(); 
		bdraggingworld = false;
		bdraggingrmenu = false;
		input.mouse.bButtons[MRBUTTON] = false;
		return 0;

	case WM_SIZE:
		R_resize( LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;

	case WM_DESTROY:
		return 0;

	case WM_KEYDOWN:
		input.keyboard.bKeys[(unsigned char)wParam] = true;
		return 0;

	case WM_KEYUP:
		input.keyboard.bKeys[(unsigned char)wParam] = false;
		return 0;

	case WM_NCLBUTTONDOWN:
		last_movesize_time = T_GetCurrentTime();
		movesize_time = last_movesize_time - last_game_time;
		SetTimer(hWnd,NULL,1,NULL);
		return DefWindowProc( hWnd, message, wParam, lParam );

	case WM_NCLBUTTONUP:
		KillTimer(hWnd, 0);
		return 0;

	case WM_TIMER:
		{
			double current_time = T_GetCurrentTime();
			movesize_time += current_time - last_movesize_time;
			last_movesize_time = current_time;
		}
		return 0;

	case WM_EXITSIZEMOVE:
		KillTimer(hWnd, 0);
		return 0;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case ID_RED:
				{

				} 
				break;

			case ID_GREEN:
				{

				} 
				break;

			case ID_BLUE:
				{

				} 
				break;

			case ID_EXIT:
				{
					PostQuitMessage( 0 );
				} 
				break;
			case ID_FILE_OPEN_T1:
				{
					D_OpenTileMapDialog(hWnd,1);
				} 
				break;
			case ID_FILE_SAVE_T1:
				{
					D_SaveTileMapDialog(hWnd,1);
				} 
				break;
			case ID_FILE_OPEN_T2:
				{
					D_OpenTileMapDialog(hWnd,2);
				} 
				break;
			case ID_FILE_SAVE_T2:
				{
					D_SaveTileMapDialog(hWnd,2);
				} 
				break;
			case ID_FILE_NEW:
				{
					DialogBox(window.hInstance, MAKEINTRESOURCE(IDD_DLGFIRST),
						hWnd, reinterpret_cast<DLGPROC>(P_DlgProc));
				} 
				break;				
			}
		} 
		return 0;

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );

	}
}

LRESULT CALLBACK P_DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_INITDIALOG:
		blayer01loaded = false;
		blayer02loaded = false;
		SendMessage(hWnd, DM_SETDEFID, (WPARAM)ID_OK, 0);
		CheckDlgButton(hWnd, ID_CHECK_BC, BST_CHECKED);
		return TRUE;

	case WM_COMMAND:
		switch(wParam)
		{
		case ID_OK:
			{
				bool chk = SendDlgItemMessage(hWnd, ID_CHECK_BC, BM_GETCHECK, 0, 0)?true:false;
				LPSTR lpstrmapw,lpstrmaph,lpstrmapx,lpstrmapy,lpstrmapd,lpstrtilew,lpstrtileh;
				lpstrmapw = (LPSTR)GlobalAlloc(GPTR, 6 + 1);
				lpstrmaph = (LPSTR)GlobalAlloc(GPTR, 6 + 1);
				lpstrmapx = (LPSTR)GlobalAlloc(GPTR, 6 + 1);
				lpstrmapy = (LPSTR)GlobalAlloc(GPTR, 6 + 1);
				lpstrmapd = (LPSTR)GlobalAlloc(GPTR, 6 + 1);
				lpstrtilew = (LPSTR)GlobalAlloc(GPTR, 6 + 1);
				lpstrtileh = (LPSTR)GlobalAlloc(GPTR, 6 + 1);
				GetDlgItemText(hWnd,ID_EDIT_MAPX,lpstrmapx,6);
				GetDlgItemText(hWnd,ID_EDIT_MAPY,lpstrmapy,6);
				GetDlgItemText(hWnd,ID_EDIT_DEPTH,lpstrmapd,6);
				GetDlgItemText(hWnd,ID_EDIT_MAPW,lpstrmapw,6);
				GetDlgItemText(hWnd,ID_EDIT_MAPH,lpstrmaph,6);	
				GetDlgItemText(hWnd,ID_EDIT_TILEW,lpstrtilew,6);
				GetDlgItemText(hWnd,ID_EDIT_TILEH,lpstrtileh,6);	
				int mx,my,mw,mh,tw,th;
				float md;
				mx = _ttoi(lpstrmapx);
				my = _ttoi(lpstrmapy);
				md = _ttof(lpstrmapd);
				mw = _ttoi(lpstrmapw);
				mh = _ttoi(lpstrmaph);
				tw = _ttoi(lpstrtilew);
				th = _ttoi(lpstrtileh);
				blayer01loaded = true;
				tilemap_back.initialize( mx,my,mw,mh,tw,th,(md/100.0f), &atlas_tile, chk); 
				tilemap_front.initialize( mx,my,mw,mh,tw,th,(md/100.0f), &atlas_tile, false); 
				if(chk)
				{
					hascollisionmap = true;
					blayer02loaded = true;
				}
				else
					hascollisionmap = false;
				bdrawbglayer02 = chk;
			}
			EndDialog(hWnd, 0);
			return TRUE;
		case ID_CANCEL:
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

void P_run()
{
	if(window.isActive)
	{
		double elapsed_time;
		double current_time = T_GetCurrentTime();
		if(movesize_time < 0.05f)
			movesize_time = 0.0f;
		elapsed_time = (current_time - last_game_time - movesize_time) + remaining_game_time ;
		movesize_time = 0.0f;
		while (elapsed_time >= S_PER_GAME_UPDATE*fps_mult)
		{
			G_update(elapsed_time);
			elapsed_time -= S_PER_GAME_UPDATE*fps_mult;
		}
		remaining_game_time = elapsed_time;
		last_game_time = current_time;

		P_computeFPS();
		R_draw(window.hDC);

		if(btogglefullscreen || bmodechange)
		{
			last_movesize_time = T_GetCurrentTime();
			if(btogglefullscreen)
			{
				P_cleanup();
				window.createGLWindow(true);
				btogglefullscreen = false;
				GL_initializeGLObjects();
			}
			else
			{
				window.createGLWindow(false);
				bmodechange = false;
			}
			movesize_time = T_GetCurrentTime() - last_movesize_time;
		}
	}
}

void D_OpenTileMapDialog(HWND hwnd, int layer) 
{
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.hwndOwner = hwnd;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("Tile Map files(*.lvl)\0*.lvl\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrInitialDir = TEXT("levels");
	ofn.lpstrFileTitle = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if(GetOpenFileName(&ofn))
	{
		if(layer == 1)
		{
			blayer01loaded = true;
			tilemap_back.load(ofn.lpstrFile, &atlas_tile);
		}
		else
		{
			blayer02loaded = true;
			tilemap_front.load(ofn.lpstrFile, &atlas_tile);
		}
	}
}

void D_SaveTileMapDialog(HWND hwnd, int layer) 
{
	OPENFILENAME ofn;
	TCHAR   szFile[MAX_PATH] = TEXT("\0");

	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize   = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("Tile Map files(*.lvl)\0*.lvl\0");
	ofn.lpstrTitle = TEXT("Save File As");
	ofn.lpstrInitialDir = TEXT("levels");
	ofn.lpstrDefExt = TEXT("lvl");
	ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR; 

	if(GetSaveFileName(&ofn))
	{
		if(layer == 1)
		{
			tilemap_back.save(ofn.lpstrFile);
		}
		else if(layer == 2)
		{
			tilemap_front.save(ofn.lpstrFile);
		}
	}
}

