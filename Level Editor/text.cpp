//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	le text drawing with opengl
//-----------------------------------------------------------------------------

#include "text.h"

#pragma warning(disable:4244)

char w_spacing[256];
char s_spacing[256];

Font::Font()
{
}

Font::~Font()
{
}

void Font::initialize( int charsWide, int charsHigh, int charWidth, int charHeight,
				 float Scale, Camera* pCamera, ScreenRect* pScreen, TileAtlas* ptexatlas)
{
	atlas = ptexatlas;
	pcamera = pCamera;
	pscreen = pScreen;
	scaleValue = Scale;
	charsXYWH = ScreenRect(charsWide, charsHigh, charWidth, charHeight);
}

void Font::displayText(float x, float y, float w, float h, 
					   float r, float g, float b, const char *string) 
{
	float w_x = x;
	float w_y = y + h;
	int length = strlen( string);	

	float ds = w / 8.0f;

	glBindTexture(GL_TEXTURE_2D, *(atlas->texture));
	for( int i = 0; i < length; i++ ) 
	{
		glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
		BitmapCharacter(w_x, w_y+ds, w, -h, string[i]);
		BitmapCharacter(w_x, w_y-ds, w, -h, string[i]);
		BitmapCharacter(w_x+ds, w_y, w, -h, string[i]);
		BitmapCharacter(w_x-ds, w_y, w, -h, string[i]);
		glColor4f( 0.0f, 0.0f, 0.0f, 0.5f );
		BitmapCharacter(w_x-ds, w_y+ds, w, -h, string[i]);
		BitmapCharacter(w_x+ds, w_y-ds, w, -h, string[i]);
		BitmapCharacter(w_x+ds, w_y+ds, w, -h, string[i]);
		BitmapCharacter(w_x-ds, w_y-ds, w, -h, string[i]);
	
		glColor4f( r, g, b, 1.0f );
		BitmapCharacter(w_x, w_y, w, -h, string[i]);
		w_x +=  w + w_spacing[string[i]];
	}
}

void Font::displayScreenText(int x, int y, float r, float g, float b, const char *string) 
{
	float w = charsXYWH.w * scaleValue;
	float h = charsXYWH.h * scaleValue;

	int length = strlen( string);	

	glBindTexture(GL_TEXTURE_2D, *(atlas->texture));
	glColor4f( r, g, b, 1.0f );
	for( int i = 0; i < length; i++ ) 
	{
		BitmapCharacter(x, y - h, w, h, string[i]);
		x +=  w;
	}
}

void Font::displayScreenTextSpacing(int x, int y, float r, float g, float b, const char *string) 
{
	float w = charsXYWH.w * scaleValue;
	float h = charsXYWH.h * scaleValue;

	int length = strlen( string);	

	glBindTexture(GL_TEXTURE_2D, *(atlas->texture));
	glColor4f( r, g, b, 1.0f );
	for( int i = 0; i < length; i++ ) 
	{
		BitmapCharacter(x, y - h, w, h, string[i]);
		x +=  w + scaleValue*s_spacing[string[i]];
	}
}

void Font::BitmapCharacter(float x, float y, float w, float h, int character)
{
	WorldRect dstRect(x, y, w, h);
	ScreenRect* psrcRect;
	psrcRect = &atlas->sourceRectangles[character];
	glBegin(GL_QUADS);
	{
		glTexCoord2d(psrcRect->fLeft(), psrcRect->fTop());	glVertex2f(dstRect.iLeft(), dstRect.iBottom());
		glTexCoord2d(psrcRect->fRight(), psrcRect->fTop());	glVertex2f(dstRect.iRight(), dstRect.iBottom());
		glTexCoord2d(psrcRect->fRight(), psrcRect->fBottom());	glVertex2f(dstRect.iRight(),  dstRect.iTop());
		glTexCoord2d(psrcRect->fLeft(), psrcRect->fBottom());	glVertex2f(dstRect.iLeft(),  dstRect.iTop());
	}
	glEnd();
}

void InitSpacings()
{
	int i,length;
	char* string;
	for(i = 0;i<256;i++)
	{
		w_spacing[i] = -8;
		s_spacing[i] = -1;
	}
	string = "il.,";
	length = strlen(string);	
	for(i = 0; i < length; i++ ) 
	{
		w_spacing[string[i]] = -24;
		s_spacing[string[i]] = -3;
	}
	string = "ft";
	length = strlen(string);	
	for(i = 0; i < length; i++ ) 
	{
		w_spacing[string[i]] = -16;
		s_spacing[string[i]] = -2;
	}
}