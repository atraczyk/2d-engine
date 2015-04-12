//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	frame buffer object encapsulation and rendering funcs
//-----------------------------------------------------------------------------

#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include <windows.h>
#include <GL\glew.h>

#include "rectangle.h"
#include "vector.h"

class RenderTarget
{
public:
	int width;
	int height;
	bool hasdepthbuffer;
	unsigned int fbo;
	unsigned int rbo_depth;
	unsigned int fbo_texture;

	RenderTarget(){};
	~RenderTarget(){};

	void Create(int w, int h, bool hasdepth);
	void Clear(float r, float g, float b, float a);
	void Resize(int w, int h);
	void Delete();
};

void DrawFullscreenQuad(RenderTarget dst, RenderTarget src, unsigned int shader);
void DrawFullscreenQuad(RenderTarget dst, unsigned int shader);
void DrawFullscreenQuadG(unsigned int framebuffer, unsigned int texture, unsigned int shader);
void DrawFullscreenQuadS(RenderTarget dst, RenderTarget src, ScreenRect* psrcRect, unsigned int shader);
void DrawQuadD(RenderTarget dst, RenderTarget src, ScreenRect* pdstRect, unsigned int shader);
void DrawQuad(RenderTarget dst, unsigned int texture, ScreenRect* psrcRect, ScreenRect* pdstRect, unsigned int shader);

#endif