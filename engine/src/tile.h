//-----------------------------------------------------------------------------
//  2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//  http://andreastraczyk.com/  email: andreastraczyk@gmail.com
//
//  DESCRIPTION:    tilemaps, tiles, animated tiles, dynamic tiles
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
    int  tileIndex;
    int  atileIndex;
    void(*function)();
    float   bounce;
    int  value;

    ITile(int i = -1, int ai = 0, void(*f)() = NULL)
    {
        value = 5;
        bounce = 0.0f;
        tileIndex = i;
        atileIndex = ai;
        function = f;
    };

    ~ITile() {};
};

class CTile
{
public:
    int  tileIndex;
    bool isCollidable;
    USHORT collisionFlags;
    void(*function)();

    CTile(int index = 0, bool c = false, USHORT l = 0x0000, void(*f)() = NULL)
    {
        tileIndex = index;
        isCollidable = c;
        collisionFlags = l;
        function = f;
    };

    ~CTile() {};
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
        currentFrame = 0;
    };

    ~TileAnimation() {};

    int GetCurrentIndex()
    {
        return frames[currentAtlasFrame];
    };

    void update()
    {
        currentFrame++;
        if (currentFrame >= delays[currentAtlasFrame])
        {
            currentAtlasFrame++;
            if (currentAtlasFrame > frames.size() - 1)
                currentAtlasFrame = static_cast<UCHAR>(looped ? 0 : frames.size()) - 1;
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
    TileAnimation   animation;
    Point           position;

    AnimatedTile(Point p, TileAnimation anim)
    {
        position = p;
        animation = anim;
    };
    ~AnimatedTile() {};
};

class TileMap
{
public:
    bool    has_c_Map;

    vector<vector<ITile> > imageLayer;
    vector<vector<CTile> > collisionLayer;

    Vector2 position;
    Vector2 velocity;
    Vector2 w_tileSize;
    float   w_depth;
    Point   t_mapSize;
    Point   t_cmapSize;
    Point   Min, Max;

    TileAtlas* atlas;
    vector<AnimatedTile> animatedTiles;

    TileMap() { velocity = Vector2(0, 0); };
    ~TileMap() {};

    void save(const char* filename);
    void load(const char* filename, TileAtlas* ptexatlas, Camera* camera);
    void initialize(float x = 0, float y = 0, int w = 0, int h = 0, float tilewidth = 0, float tileheight = 0,
        float depth = 0, TileAtlas* ptexatlas = NULL, bool has_c_map = false, int itype = -1, int ctype = 0);
    void adjustPosition(Camera* pcamera);
    void update();
    void draw(Camera* camera);
    void DrawTile(Camera* camera, ScreenRect dstRect, int index);
};

class DynamicTileMapList
{
public:
    unsigned int count;
    std::vector<TileMap>    tileMaps;
    std::vector<Vector2(*)(unsigned int)> movementfunctions;

    DynamicTileMapList() { count = 0; };
    ~DynamicTileMapList() {};

    void Add(TileMap tilemap, Vector2(*_function)(unsigned int))
    {
        count++;
        tileMaps.push_back(tilemap);
        movementfunctions.push_back(_function);
    };

    void update(unsigned int currentFrame)
    {
        for (unsigned int i = 0; i < count; i++)
        {
            tileMaps[i].velocity = movementfunctions[i](currentFrame);
            tileMaps[i].update();
        }
    };

    void Clear()
    {
        count = 0;
        tileMaps.clear();
        movementfunctions.clear();
    };
};

bool isAnimatedTile(int index);
void initializeTileAnimations();

#endif