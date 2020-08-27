//-----------------------------------------------------------------------------
//  2d platform game engine and level editor by Andreas Traczyk (2014-15)
//  http://andreastraczyk.com/  email: andreastraczyk@gmail.com
//
//  DESCRIPTION:    containers.
//-----------------------------------------------------------------------------

#include "world.h"
#include "game.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

extern Game game;

void Level::initializeLevel()
{
    enemySpawns.clear();
    dynTileMaps.Clear();
}

void World::loadLevel(const char* filename, Camera* camera)
{
    level.initializeLevel();
    enemies.clear();

    string line;
    ifstream file(filename);
    istringstream ssf;
    int done = 0;
    string layerfile[NUM_LAYERS];

    if (file.is_open())
    {
        getline(file, line);
        if (line.compare(0, 3, "\xEF\xBB\xBF") == 0)
            line.erase(0, 3);
        ssf = istringstream(line);

        ssf >> layerfile[0] >> layerfile[1] >> layerfile[2] >> layerfile[3];

        for (int i = 0; i < NUM_LAYERS; i++)
        {
            if (layerfile[i] != "null")
            {
                switch (i)
                {
                case 0: case 1:
                    level.layer[i].load(layerfile[i].c_str(), &game.atlases.bgs, camera);
                    break;
                case 2: case 3:
                    level.layer[i].load(layerfile[i].c_str(), &game.atlases.tiles, camera);
                    break;
                }
            }
            else
            {
                level.layer[i].initialize(0, 0, 0, 0, 0, 0, 0, NULL, 0);
            }
        }

        getline(file, line);
        ssf = istringstream(line);
        ssf >> level.ambientLighting >> level.musicID >> level.reverb;

        getline(file, line);
        ssf = istringstream(line);
        ssf >> level.playerSpawn.position.x
            >> level.playerSpawn.position.y
            >> level.playerSpawn.type
            >> level.playerSpawn.direction;

        //TODO portals/items

        EnemySpawn spawn;
        while (getline(file, line))
        {
            istringstream ss(line);
            ss >> spawn.type >> spawn.position.x >> spawn.position.y >> spawn.direction;
            if (spawn.type == -1)
                break;
            level.enemySpawns.push_back(spawn);
        }

        int t, w, h, mf;
        float x, y, tw, th;
        TileMap dobject;
        while (getline(file, line))
        {
            istringstream ss(line);
            ss >> mf >> t >> x >> y >> w >> h >> tw >> th;
            dobject.initialize(x, y, w, h, tw, th, 0.0f, &game.atlases.tiles, true, t, 1);
            switch (mf)
            {
            case 0:
                level.dynTileMaps.Add(dobject, [](unsigned int frame)
                {
                    float ftime = ((float)frame)*S_PER_GAME_UPDATE;
                    return Vector2((int)(2 * sgn(sinf((2 * PI / 2)*ftime))), (int)(-1 * sgn(sinf((2 * PI / 8)*ftime))));
                });
                break;
            case 1:
                level.dynTileMaps.Add(dobject, [](unsigned int frame)
                {
                    float ftime = ((float)frame)*S_PER_GAME_UPDATE;
                    return Vector2((int)2 * sgn(sinf((2 * PI / 40)*ftime)), (int)0.0f);
                });
                break;
            }
        }
        file.close();
    }

    constraints = WorldRect(level.layer[2].w_tileSize.x, 0,
        level.layer[2].t_mapSize.x * level.layer[2].w_tileSize.x - 2 * level.layer[2].w_tileSize.x,
        level.layer[2].t_mapSize.y * level.layer[2].w_tileSize.y);
}