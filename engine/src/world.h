//-----------------------------------------------------------------------------
//  2d platform game engine and level editor by Andreas Traczyk (2014-15)
//  http://andreastraczyk.com/  email: andreastraczyk@gmail.com
//
//  DESCRIPTION:    containers.
//-----------------------------------------------------------------------------

#pragma once

#include "tile.h"
#include "entity.h"
#include "rectangle.h"

#define NUM_LAYERS 4

struct EnemySpawn
{
    Vector2 position;
    int     type;
    int     direction;
};

struct PlayerSpawn
{
    Vector2 position;
    int     type;
    int     direction;
};

struct DynamicObject
{
    Vector2 position;
    int     type;
    int     direction;
};

class Level
{
public:
    TileMap                 layer[NUM_LAYERS];
    DynamicTileMapList      dynTileMaps;
    std::vector<EnemySpawn> enemySpawns;
    float                   ambientLighting;
    PlayerSpawn             playerSpawn;
    int                     musicID;
    int                     reverb;

    void                    initializeLevel();
};

class World
{
public:
    Level                   level;
    WorldRect               constraints;
    std::vector<Enemy>      enemies;
    Vector2                 gravity;

    void                    loadLevel(const char* filename, Camera* camera);
    void                    loadTextureAtlas(const char* filename);
    void                    update();
};
