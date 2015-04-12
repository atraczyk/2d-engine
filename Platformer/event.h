//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	event system (totally not finished) 
//-----------------------------------------------------------------------------

#ifndef EVENT_H
#define EVENT_H

#include <stdio.h>
#include "vector.h"

class Event
{
public:
	void (*function)();

	Event(void (*_function)() = NULL)
	{
		function = _function;
	};

	~Event(){};

	void Execute()
	{
		if (function != NULL)
			function();
	};
};

class TimeEventList
{
public:
	unsigned int frame;
	std::vector<Event> chainOfEvents;

	TimeEventList(unsigned int _frame = 0, void (*_function)() = NULL)
	{
		frame = _frame;
		chainOfEvents.push_back(Event(_function));
	};
	~TimeEventList(){};

	void AddEvent(unsigned int _frame = 0, void (*_function)() = NULL)
	{
		frame = _frame;
		chainOfEvents.push_back(Event(_function));
	};

	void Execute()
	{
		for(int i = 0; i < chainOfEvents.size(); i++)
			chainOfEvents[i].Execute();
	};
};

class PositionEventList
{
public:
	Vector2 position;
	std::vector<Event> chainOfEvents;

	PositionEventList(Vector2 pos = Vector2(0,0), void (*_function)() = NULL)
	{
		position = pos;
		chainOfEvents.push_back(Event(_function));
	};
	~PositionEventList(){};

	void AddEvent(Vector2 pos = Vector2(0,0), void (*_function)() = NULL)
	{
		position = pos;
		chainOfEvents.push_back(Event(_function));
	};

	void Execute()
	{
		for(int i = 0; i < chainOfEvents.size(); i++)
			chainOfEvents[i].Execute();
	};
};

class Timeline
{
public:
	std::string name;
	std::vector<TimeEventList> events;
	unsigned int currentFrame;
	unsigned int totalFrames;
	bool playing;
	bool loop;

	Timeline( std::string _name = "", unsigned int totalframes = 0)
	{
		currentFrame = 0;
		name = _name;
		totalFrames = totalframes;
		playing = false;
		loop = false;
	};
	~Timeline(){};
	
	void Play(bool _loop)
	{
		currentFrame = 0;
		loop = _loop;
		playing = true;
	};

	void Stop()
	{
		playing = false;
	};

	void update()
	{
		if (playing)
		{
			for(int i = 0; i < events.size(); i++)
				if (events[i].frame == currentFrame)
					events[i].Execute();

			if (currentFrame >= totalFrames - 1)
			{
				if (loop)
					Play(true);
				else
					Stop();
			}
			else
				currentFrame++;
		}
	};

	void AddEvent(unsigned int _frame, void (*_function)())
	{
		bool chainEvent = false;
		int chainto = 0;
		
		for(int i = 0; i < events.size() && !chainEvent; i++)
		{
			if (events[i].frame == _frame)
			{
				chainto = i;
				chainEvent = true;
				break;
			}
		}

		if (chainEvent)
			events[chainto].AddEvent(_frame, _function);
		else
			events.push_back(TimeEventList(_frame, _function));
	};
};

#endif