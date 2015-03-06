//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	collision types & descriptions 
//-----------------------------------------------------------------------------

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "vector.h"
#include "objects.h"
#include <vector>

enum 
{
	CT_NONE			= 0x0000,
	CT_LEFT			= 0x0001,
	CT_RIGHT		= 0x0002,
	CT_TOP			= 0x0004,
	CT_BOTTOM		= 0x0008,
	CT_SQUARE		= 0x000F,
	CT_LEFT_SLOPE	= 0x0010,
	CT_RIGHT_SLOPE	= 0x0020,
	CT_0_TO_7		= 0x0040,
	CT_8_TO_15		= 0x0080,
	CT_0_TO_15		= 0x0100,
	CT_SPECIAL		= 0x1000
};

class Collision
{
public:
	Vector2 minimumTranslation;
	float area;
	bool leftCollision;
	bool rightCollision;
	bool bottomCollision;
	bool topCollision;
	bool leftSlopeCollision;
	bool rightSlopeCollision;
	float x;
	float y;
	USHORT flags;
	bool centerInBetween;
	int tileIndex;
	Vector2 velocity;

	Collision( Vector2 mtd = Vector2(0,0),float sa = 0, 
		bool lc = false, bool rc = false, bool bc = false, bool tc = false,
		bool lsc = false, bool rsc = false, float _x = 0,float _y = 0, 
		unsigned short Flags = 0x00, bool cxir = false, int index = 0, Vector2 vel = Vector2(0,0))	
	{
		area = sa;
		minimumTranslation = mtd; 
		leftCollision = lc;
		rightCollision = rc;
		bottomCollision = bc;
		topCollision = tc;
		leftSlopeCollision = lsc;
		rightSlopeCollision = rsc;
		x = _x;
		y = _y;
		flags = Flags;
		centerInBetween = cxir;
		velocity = vel;
	};
	~Collision(){};

	void Collision::Initialize()
	{
		area = 0;
		minimumTranslation =  Vector2(0,0); 
		leftCollision = false;
		rightCollision = false;
		bottomCollision = false;
		topCollision = false;
		leftSlopeCollision = false;
		rightSlopeCollision = false;
		x = 0;
		y = 0;
		flags = 0x00;
		centerInBetween = false;
		velocity = Vector2(0,0);
	};

	Collision& operator=(Collision obj)
	{
		std::swap(area, obj.area);
		std::swap(minimumTranslation, obj.minimumTranslation);
		std::swap(leftCollision, obj.leftCollision);
		std::swap(rightCollision, obj.rightCollision);
		std::swap(bottomCollision, obj.bottomCollision);
		std::swap(topCollision, obj.topCollision);
		std::swap(leftSlopeCollision, obj.leftSlopeCollision);
		std::swap(rightSlopeCollision, obj.rightSlopeCollision);
		std::swap(x, obj.x);
		std::swap(y, obj.y);
		std::swap(flags, obj.flags);
		std::swap(centerInBetween, obj.centerInBetween);
		std::swap(velocity, obj.velocity);
		return *this;
	}
};

class CollidableTileList
{
public:
	std::vector<StaticObject>	tileObjects;
	std::vector<unsigned short>	tileFlags;
	std::vector<int>			tileIndices;
	Vector2						velocity;

	CollidableTileList(){};
	~CollidableTileList(){};

};

#endif