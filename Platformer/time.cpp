//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	time handling
//-----------------------------------------------------------------------------

#include "time.h"

Time::Time(void)
{
}

Time::~Time(void)
{
}

void Time::initialize()
{
	fpsFrame			= 0;
	lastfpsTime			= 0;
	fps					= 0.0f;
	fpsPollingTime		= 250.0f;
	currentFrame		= 0;
	pauseTime			= 0.0f;
	moveSizeTime		= 0.0f;
	lastMoveSizeTime	= 0.0f;
	startTime			= getCurrentTime();
	gameTime			= 0.0f;
	remaining			= 0.0f;
	lastGameTime		= 0.0f;
	frameMultiplier		= 1.0;
}

double Time::getCurrentTime() 
{
	static LARGE_INTEGER frequency;
	LARGE_INTEGER currentTime;

	if (frequency.QuadPart == 0) 
	{
		QueryPerformanceFrequency(&frequency);
	}
	QueryPerformanceCounter(&currentTime);

	return (double) currentTime.QuadPart / frequency.QuadPart;
}

int Time::getCurrentMilliseconds() 
{
	return (int)(1000 * getCurrentTime());
}

void Time::computeFPS()
{
	fpsFrame++;
	fpsTime= getCurrentMilliseconds();

	if (fpsTime - lastfpsTime > fpsPollingTime) 
	{
		fps = fpsFrame*1000.0f/(fpsTime-lastfpsTime);
		lastfpsTime = fpsTime;
		fpsFrame = 0;
	}
}

