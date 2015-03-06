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
	GLuint fbo;
	GLuint rbo_depth;
	GLuint fbo_texture;

	RenderTarget(){};
	~RenderTarget(){};

	void Create(int w, int h, bool hasdepth);
	void Clear(float r, float g, float b, float a);
	void Resize(int w, int h);
	void Delete();
};

void DrawFullscreenQuad(RenderTarget dst, RenderTarget src, GLhandleARB shader);
void DrawFullscreenQuad(RenderTarget dst, GLhandleARB shader);
void DrawFullscreenQuadG(GLuint framebuffer, GLuint texture, GLhandleARB shader);
void DrawFullscreenQuadS(RenderTarget dst, RenderTarget src, ScreenRect* psrcRect, GLhandleARB shader);
void DrawQuadDS(RenderTarget dst, RenderTarget src, ScreenRect* pdstRect, ScreenRect* psrcRect, GLhandleARB shader);
void DrawFullscreenQuad4C(RenderTarget dst, Vector4 c1, Vector4 c2, Vector4 c3, Vector4 c4);

#endif