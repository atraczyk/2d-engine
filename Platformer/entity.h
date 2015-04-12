//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://andreastraczyk.com/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	entities : players,world.enemies,items
//-----------------------------------------------------------------------------

#ifndef ENTITY_H
#define ENTITY_H

#include "texture.h"
#include "point.h"
#include "vector.h"
#include "tile.h"
#include "collisions.h"
#include "objects.h"
#include "rectangle.h"
#include "rendertarget.h"
#include <vector>
#include <functional>
#include <algorithm>

#include "shared.h"

typedef enum CHARACTER_STATE 
{
	CS_IDLE			= 0x00, 
	CS_MOVINGLEFT	= 0x01,
	CS_MOVINGRIGHT	= 0x02,
	CS_JUMPING		= 0x04,
	CS_FALLING		= 0x08
}CHARACTER_STATE;

typedef enum ENEMY_TYPE 
{
	ET_KOOPA		= 0, 
	ET_GOOMBA		= 1,
	ET_SHELL		= 2
}ENEMY_TYPE;

typedef enum ITEM_TYPE 
{
	IT_MUSHROOM		= 0, 
	IT_STAR			= 1,
	IT_FLOWER		= 2
}ITEM_TYPE;

class CollisionInfo
{
public:
	float		area;
	std::string axis;
	int			index;

	CollisionInfo(float Area = 0,std::string Axis = NULL,int Index = 0)
	{
		area = Area;
		axis = Axis;
		index = Index;
	};
	~CollisionInfo(){};
};

#define NUM_SPRITE_ANIMATIONS 128

typedef enum ANIMATION 
{
	P_IDLE		= 0,	
	P_MOVE		= 1, 
	P_JUMP		= 2, 
	P_FALL		= 3, 
	P_KICK		= 4, 
	P_SWIM		= 5, 
	P_DUCK		= 6, 
	P_ATTACK	= 7, 
	P_SLIDE		= 8, 
	P_HOLD_LOOK	= 9,
	P_HOLD_IDLE	= 10,
	P_HOLD_MOVE	= 11,
	P_HOLD_JUMP	= 12,
	P_EXT4		= 13,
	P_EXT5		= 14,
	P_EXT6		= 15,
	
	E1_IDLE		= 16,
	E1_MOVE		= 17,
	E1_JUMP		= 18,
	
	E2_IDLE		= 32,
	E2_MOVE		= 33,
	E2_JUMP		= 34,
	
	E3_IDLE		= 48,
	E3_MOVE		= 49,
	E3_JUMP		= 50
}ANIMATION;

struct AnimationFrame
{
	ScreenRect sourceRectangle;
	int frameDelay;

	AnimationFrame(ScreenRect src_rect, unsigned int delay);
	~AnimationFrame(){};
};

class SpriteAtlas
{
public:
	unsigned int* texture;
	int totalFrames;
	Point imageSize;
	std::vector<AnimationFrame> animations[NUM_SPRITE_ANIMATIONS];

	SpriteAtlas(){};
	~SpriteAtlas(){};

	void initialize(unsigned int* ptex);
	void loadFromFile(const char* filename);
	void addFrame(unsigned int index, ScreenRect src_rect, unsigned int delay);
};

class Entity : public MovingObject
{
public:
	bool isSquished;
	Point toMove;
	bool inFront;
	bool isRunning;
	int currentState;
	Point currentAtlasFrame;
	Point lastFrame;
	int currentFrame;
	TileAtlas* atlas;
	SpriteAtlas* frames;
	int animationState;
	int currentAnimationFrame;

	float leftPadding;
	float rightPadding;
	float topPadding;
	float bottomPadding;
	float storeTopPadding;
	
	int jumpCountDown;
	int direction;
	bool canJump;

	int numCollisions;
	int totalCollisions;
	bool xcol;
	bool ycol;
	int lxycol;
	int xycol;
	bool ldcol;
	bool dcol;
	bool scol;

	std::vector<CollisionInfo> collisionInfos;
	CollidableTileList collidableTiles;

	Entity();
	~Entity();

	virtual void draw(Camera* camera) {};
	void	setAnimationState(ANIMATION state = (ANIMATION)-1, int frame = 0);
	void	initialize(float x, float y, float w, float h, float pl, float pr,float pt, float pb, TileAtlas* ptexatlas);
	void	populateCollisionTileArray(TileMap* ptilemap);
	float	intersectionArea(StaticObject* pobj);
	void	landing();
	bool	tileIntersectionTest(StaticObject* ptile, Collision& collision, unsigned short flags);
	bool	entityIntersectionTest(Entity* pentity, Collision& collision);
	void	advanceAnimationFrame(int elapsedframes, float framedivider, int endframe, bool loop);
	AnimationFrame* getFrame();
};

class Player : public Entity
{
public:
	bool canWarp;
	int score;
	int hit;
	int hitEnemy;
	bool tryingToHold;
	int holding;
	int kicking;
	bool lookingUp;
	bool crouching;
	bool swimming;
	int cantSwim;
	int health;

	Player();
	~Player();

	void	Hit();
	void	draw(Camera* camera);
	void	reinitialize(float x, float y);
	void	update( WorldRect* pconstraints,TileMap* pcollisionmap, TileMap* pfrontmap, std::vector<TileMap>* pdcollisionmaps, Vector2 world_accel,
					int elapsedFrames,Camera* pcamera,ScreenRect* pscreen);
	void	resolveTileCollisions(bool dynamic);
	void	resolveFrame(int elapsedframes);
	void	resolveItemCollisions(TileMap* pmap);
};

class Enemy : public Entity
{
public:
	ENEMY_TYPE type;
	bool isAlive;
	bool toRemove;
	int cantHitPlayer;
	bool beingHeld;
	Player* pHolder;

	Enemy(ENEMY_TYPE enemy_type, float x, float y, float w, float h, float ivx, float ivy, float pl, float pr,float pt, float pb, TileAtlas* ptexatlas);
	~Enemy();

	void	draw(Camera* camera);
	void	update( WorldRect* pconstraints,TileMap* pcollisionmap, std::vector<TileMap>* pdcollisionmaps, Vector2 world_accel,
					int elapsedFrames,Camera* pcamera,ScreenRect* pscreen,int index);
	void	resolveTileCollisions(bool dynamic);
	void	resolveFrame(int elapsedFrames);
};

class Item : public Entity
{
public:
	ENEMY_TYPE type;
	bool toRemove;

	Item(ITEM_TYPE item_type, float x, float y, float w, float h, float ivx, float ivy, float pl, float pr,float pt, float pb, TileAtlas* ptexatlas);
	~Item();

	void	draw(Camera* camera);
	void	update( WorldRect* pconstraints,TileMap* pcollisionmap, std::vector<TileMap>* pdcollisionmaps, Vector2 world_accel,
					int elapsedFrames,Camera* pcamera,ScreenRect* pscreen,int index);
	void	resolveTileCollisions(bool dynamic);
	void	resolveFrame(int elapsedFrames);
};

#endif