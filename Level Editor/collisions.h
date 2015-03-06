#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "vector.h"
#include "objects.h"
#include <vector>

enum CollisionFlags
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
	bool leftcollision;
	bool rightcollision;
	bool bottomcollision;
	bool topcollision;
	bool leftslopecollision;
	bool rightslopecollision;
	float X;
	float Y;
	unsigned short flags;
	bool centerXinRange;	

	Collision();
	Collision(	Vector2 mtd,float sa, 
				bool lc, bool rc, bool bc, bool tc,
				bool lsc, bool rsc, float x,float y, 
				unsigned short Flags, bool cxir);
	~Collision();

	void Initialize();

};

class CollidableTileList
{
public:
	std::vector<StaticObject> TileObjects;
	std::vector<unsigned short>	TileFlags;

	CollidableTileList();
	~CollidableTileList();

};

#endif