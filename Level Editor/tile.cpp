//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	le tilemaps and tiles
//-----------------------------------------------------------------------------

#include "tile.h"
#include "utils.h"
#include "collisions.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>

#pragma warning(disable: 4244)

ScreenRect dstRect;
ScreenRect srcRect;

#define NUM_COLLISIONTILETYPES	10
static const unsigned short entry_type[NUM_COLLISIONTILETYPES] = 
{
	CT_NONE,					//0
	CT_SQUARE,					//1
	CT_TOP,						//2
	CT_LEFT_SLOPE | CT_0_TO_7,	//3
	CT_LEFT_SLOPE | CT_8_TO_15,	//4
	CT_RIGHT_SLOPE | CT_0_TO_7,	//5
	CT_RIGHT_SLOPE | CT_8_TO_15,//6
	CT_LEFT_SLOPE | CT_0_TO_15,	//7
	CT_RIGHT_SLOPE | CT_0_TO_15,//8
	CT_TOP | CT_SPECIAL			//9
};

#define NUM_TILEANIMATIONS		16
TileAnimation tileAnimations[NUM_TILEANIMATIONS];

using namespace std;

void TileMap::save(const char* filename)
{
	ofstream file(filename);
	stringstream ss;
	if ( file.is_open() ) 
	{
		file << has_c_Map << " " << position.x << " " << position.y << " " << w_tileSize.x << " " << w_tileSize.y << " " << w_depth << endl;
		for (unsigned __int16 x = 0; x < imageLayer.size(); x++)
		{
			for (unsigned __int16 y = 0; y < imageLayer[0].size(); y++)
			{
				if(imageLayer[x][y].atileIndex != 0)
					file << imageLayer[x][y].atileIndex << " ";
				else
					file << imageLayer[x][y].tileIndex << " ";
			}
			if(x == imageLayer.size() - 1)
				file << -2;
			file << endl;
		}
		if(has_c_Map)
		{
			for (unsigned __int16 x = 0; x < collisionLayer.size(); x++)
			{
				for (unsigned __int16 y = 0; y < collisionLayer[0].size(); y++)
				{
					file << collisionLayer[x][y].tileIndex << " ";
				}
				file << endl;
			}
		}
		file.close();
	}
}

void TileMap::load(const char* filename, TileAtlas* ptexatlas)
{
	has_c_Map = false;
	atlas = ptexatlas;

	imageLayer.clear();
	collisionLayer.clear();
	animatedTiles.clear();

	string line;
	ifstream file(filename);
	int index, col = 0, row = 0;
	bool done  = false;

	if ( file.is_open() ) 
	{
		getline(file, line);
		if (line.compare( 0, 3, "\xEF\xBB\xBF" ) == 0)
			line.erase( 0, 3 );
		istringstream ssf(line);
		ssf >> has_c_Map >> position.x >> position.y >> w_tileSize.x >> w_tileSize.y >> w_depth;
		
		while( !done)
		{
			getline(file, line);
			col = 0;
			imageLayer.push_back(vector<ITile>());
			istringstream ss(line);
			while(ss >> index)
			{
				if(index ==  -2)
				{
					done = true;
					continue;
				}
				bool animated = isAnimatedTile(index)?true:false;
				if(animated)
					animatedTiles.push_back(AnimatedTile(Point(row,col),tileAnimations[index-240]));
				imageLayer.back().push_back(ITile(index,animated?index:0,NULL));
				col++;
			}
			row++;
		}
		t_mapSize = Point(imageLayer.size(),imageLayer[0].size());

		if(has_c_Map)
		{
			done = false;
			while(getline(file, line) && !done)
			{
				collisionLayer.push_back(vector<CTile>());
				istringstream ss(line);
				while(ss >> index)
				{
					if(index ==  -2)
					{
						done = true;
						continue;
					}
					collisionLayer.back().push_back(CTile(index,(index != 0),entry_type[index],NULL));
				}
			}
			t_cmapSize = Point(collisionLayer.size(),collisionLayer[0].size());
		}
		file.close();
	}
}

void TileMap::initialize(float x,float y, int w, int h,float tilewidth,float tileheight,
				   float depth,TileAtlas* ptexatlas, bool has_c_map)
{
	position = Vector2(x, y);
	w_tileSize = Vector2(tilewidth, tileheight);
	w_depth = depth;
	has_c_Map = has_c_map;
	atlas = ptexatlas;

	imageLayer.clear();
	collisionLayer.clear();
	animatedTiles.clear();

	t_mapSize = Point(w, h);
	if(has_c_Map)
		t_cmapSize = Point(w, h);
	for(int col = 0; col <  w; col++)
	{
		imageLayer.push_back(vector<ITile>());
		if(has_c_Map)
			collisionLayer.push_back(vector<CTile>());
		for(int row = 0; row <  h; row++)
		{
			imageLayer.back().push_back(ITile(-1,false,NULL));
			if(has_c_Map)
				collisionLayer.back().push_back(CTile(0,false,0,NULL));
		}
	}
}

void TileMap::update()
{
	for(unsigned int i = 0; i < animatedTiles.size(); i++)
	{
		Point p = animatedTiles[i].position;
		animatedTiles[i].animation.update();
		imageLayer[p.x][p.y].tileIndex = animatedTiles[i].animation.GetCurrentIndex();
	}
}

void TileMap::draw(Camera* camera)
{	
	float transx = (int)(camera->position.x + camera->size.x/2) * 0.0f;
	float transy = (int)(camera->position.y + camera->size.y/2) * 0.0f;
	Vector2 trans = Vector2(transx, transy);
	Vector2 bl = camera->position - position;
	Vector2 tr;
	tr.x = camera->position.x + camera->size.x - position.x;
	tr.y = camera->position.y + camera->size.y - position.y;
	Point BottomLeft = worldToTile(bl - trans, w_tileSize);
	Point TopRight = worldToTile(tr - trans, w_tileSize);

	Point min;
	Point max;
	min.x = max(0, BottomLeft.x - 1);
	max.x = min(TopRight.x + 1, t_mapSize.x);
	min.y = max(0, BottomLeft.y - 1);            
	max.y = min(TopRight.y + 1, t_mapSize.y);

	Min = min;
	Max = max;

	dstRect.w = w_tileSize.x;
	dstRect.h = w_tileSize.y;

	if(atlas)
		glBindTexture(GL_TEXTURE_2D, *(atlas->texture));
	glPushMatrix();
	glTranslatef(transx, transy, 0.0f);

	for (int y = min.y; y < max.y; y++)
	{
		for (int x = min.x; x < max.x; x++)
		{
			if (imageLayer[x][y].tileIndex < 0 )
				continue;
			dstRect.x = (int)position.x + x * w_tileSize.x;
			dstRect.y = (int)position.y + y * w_tileSize.y;
			DrawTile( camera, dstRect, imageLayer[x][y].tileIndex);
		}
	}

	glPopMatrix();
}

void TileMap::DrawTile(Camera* camera, ScreenRect dstRect, int index)
{
	srcRect = atlas->sourceRectangles[index];

	glBegin(GL_QUADS);
	{
		glTexCoord2d(srcRect.fLeft(), srcRect.fTop());	glVertex2f(dstRect.fLeft(), dstRect.fBottom());
		glTexCoord2d(srcRect.fRight(), srcRect.fTop());	glVertex2f(dstRect.fRight(), dstRect.fBottom());
		glTexCoord2d(srcRect.fRight(), srcRect.fBottom());	glVertex2f(dstRect.fRight(),  dstRect.fTop());
		glTexCoord2d(srcRect.fLeft(), srcRect.fBottom());	glVertex2f(dstRect.fLeft(),  dstRect.fTop());
	}
	glEnd();
}

bool isInTileMap(int x, int y, TileMap tilemap_back)
{
	return (isInRange(x, 0, tilemap_back.t_mapSize.x - 1) && isInRange(y, 0, tilemap_back.t_mapSize.y - 1));
}

bool isAnimatedTile(int index)
{
	return (index > 239) && (index < 256);
}

void initializeTileAnimations()
{
	tileAnimations[0].addFrame(224, 9);
	tileAnimations[0].addFrame(225, 9);
	tileAnimations[0].addFrame(226, 9);
	tileAnimations[0].addFrame(227, 9);

	tileAnimations[1].addFrame(232, 9);
	tileAnimations[1].addFrame(233, 9);
	tileAnimations[1].addFrame(234, 9);
	tileAnimations[1].addFrame(235, 9);

	tileAnimations[2].addFrame(228, 30);
	tileAnimations[2].addFrame(229, 30);
	tileAnimations[2].addFrame(230, 30);
	tileAnimations[2].addFrame(231, 30);

	tileAnimations[3].addFrame(236, 30);

	tileAnimations[4].addFrame(0, 30);
	tileAnimations[4].addFrame(1, 30);
	tileAnimations[4].addFrame(2, 30);
	tileAnimations[4].addFrame(1, 30);

	tileAnimations[5].addFrame(208, 6);
	tileAnimations[5].addFrame(209, 6);
	tileAnimations[5].addFrame(210, 6);
	tileAnimations[5].addFrame(211, 6);

	tileAnimations[6].addFrame(212, 15);
	tileAnimations[6].addFrame(213, 15);
	tileAnimations[6].addFrame(214, 15);
	tileAnimations[6].addFrame(215, 15);

	tileAnimations[7].addFrame(0, 30);
	tileAnimations[8].addFrame(0, 30);
	tileAnimations[9].addFrame(0, 30);
	tileAnimations[10].addFrame(0, 30);
	tileAnimations[11].addFrame(0, 30);
	tileAnimations[12].addFrame(0, 30);
	tileAnimations[12].addFrame(0, 30);
	tileAnimations[13].addFrame(0, 30);
	tileAnimations[14].addFrame(0, 30);
	tileAnimations[15].addFrame(0, 30);
}