//-----------------------------------------------------------------------------
//  2d platform game engine and level editor by Andreas Traczyk (2014-15)
//  http://andreastraczyk.com/  email: andreastraczyk@gmail.com
//
//  DESCRIPTION:    shared macros
//-----------------------------------------------------------------------------

#pragma once

#pragma warning(disable: 4996)
#pragma warning(disable: 4244)
#pragma warning(disable: 4018)
#pragma warning(disable: 4800)

#include <iostream>
#include <string>

//camera
#define ACCELERATION            0.75f
#define DECELERATION            0.75f
#define VELOCITY_MAX            10.0f

//entities
#define JUMPFRAMES              14

#define MAX_X_VELOCITY          8.0f
#define MAX_X_RUN_VELOCITY      12.0f
#define S_MAX_X_RUN_VELOCITY    10.0f //prevent stumbling down 0-15 slopes
#define MAX_Y_VELOCITY          20.0f
#define MAX_LIQUID_X_VELOCITY   6.0f
#define MAX_LIQUID_Y_VELOCITY   6.0f
#define MAX_SHELL_X_VELOCITY    10.0f
#define MAX_SHELL_Y_VELOCITY    24.0f

#define ENEMY1_X_SPEED          2.0f
#define ENEMY2_X_SPEED          10.0f

#define X_AIR_MULTIPLIER        0.75f
#define X_ACCELERATION          0.5f
#define X_MDECELERATION         1.0f
#define X_DECELERATION          0.5f
#define Y_ACCELERATION          8.0f
#define S_GRAVITY               10.0f
#define D_GRAVITY               8.0f

#define CANTHITPLAYERFRAMES     12

//sound
#define DEFAULT_MASTER_VOLUME   1.0f
#define DEFAULT_MUSIC_VOLUME    0.5f
#define DEFAULT_SFX_VOLUME      0.5f

//misc
#define GRAVITY                 -2.0f
#define S_PER_GAME_UPDATE       (1.0f / 60.0f)
#define S_PER_FRAME             (1.0f / 120.0f)
#define MS_PER_FRAME            (int)(S_PER_FRAME*1000.0f)
#define CLAMP(x,MIN,MAX)        min(max(MIN,x),MAX)
#define PI                      3.14159265359f
#define _16x9                   1.77777777778f
#define _L(x)                   [](){x}