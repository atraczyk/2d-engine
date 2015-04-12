//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	le tilemaps and tiles
//-----------------------------------------------------------------------------

#ifndef TILE_H
#define TILE_H

#include <vector>
#include <stdio.h>
#include "texture.h"
#include <Windows.h>
#include <GL/gl.h>
#include "vector.h"
#include "point.h"
#include "objects.h"
#include "camera.h"
#include "rectangle.h"

using namespace std;

class ITile
{
public:
	int	 tileIndex;
	int	 atileIndex;
	void (*function)();

	ITile(int i = -1,int ai = 0,void (*sp_fragment)() = NULL)
	{
		tileIndex = i;
		atileIndex = ai;
		function = sp_fragment;
	};

	~ITile(){};

};

class CTile
{
public:
	int	 tileIndex;
	bool isCollidable;
	USHORT collisionFlags;
	void (*function)();

	CTile(int index = 0, bool c = false,USHORT l = 0x0000,void (*sp_fragment)() = NULL)
	{
		tileIndex = index;
		isCollidable = c;
		collisionFlags = l;
		function = sp_fragment;
	};

	~CTile(){};

};

class TileAnimation
{
public:
	bool looped;
	unsigned int currentFrame;
	unsigned __int8 currentAtlasFrame; 
	std::vector<int> frames;
	std::vector<unsigned int> delays;

	TileAnimation()
	{
		looped = true;
		currentAtlasFrame = 0;
		currentFrame  = 0; 
	};

	~TileAnimation(){};

	int GetCurrentIndex()
	{
		return frames[currentAtlasFrame];
	};

	void update()
	{
		currentFrame++;
		if(currentFrame >= delays[currentAtlasFrame])
		{
			currentAtlasFrame++;
			if(currentAtlasFrame > frames.size()-1)
				currentAtlasFrame = looped?0:frames.size()-1;
			currentFrame = 0;
		}
	};

	void addFrame(int index, unsigned int delay)
	{
		frames.push_back(index);
		delays.push_back(delay);
	};

};

class AnimatedTile
{
public:
	TileAnimation	animation;
	Point			position;

	AnimatedTile(Point p,TileAnimation anim)
	{
		position = p;
		animation = anim;
	};
	~AnimatedTile(){};
};

class TileMap
{
public:
	bool	has_c_Map;

	vector<vector<ITile> > imageLayer;
	vector<vector<CTile> > collisionLayer;

	Vector2	position;
	Vector2	w_tileSize;
	float	w_depth;
	Point	t_mapSize;      
	Point	t_cmapSize;      
	Point	Min, Max;
	string  line1;

	TileAtlas* atlas;
	vector<AnimatedTile> animatedTiles;

	TileMap()
	{
		t_mapSize = Point(0,0);
		w_tileSize = Vector2(0,0);
		t_cmapSize = Point(0,0);
	};

	~TileMap(){};

	void save(const char* filename);
	void load(const char* filename ,TileAtlas* ptexatlas);
	void initialize(float x,float y, int w, int h,float tilewidth,float tileheight,
		float depth,TileAtlas* ptexatlas, bool has_c_map);
	void update();
	void draw(Camera* camera);
	void DrawTile(Camera* camera, ScreenRect dstRect, int index);
};

class DynamicTileMapList
{
public:
	unsigned int count;
	std::vector<TileMap>	tileMaps;
	std::vector<Vector2 (*)(unsigned int)> movementfunctions;
	std::vector<Vector2> velocities;

	DynamicTileMapList(){ count = 0;};
	~DynamicTileMapList(){};

	void Add(TileMap tilemap_back, Vector2 (*func)(unsigned int))
	{
		count++;
		tileMaps.push_back(tilemap_back);
		movementfunctions.push_back(func);
		velocities.push_back(Vector2(0,0));
	};

	void update(unsigned int currentFrame)
	{
		for(unsigned int i = 0;i<count;i++)
		{
			tileMaps[i].update();
			velocities[i] = movementfunctions[i](currentFrame);
			tileMaps[i].position += velocities[i];
		}
	};

	void Clear()
	{
		count = 0;
		tileMaps.clear();
		movementfunctions.clear();
		velocities.clear();
	};

};

bool isInTileMap(int x, int y, TileMap tilemap_back);
bool isAnimatedTile(int index);
void initializeTileAnimations();

#endif