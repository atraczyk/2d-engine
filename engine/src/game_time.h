//-----------------------------------------------------------------------------
//  2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//  http://andreastraczyk.com/  email: andreastraczyk@gmail.com
//
//  DESCRIPTION:    time handling
//-----------------------------------------------------------------------------

#ifndef GAME_TIME_H
#define GAME_TIME_H

// GameTime class for handling game timing
class GameTime {
public:
  GameTime();
  ~GameTime();

  void initialize();
  double getCurrentTime();
  double getCurrentMilliseconds();
  void computeFPS();

  double startTime;
  double lastTime;
  double currentTime;
  double gameTime;
  double pauseTime;
  double moveSizeTime;
  double lastMoveSizeTime;
  double lastGameTime;
  double remaining;
  double fps;
  double fpsTime;
  double lastfpsTime;
  double fpsPollingTime;
  int fpsFrame;
  int currentFrame;
  double frameMultiplier;
};

#endif // GAME_TIME_H
