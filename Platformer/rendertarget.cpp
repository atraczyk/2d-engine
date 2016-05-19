//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15)
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	frame buffer object encapsulation and rendering funcs
//-----------------------------------------------------------------------------

#include "rendertarget.h"

void RenderTarget::Create(int w, int h, bool hasdepth)
{
    hasdepthbuffer = hasdepth;
    width = w;
    height = h;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &fbo_texture);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (hasdepthbuffer)
    {
        glGenRenderbuffers(1, &rbo_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
    if (hasdepthbuffer)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::Resize(int w, int h)
{
    width = w;
    height = h;
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (hasdepthbuffer)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
}

void RenderTarget::Clear(float r, float g, float b, float a)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderTarget::Delete()
{
    glDeleteRenderbuffers(1, &rbo_depth);
    glDeleteTextures(1, &fbo_texture);
    glDeleteFramebuffers(1, &fbo);
}

void DrawFullscreenQuad(RenderTarget dst, RenderTarget src, unsigned int shader)
{
    DrawFullscreenQuadG(dst.fbo, src.fbo_texture, shader);
}

void DrawFullscreenQuad(RenderTarget dst, unsigned int shader)
{
    DrawFullscreenQuadG(dst.fbo, NULL, shader);
}

void DrawFullscreenQuadG(unsigned int framebuffer, unsigned int texture, unsigned int shader)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glUseProgramObjectARB(shader);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);	glVertex2f(-1.0f, -1.0f);
    glTexCoord2d(1.0, 0.0);	glVertex2f(1.0f, -1.0f);
    glTexCoord2d(1.0, 1.0);	glVertex2f(1.0f, 1.0f);
    glTexCoord2d(0.0, 1.0);	glVertex2f(-1.0f, 1.0f);
    glEnd();
}

void DrawFullscreenQuadS(RenderTarget dst, RenderTarget src, ScreenRect* psrcRect, unsigned int shader)
{
    glBindFramebuffer(GL_FRAMEBUFFER, dst.fbo);
    glUseProgramObjectARB(shader);
    glBindTexture(GL_TEXTURE_2D, src.fbo_texture);
    glBegin(GL_QUADS);
    glTexCoord2d(psrcRect->fLeft(), psrcRect->fTop());	glVertex2f(-1.0f, -1.0f);
    glTexCoord2d(psrcRect->fRight(), psrcRect->fTop());	glVertex2f(1.0f, -1.0f);
    glTexCoord2d(psrcRect->fRight(), psrcRect->fBottom());	glVertex2f(1.0f, 1.0f);
    glTexCoord2d(psrcRect->fLeft(), psrcRect->fBottom());	glVertex2f(-1.0f, 1.0f);
    glEnd();
}

void DrawQuadD(RenderTarget dst, RenderTarget src, ScreenRect* pdstRect, unsigned int shader)
{
    glBindFramebuffer(GL_FRAMEBUFFER, dst.fbo);
    glUseProgramObjectARB(shader);
    glBindTexture(GL_TEXTURE_2D, src.fbo_texture);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);	glVertex2f(pdstRect->fLeft(), pdstRect->fTop());
    glTexCoord2d(1.0, 0.0);	glVertex2f(pdstRect->fRight(), pdstRect->fTop());
    glTexCoord2d(1.0, 1.0);	glVertex2f(pdstRect->fRight(), pdstRect->fBottom());
    glTexCoord2d(0.0, 1.0);	glVertex2f(pdstRect->fLeft(), pdstRect->fBottom());
    glEnd();
}

void DrawQuad(RenderTarget dst, unsigned int texture, ScreenRect* psrcRect, ScreenRect* pdstRect, unsigned int shader)
{
    glBindFramebuffer(GL_FRAMEBUFFER, dst.fbo);
    glUseProgramObjectARB(shader);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
    glTexCoord2d(psrcRect->fLeft(), psrcRect->fBottom());	glVertex2f(pdstRect->fLeft(), pdstRect->fTop());
    glTexCoord2d(psrcRect->fRight(), psrcRect->fBottom());	glVertex2f(pdstRect->fRight(), pdstRect->fTop());
    glTexCoord2d(psrcRect->fRight(), psrcRect->fTop());	glVertex2f(pdstRect->fRight(), pdstRect->fBottom());
    glTexCoord2d(psrcRect->fLeft(), psrcRect->fTop());	glVertex2f(pdstRect->fLeft(), pdstRect->fBottom());
    glEnd();
}