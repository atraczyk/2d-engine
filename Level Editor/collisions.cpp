#include "collisions.h"

//Collision
Collision::Collision()
{
	area = 0;
	minimumTranslation = Vector2(0,0);
	leftcollision = false;
	rightcollision = false;
	bottomcollision = false;
	topcollision = false;
	leftslopecollision = false;
	rightslopecollision = false;
	X = 0;
	Y = 0;
	flags = 0x00;
	centerXinRange = false;
}

Collision::Collision( Vector2 mtd,float sa, 
					bool lc, bool rc, bool bc, bool tc,
					bool lsc, bool rsc, float x,float y, 
					unsigned short Flags, bool cxir)	
{
	area = sa;
	minimumTranslation = mtd; 
	leftcollision = lc;
	rightcollision = rc;
	bottomcollision = bc;
	topcollision = tc;
	leftslopecollision = lsc;
	rightslopecollision = rsc;
	X = x;
	Y = y;
	flags = Flags;
	centerXinRange = cxir;
}

Collision::~Collision()
{
}

void Collision::Initialize()
{
	leftcollision = false;
	rightcollision = false;
	bottomcollision = false;
	topcollision = false;
	flags = 0x00;
}

//CollidableTileList
CollidableTileList::CollidableTileList()
{
}

CollidableTileList::~CollidableTileList()
{
}