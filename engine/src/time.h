//-----------------------------------------------------------------------------
//  2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//  http://andreastraczyk.com/  email: andreastraczyk@gmail.com
//
//  DESCRIPTION:    time handling
//-----------------------------------------------------------------------------

#pragma once

#include <windows.h>

class Time
{
public:
    int                     fpsFrame;
    int                     fpsTime;
    int                     lastfpsTime;
    int                     currentFrame;

    float                   fps;
    float                   fpsPollingTime;
    float                   frameMultiplier;

    double                  pauseTime;
    double                  moveSizeTime;
    double                  lastMoveSizeTime;
    double                  startTime;
    double                  gameTime;
    double                  remaining;
    double                  lastGameTime;

    Time(void);
    ~Time(void);

    void                    initialize();
    double                  getCurrentTime();
    int                     getCurrentMilliseconds();
    void                    computeFPS();
};
