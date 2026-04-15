//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15)
// GPL 	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	time handling
//-----------------------------------------------------------------------------

#include <SDL2/SDL.h>

#include "game_time.h"

GameTime::GameTime() {
  startTime = 0.0;
  lastTime = 0.0;
  currentTime = 0.0;
  gameTime = 0.0;
  pauseTime = 0.0;
  moveSizeTime = 0.0;
  lastMoveSizeTime = 0.0;
  lastGameTime = 0.0;
  remaining = 0.0;
  fps = 0.0;
  fpsTime = 0.0;
  lastfpsTime = 0.0;
  fpsPollingTime = 1000.0;
  fpsFrame = 0;
  currentFrame = 0;
  frameMultiplier = 1.0;
}

GameTime::~GameTime() {}

void GameTime::initialize() {
  startTime = getCurrentTime();
  lastTime = startTime;
  currentTime = startTime;
  gameTime = 0.0;
  pauseTime = 0.0;
  moveSizeTime = 0.0;
  lastMoveSizeTime = startTime;
  lastGameTime = startTime;
  remaining = 0.0;
  fps = 0.0;
  fpsTime = 0.0;
  lastfpsTime = 0.0;
  fpsPollingTime = 1000.0;
  fpsFrame = 0;
  currentFrame = 0;
  frameMultiplier = 1.0;
}

double GameTime::getCurrentTime() {
  const Uint64 frequency = SDL_GetPerformanceFrequency();
  const Uint64 counter = SDL_GetPerformanceCounter();
  return static_cast<double>(counter) / static_cast<double>(frequency);
}

double GameTime::getCurrentMilliseconds() { return getCurrentTime() * 1000.0; }

void GameTime::computeFPS() {
  fpsFrame++;
  fpsTime = getCurrentMilliseconds();

  if (fpsTime - lastfpsTime > fpsPollingTime) {
    fps = fpsFrame * 1000.0 / (fpsTime - lastfpsTime);
    lastfpsTime = fpsTime;
    fpsFrame = 0;
  }
}
