//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	entities : players,world.enemies,items
//-----------------------------------------------------------------------------

#include "entity.h"
#include "utils.h"
#include "sound.h"
#include "input.h"
#include "world.h"

#include <fstream>
#include <sstream>

extern Sound				sound; 
extern World				world;
extern Input				input;

AnimationFrame::AnimationFrame(ScreenRect src_rect, unsigned int delay)
{
	sourceRectangle = src_rect;
	frameDelay = delay;
}

void SpriteAtlas::initialize(unsigned int* ptex)
{
	texture = ptex;
}

void SpriteAtlas::loadFromFile(const char* filename)
{
	string line;
	ifstream file(filename);
	istringstream ssf;
	int delay;
	int index;
	ScreenRect srcRect;

	if ( file.is_open() ) 
	{
		cout << "fileopen" << endl;
		getline(file, line);
		if (line.compare( 0, 3, "\xEF\xBB\xBF" ) == 0)
			line.erase( 0, 3 );
		ssf = istringstream(line); 
		ssf >> imageSize.x >> imageSize.y;
		cout << "l0: " << line << endl;
		while(getline(file, line))
		{
			ssf = istringstream(line);
			cout << "li: " << line << endl;
			ssf >> index;
			if(index != -1)
			{				
				while(getline(file, line))
				{
					ssf = istringstream(line);
					cout << "lf: " << line << endl;
					ssf >> delay >> srcRect.x >> srcRect.y >> srcRect.w >> srcRect.h;
					if(delay == -1)
						break;
					else
					{
						srcRect.w = srcRect.w / imageSize.x;
						srcRect.h = - srcRect.h / imageSize.y;
						srcRect.x = srcRect.x / imageSize.x;
						srcRect.y = 1.0 - srcRect.y / imageSize.y;						
						addFrame(index, srcRect, delay);
					}
				}
			}
		}
		file.close();
	}
	int na=0;
	for(int i=0; i<NUM_SPRITE_ANIMATIONS; i++)
		na = animations[i].size()?na+1:na;
	cout << "number of animations: " << na << endl;
	for(int i=0; i<NUM_SPRITE_ANIMATIONS;i++)
	{
		if(animations[i].size())
			cout << "index: " << i << endl;
		for(int c=0; c<animations[i].size(); c++)
			cout << "delay: " << animations[i][c].frameDelay	<< " x: " << animations[i][c].sourceRectangle.x
																<< " y: " << animations[i][c].sourceRectangle.y 
																<< " w: " << animations[i][c].sourceRectangle.w
																<< " h: " << animations[i][c].sourceRectangle.h << endl;
	}	
}

void SpriteAtlas::addFrame(unsigned int index, ScreenRect src_rect, unsigned int delay)
{
	animations[index].push_back(AnimationFrame(src_rect,delay));
}

Entity::Entity()
{
	atlas = NULL;
}

void Entity::initialize(float x, float y, float w, float h, float pl, float pr,float pt, float pb, TileAtlas* ptexatlas)
{
	animationState = 0;
	atlas = ptexatlas;
	isRunning = false;
	inFront = true;
	position = Vector2(x, y);
	size = Vector2(w, h);
	leftPadding = pl;
	rightPadding = pr;
	topPadding = pt;
	bottomPadding = pb;
	storeTopPadding = pt;
	velocity = Vector2(0,0);
	direction = 1;
	currentState = 0;
	toMove.x = 0;
	toMove.y = 0;
	currentFrame = 0;
	jumpCountDown = JUMPFRAMES;
	canJump = true;
}

Entity::~Entity()
{
}

void Entity::setAnimationState(ANIMATION state, int frame)
{
	animationState = state;
	currentAnimationFrame = frame;
}

AnimationFrame* Entity::getFrame()
{
	return &frames->animations[animationState][currentAnimationFrame];
}

void Entity::populateCollisionTileArray(TileMap* pcollisionmap)
{
	collidableTiles.tileObjects.clear();
	collidableTiles.tileFlags.clear();
	collidableTiles.tileIndices.clear();
	collidableTiles.velocity = Vector2(0,0);

	Point BottomLeft = worldToTile(position + Vector2(leftPadding,bottomPadding)- pcollisionmap->position, pcollisionmap->w_tileSize);
	Point TopRight = worldToTile(position - Vector2(rightPadding,topPadding) + size - pcollisionmap->position, pcollisionmap->w_tileSize);

	Point min;
	Point max;

	min.x = max(0, BottomLeft.x);
	min.y = max(0, BottomLeft.y);
	max.x = min(TopRight.x + 1, pcollisionmap->t_mapSize.x);
	max.y = min(TopRight.y + 1, pcollisionmap->t_mapSize.y);

	float tx,ty; 

	for (int y = min.y; y < max.y; y++)
	{
		for (int x = min.x; x < max.x; x++)
		{
			if (pcollisionmap->collisionLayer[x][y].isCollidable)
			{
				tx = (float)(x * pcollisionmap->w_tileSize.x) + pcollisionmap->position.x;
				ty = (float)(y * pcollisionmap->w_tileSize.y) + pcollisionmap->position.y;
				collidableTiles.tileObjects.push_back(StaticObject(Vector2(tx, ty), Vector2(pcollisionmap->w_tileSize.x, pcollisionmap->w_tileSize.y)));
				collidableTiles.tileFlags.push_back(pcollisionmap->collisionLayer[x][y].collisionFlags);
				collidableTiles.tileIndices.push_back(pcollisionmap->imageLayer[x][y].tileIndex);
				collidableTiles.velocity = pcollisionmap->velocity;
			}
		}
	}
}

bool Entity::tileIntersectionTest(StaticObject* ptile, Collision& collision, unsigned short flags)
{
	float pl = position.x + leftPadding;
	float pr = position.x + size.x - rightPadding;
	float pb = position.y + bottomPadding;
	float pt = position.y + size.y - topPadding;
	float pcx = position.x + size.x * 0.5f;
	float tl = ptile->position.x;
	float tr = ptile->position.x + ptile->size.x;
	float tb = ptile->position.y;
	float tt = ptile->position.y + ptile->size.y;

	bool xintersects = false;
	bool yintersects = false;

	float mtd_x = 0.0f;
	float mtd_y = 0.0f;

	if((pl < tl && pr > tl))
	{
		xintersects = true;
		mtd_x = tl - pr;
	}
	else if((pr > tr && pl < tr)) 
	{
		xintersects = true;
		mtd_x = tr - pl;
	}
	else if((pl >= tl && pr <= tr))
	{
		xintersects = true;
		mtd_x = min(tl - pr,tr - pl);
	}
	else if((pl <= tl && pr >= tr))
	{
		xintersects = true;
		mtd_x = min(tl - pr, tr - pl);
	}

	if((pb < tb && pt > tb))
	{
		yintersects = true;
		mtd_y = tb - pt;
	}
	else if((pt > tt && pb < tt)) 
	{
		yintersects = true;
		mtd_y = tt - pb;
	}
	else if((pb >= tb && pt <= tt) )
	{
		yintersects = true;
		mtd_y = min(tb - pt,tt - pb);
	}
	else if((pb <= tb && pt >= tt))
	{
		yintersects = true;
		mtd_y = min(tb - pt, tt - pb);
	}           

	if(xintersects && yintersects)
	{
		collision.flags = flags;
		if(flags & CT_LEFT_SLOPE || flags & CT_RIGHT_SLOPE)
		{
			float t = (pcx - tl) / ptile->size.x;;
			collision.leftSlopeCollision = (flags & CT_LEFT_SLOPE) ? true : false;
			collision.rightSlopeCollision = !collision.leftSlopeCollision;
			collision.centerInBetween = (pcx >= tl && pcx <= tr);  
			float leftfloor,rightfloor;			 
			if(collision.leftSlopeCollision)
			{
				if(flags & CT_0_TO_15)
				{
					leftfloor = 0; 
					rightfloor = ptile->size.y; 
				}
				else
				{
					leftfloor = (flags & CT_0_TO_7) ? 0 : ptile->size.y * 0.5f; 
					rightfloor = (flags & CT_0_TO_7) ? ptile->size.y * 0.5f : ptile->size.y; 
				}
			}
			else
			{
				if(flags & CT_0_TO_15)
				{
					leftfloor =  ptile->size.y; 
					rightfloor = 0; 
				}
				else
				{
					leftfloor = (flags & CT_0_TO_7) ? ptile->size.y * 0.5f : ptile->size.y; 
					rightfloor = (flags & CT_0_TO_7) ? 0 : ptile->size.y * 0.5f; 
				}
			}
			mtd_y = (tb + ((1 - t) * leftfloor + t * rightfloor)) - pb;
			collision.minimumTranslation = Vector2(0, mtd_y);			
		}
		else if(abs(mtd_x) < abs(mtd_y))
		{
			collision.leftCollision = mtd_x < 0 ? true : false;
			collision.rightCollision = !collision.leftCollision;
			if(collision.rightCollision && !(flags & CT_RIGHT))
				return false;
			else if(collision.leftCollision && !(flags & CT_LEFT))
				return false;
			collision.minimumTranslation = Vector2(mtd_x, 0);			
		}
		else if(abs(mtd_x) > abs(mtd_y))
		{
			collision.topCollision = mtd_y > 0 ? true : false;
			collision.bottomCollision = !collision.topCollision;
			if(collision.topCollision && !(flags & CT_TOP))
				return false;
			else if(collision.bottomCollision && !(flags & CT_BOTTOM))
				return false;
			collision.minimumTranslation = Vector2(0, mtd_y);			
		}
		return true;
	}
	return false;
}

bool Entity::entityIntersectionTest(Entity* pentity, Collision& collision)
{
	float al = position.x + leftPadding;
	float area = position.x + size.x - rightPadding;
	float ab = position.y + bottomPadding;
	float at = position.y + size.y - topPadding;
	float bl = pentity->position.x + pentity->leftPadding;
	float br = pentity->position.x + pentity->size.x - pentity->rightPadding;
	float bb = pentity->position.y + pentity->bottomPadding;
	float bt = pentity->position.y + pentity->size.y - pentity->topPadding;

	bool xintersects = false;
	bool yintersects = false;

	float mtd_x = 0.0f;
	float mtd_y = 0.0f;

	if((al < bl && area > bl))
	{
		xintersects = true;
		mtd_x = bl - area;
	}
	else if((area > br && al < br)) 
	{
		xintersects = true;
		mtd_x = br - al;
	}
	else if((al >= bl && area <= br))
	{
		xintersects = true;
		mtd_x = min(bl - area,br - al);
	}
	else if((al <= bl && area >= br))
	{
		xintersects = true;
		mtd_x = min(bl - area, br - al);
	}

	if((ab < bb && at > bb))
	{
		yintersects = true;
		mtd_y = bb - at;
	}
	else if((at > bt && ab < bt)) 
	{
		yintersects = true;
		mtd_y = bt - ab;
	}
	else if((ab >= bb && at <= bt) )
	{
		yintersects = true;
		mtd_y = min(bb - at,bt - ab);
	}
	else if((ab <= bb && at >= bt))
	{
		yintersects = true;
		mtd_y = min(bb - at, bt - ab);
	}           

	if(xintersects && yintersects)
	{
		if(abs(mtd_x) < abs(mtd_y))
		{
			collision.leftCollision = mtd_x < 0 ? true : false;
			collision.rightCollision = !collision.leftCollision;
			collision.minimumTranslation = Vector2(mtd_x, 0);			
		}
		else if(abs(mtd_x) > abs(mtd_y))
		{
			collision.topCollision = mtd_y > 0 ? true : false;
			collision.bottomCollision = !collision.topCollision;
			collision.minimumTranslation = Vector2(0, mtd_y);			
		}
		return true;
	}
	return false;
}

float Entity::intersectionArea(StaticObject* pobj)
{
	float x11 = position.x + leftPadding;
	float y11 = position.y + bottomPadding;
	float x12 = position.x + size.x - rightPadding;
	float y12 = position.y + size.y - topPadding;
	float x21 = pobj->position.x;
	float y21 = pobj->position.y;
	float x22 = pobj->position.x + pobj->size.x;
	float y22 = pobj->position.y + pobj->size.y;

	float x_overlap = max(0, min(x12, x22) - max(x11, x21));
	float y_overlap = max(0, min(y12, y22) - max(y11, y21));

	return x_overlap * y_overlap;
}

Player::Player()
{
	cantSwim = 0;
	swimming = false;
	isSquished = false;
	hit = 0;
	score = 0;
	holding = 0;
	tryingToHold = false;
	kicking = 0;
	lookingUp = false;
	crouching = false;
	health = 3;
}

Player::~Player()
{
}

void Player::draw(Camera* camera)
{
	ScreenRect srcRect = getFrame()->sourceRectangle;
	int flashsmall = (health<2 && hit>59)?(hit%3?1:0):(health<2?1:0);
	srcRect.y += flashsmall*srcRect.h;
	cout<< flashsmall;
	if(direction == -1)
	{
		srcRect.x += srcRect.w;
		srcRect.w *= -1;
	}

	ScreenRect dstRect((int)position.x, (int)position.y, size.x, size.y);
	glBindTexture(GL_TEXTURE_2D, *(atlas->texture));
	glBegin(GL_QUADS);
	{
		glTexCoord2d(srcRect.fLeft(), srcRect.fTop());	glVertex2f(dstRect.fLeft(), dstRect.fBottom());
		glTexCoord2d(srcRect.fRight(), srcRect.fTop());	glVertex2f(dstRect.fRight(), dstRect.fBottom());
		glTexCoord2d(srcRect.fRight(), srcRect.fBottom());	glVertex2f(dstRect.fRight(),  dstRect.fTop());
		glTexCoord2d(srcRect.fLeft(), srcRect.fBottom());	glVertex2f(dstRect.fLeft(),  dstRect.fTop());
		if(swimming && !crouching && (currentState == CS_JUMPING || currentState == CS_FALLING ))
		{			
			srcRect = getFrame()->sourceRectangle;
			if(direction == -1)
			{
				srcRect.w *= -1;
				dstRect = ScreenRect((int)position.x + size.x, (int)position.y, size.x, size.y);
			}
			else
			{
				srcRect.x -= srcRect.w;
				dstRect = ScreenRect((int)position.x - size.x, (int)position.y, size.x, size.y);
			}
			glTexCoord2d(srcRect.fLeft(), srcRect.fTop());	glVertex2f(dstRect.fLeft(), dstRect.fBottom());
			glTexCoord2d(srcRect.fRight(), srcRect.fTop());	glVertex2f(dstRect.fRight(), dstRect.fBottom());
			glTexCoord2d(srcRect.fRight(), srcRect.fBottom());	glVertex2f(dstRect.fRight(),  dstRect.fTop());
			glTexCoord2d(srcRect.fLeft(), srcRect.fBottom());	glVertex2f(dstRect.fLeft(),  dstRect.fTop());
		}
	}
	glEnd();
}

void Player::reinitialize(float x, float y)
{
	animationState = 0;
	cantSwim = 0;
	swimming = false;
	isSquished = false;
	topPadding = storeTopPadding;
	holding = 0;
	kicking = 0;
	lookingUp = false;
	crouching = false;
	health = 3;
	hit = 0;
	direction = 1;
	currentState = 0;
	currentAtlasFrame = Point(0, 0);
	lastFrame = Point(0, 0);
	position = Vector2(x, y);;
	velocity = Vector2(0.0f, 0.0f);
	lastVelocity = Vector2(0.0f, 0.0f);
	currentFrame = 0;
	jumpCountDown = JUMPFRAMES;
	toMove.y = 0;
	lxycol = 0;
	ldcol = 0;
}

void Player::Hit()
{
	sound.PlaySfx(SOUND_HURT);
	hit = 120;
	health--;
	score--;
	if(health<2 && !crouching)
		topPadding += (size.y/2 - bottomPadding);
	else if (!crouching)
		topPadding = storeTopPadding;
}

void Player::update( WorldRect* pconstraints, TileMap* pcollisionmap, TileMap* pfrontmap, std::vector<TileMap>* pdcollisionmaps, Vector2 world_accel,
					int elapsedFrames,Camera* pcamera,ScreenRect* pscreen)
{
	int pmovex = (!crouching || (crouching && (currentState == CS_JUMPING || currentState == CS_FALLING)))?toMove.x:0;
	if(pmovex != 0) 
	{
		if (pmovex == -1)
		{
			if (currentState != CS_JUMPING && currentState != CS_FALLING)
				currentState = CS_MOVINGLEFT;
			direction = -1;
		}
		else if (pmovex == 1)
		{
			if (currentState != CS_JUMPING && currentState != CS_FALLING)
				currentState = CS_MOVINGRIGHT;
			direction = 1;
		}
		if(!xycol && !ycol)
		{
			if (velocity.x * pmovex < 0 ) 
				velocity.x += X_AIR_MULTIPLIER * X_MDECELERATION * pmovex;
			else 
				velocity.x += X_AIR_MULTIPLIER * X_ACCELERATION * pmovex;
		}
		else
		{
			if (velocity.x * pmovex < 0 ) 
				velocity.x += X_MDECELERATION * pmovex;
			else if(!crouching)
				velocity.x += X_ACCELERATION * pmovex;
		}
	}
	else if(pmovex == 0 && currentState != CS_FALLING
		&& currentState != CS_JUMPING)
	{
		if (velocity.x < 0.0f) 
		{
			velocity.x += X_DECELERATION;
			if (velocity.x > 0.0f) 
				velocity.x = 0.0f;
		} 
		else if (velocity.x > 0.0f) 
		{
			velocity.x -= X_DECELERATION;
			if (velocity.x < 0.0f) 
				velocity.x = 0.0f;
		}
	}	

	if(swimming)
		isRunning = false;

	if(!isRunning)
	{
		float max = swimming?MAX_LIQUID_X_VELOCITY:MAX_X_VELOCITY;
		if (velocity.x < -max) 
			velocity.x = -max;
		else if (velocity.x > max) 
			velocity.x = max;
	}
	else
	{
		float max = xycol==2 ? S_MAX_X_RUN_VELOCITY : MAX_X_RUN_VELOCITY;
		if (velocity.x < -max) 
			velocity.x = -max;
		else if (velocity.x > max) 
			velocity.x = max;
	}

	//y
	if (toMove.y == -1)
	{
	}
	if (toMove.y == 1)
	{
		if(swimming && canJump)
		{
			//swim
			currentAnimationFrame = 0;
			currentAtlasFrame.x = 13;
			currentFrame = 0.0;

			velocity.y = 6.0f;
			currentState = CS_JUMPING;

			canJump = false;
		}
		else if ( !swimming && jumpCountDown > 0 && currentState != CS_FALLING)
		{
			//jump
			if (jumpCountDown == JUMPFRAMES) 
			{
				currentAnimationFrame = 0;
				currentAtlasFrame.x = 10;
				currentFrame = 0.0;
			}
			currentState = CS_JUMPING;
			velocity.y += Y_ACCELERATION;

			if(dcol)
				velocity.y += D_GRAVITY;
			else
			{
				if(xycol || scol)
					velocity.y += S_GRAVITY;

				if(xycol == 2)
					velocity.y += S_GRAVITY;
			}			
		}
		jumpCountDown--;
	}

	if(swimming)
		velocity += world_accel * 0.15f;
	else
		velocity += world_accel;

	if(dcol)
		velocity.y += -D_GRAVITY;
	else if(xycol || scol)
		velocity.y += -S_GRAVITY;

	float max_y_v = swimming?MAX_LIQUID_Y_VELOCITY:MAX_Y_VELOCITY;
	if(velocity.y > max_y_v)
		velocity.y = max_y_v;
	if(velocity.y < -max_y_v)
		velocity.y = -max_y_v;

	lastPosition = position;
	lastVelocity = velocity;

	position += velocity;

	ldcol = dcol;
	dcol = false;
	for(int i=0;i<pdcollisionmaps->size();i++)
	{
		populateCollisionTileArray(&(*pdcollisionmaps)[i]);
		resolveTileCollisions(true);
	}

	populateCollisionTileArray(pcollisionmap);
	resolveTileCollisions(false);

	swimming = false;
	resolveItemCollisions(pcollisionmap);
	if(pfrontmap)
		resolveItemCollisions(pfrontmap);

	resolveFrame(elapsedFrames);

	Collision collision;
	for(int i=0; i<world.enemies.size(); i++)
	{
		collision.Initialize();
		if(world.enemies[i].isAlive && !world.enemies[i].cantHitPlayer)
		{
			if(entityIntersectionTest(&world.enemies[i], collision))
			{
				if(((position.y + bottomPadding) > (world.enemies[i].position.y + world.enemies[i].bottomPadding + 32.0f))) //TOP HIT
				{
					if(!hitEnemy) 
					{
						hitEnemy = 10;
						sound.PlaySfx(SOUND_STOMP);
					}
					switch(world.enemies[i].type)
					{
					case ET_KOOPA:
						world.enemies[i].toRemove = true;
						world.enemies.push_back(Enemy(ET_SHELL, world.enemies[i].position.x, world.enemies[i].position.y, 64.0f, 128.0f, 0.0f, 0.0f, 12.0f, 12.0f, 72.0f, 8.0f, atlas));
						break;
					case ET_GOOMBA:
						world.enemies[i].isAlive = false;
						world.enemies[i].velocity.y = 10.0f;
						break;
					case ET_SHELL: 
						if(world.enemies[i].velocity.x)
						{
							world.enemies[i].velocity.x = 0.0f;
						}
						else if(!world.enemies[i].cantHitPlayer)
						{
							world.enemies[i].cantHitPlayer = CANTHITPLAYERFRAMES;
							if(position.x + size.x/2 < world.enemies[i].position.x + world.enemies[i].size.x/2)
							{
								world.enemies[i].velocity.x = ENEMY2_X_SPEED;
								world.enemies[i].direction = 1;
							}
							else 
							{
								world.enemies[i].velocity.x = -ENEMY2_X_SPEED;
								world.enemies[i].direction = -1;
							}
						}

						break;
					}
					if(velocity.y<0)
					{
						if(input.gamepad.buttonDown(GPB_A) || input.keyboard.keyDown('N')) 
						{
							currentState = CS_JUMPING;
							jumpCountDown = JUMPFRAMES/2;
							toMove.y = 1;
						}
						velocity.y = 2 * Y_ACCELERATION;
					}
				}
				else if(!hit) //SIDE HIT
				{
					switch(world.enemies[i].type)
					{
					case ET_KOOPA:
						Hit();
						break;
					case ET_GOOMBA:
						Hit();
						break;
					case ET_SHELL:
						if((world.enemies[i].velocity.x > 0 && position.x + size.x/2 > world.enemies[i].position.x + world.enemies[i].size.x/2) ||
							(world.enemies[i].velocity.x < 0 && position.x + size.x/2 < world.enemies[i].position.x + world.enemies[i].size.x/2)) //HURT
						{
							if(!world.enemies[i].beingHeld)
							{
								Hit();
							}
						}
						else if(!world.enemies[i].velocity.x) //SAFE
						{
							if(tryingToHold && !holding) //HOLD
							{
								holding = world.enemies[i].type;
								world.enemies[i].beingHeld = true;
								world.enemies[i].pHolder = this;
							}
							else if(!world.enemies[i].beingHeld) //KICK
							{
								kicking = 8;
								world.enemies[i].cantHitPlayer = CANTHITPLAYERFRAMES;
								sound.PlaySfx(SOUND_STOMP);
								if(position.x < world.enemies[i].position.x)
								{
									world.enemies[i].velocity.x = ENEMY2_X_SPEED;
									world.enemies[i].direction = 1;
								}
								else
								{
									world.enemies[i].velocity.x = -ENEMY2_X_SPEED;
									world.enemies[i].direction = -1;
								}
							}
						}
						break;
					}
				}
			}
		}
	}

	if(hit) hit--;
	if(hitEnemy) hitEnemy--;
	if(kicking) kicking--;
	if(cantSwim) cantSwim--;

	if(!xycol && lxycol && !ycol)
	{
		position.y += S_GRAVITY;
		velocity.y += S_GRAVITY;
	}

	if(!dcol && ldcol && !ycol)
	{
		position.y += D_GRAVITY;
		velocity.y += D_GRAVITY;
	}

	canWarp = ycol && (totalCollisions == 2 && collisionInfos[0].index == 29);  
}

void Player::resolveTileCollisions(bool dynamic)
{
	numCollisions = 0;
	totalCollisions = 0;
	collisionInfos.clear();

	Collision collision;
	std::vector<Collision> collisions;
	Collision bestXCollision;
	Collision bestYCollision;
	Collision bestXYCollision;
	bestYCollision.y = -1;
	float area = 0.0f;
	float area_sum = 0.0f;
	float tileheight = 0.0f;

	if(collidableTiles.tileObjects.size() > 0)
		tileheight = collidableTiles.tileObjects[0].size.y;

	for (int i = 0; i < collidableTiles.tileObjects.size(); i++)
	{
		collision.Initialize();
		if (tileIntersectionTest(&collidableTiles.tileObjects[i], collision, collidableTiles.tileFlags[i]))
		{
			totalCollisions++;
			area = intersectionArea(&collidableTiles.tileObjects[i]);
			std::string axis = (collision.topCollision || collision.bottomCollision) ? "y" : "x";
			axis = (collision.leftSlopeCollision || collision.rightSlopeCollision) ? "xy" : axis;

			if(area > 0.0f)
			{
				if(!dynamic && !(collision.flags == CT_TOP) && !axis.compare( "y"))
					area_sum += area;
				collisionInfos.push_back( CollisionInfo(area,axis,collidableTiles.tileIndices[i]));
				collisions.push_back( Collision(collision.minimumTranslation,area,
					collision.leftCollision,
					collision.rightCollision,
					collision.bottomCollision,
					collision.topCollision,
					collision.leftSlopeCollision,
					collision.rightSlopeCollision,
					collidableTiles.tileObjects[i].position.x, 
					collidableTiles.tileObjects[i].position.y,
					collision.flags,
					collision.centerInBetween,
					collidableTiles.tileIndices[i],collidableTiles.velocity));
			}
		}
	}

	//descending sort
	std::sort(	collisions.begin(), collisions.end(), 
		[](Collision const & a, Collision const & b) -> bool
	{ return a.area > b.area; });
	int newcount = collisions.size() > 2 ? collisions.size() - 1 : collisions.size();
	for (int i = 0; i < newcount; i++)
	{
		if (collisions[i].leftCollision || collisions[i].rightCollision)
		{
			if (collisions[i].area > bestXCollision.area)
				bestXCollision = collisions[i];
		}
		else if (collisions[i].bottomCollision || collisions[i].topCollision)
		{
			if (collisions[i].area > bestYCollision.area)
				bestYCollision = collisions[i];
		}
		else if (collisions[i].leftSlopeCollision || collisions[i].rightSlopeCollision)
		{
			if (collisions[i].centerInBetween > bestXYCollision.centerInBetween)
				bestXYCollision = collisions[i];
		}
	}

	isSquished = false;

	xcol = false;
	ycol = false;
	lxycol = xycol;
	xycol = 0;	
	scol = false; 

	Point xtile = Point(bestXCollision.x/world.level.layer[2].w_tileSize.x,bestXCollision.y/world.level.layer[2].w_tileSize.y);
	Point ytile = Point(bestYCollision.x/world.level.layer[2].w_tileSize.x,bestYCollision.y/world.level.layer[2].w_tileSize.y);

	if(bestXCollision.x == bestYCollision.x && ytile.y == xtile.y - 2 && !xycol && xtile.x)
	{
		isSquished = true;
	}

	if(bestXYCollision.centerInBetween && bestXYCollision.minimumTranslation.y > 0.0f &&
		(velocity.y <= 0) && 
		(lastPosition.y + tileheight * 0.5 >= (position.y + bestXYCollision.minimumTranslation.y))) //XY
	{
		if(bestXYCollision.flags & CT_0_TO_15)
			xycol = 2;
		else
			xycol = 1;
		numCollisions++;
		position += bestXYCollision.minimumTranslation;
		landing();
		velocity.y = 0.0f;
	}

	if (bestXCollision.x != bestYCollision.x && !xycol)//y
	{
		if ((bestYCollision.topCollision && velocity.y <= 0) || isSquished) //land
		{
			if(((bestYCollision.flags & CT_TOP) && !(bestYCollision.flags & CT_BOTTOM) &&
				!(bestYCollision.flags & CT_SPECIAL)) && 
				(lastPosition.y < (position.y + bestYCollision.minimumTranslation.y)))
				return;

			ycol = true;
			if(bestYCollision.flags & CT_SPECIAL)
			{
				scol = true;
			}
			numCollisions++;
			position += bestYCollision.minimumTranslation;
			landing();
			velocity.y = 0.0f;
			if(dynamic)
			{
				dcol = true; 
				position.x += bestYCollision.velocity.x;
			}	
		}
		else if (bestYCollision.bottomCollision && velocity.y >= 0) //hit head
		{
			ycol = true;
			numCollisions++;
			if(!dcol)
			{
				position += bestYCollision.minimumTranslation;
				sound.PlaySfx(SOUND_THUD);
			}
			currentState = CS_FALLING;
			velocity.y = 0.0f;			

			if(world.level.layer[2].imageLayer[ytile.x][ytile.y].atileIndex == 240 && !world.level.layer[2].imageLayer[ytile.x][ytile.y].bounce)
			{
				score++;
				world.level.layer[2].imageLayer[ytile.x][ytile.y].value--;
				world.level.layer[2].imageLayer[ytile.x][ytile.y].bounce = 10.0f;
				sound.PlaySfx(SOUND_COIN);
			}
		}
	}

	if (bestXCollision.y != bestYCollision.y && !isSquished)//x
	{
		if (bestXCollision.leftCollision || bestXCollision.rightCollision)
		{
			xcol = true;
			numCollisions++;
			if(bestXCollision.minimumTranslation.x > 0)
				bestXCollision.minimumTranslation.x = min(bestXCollision.minimumTranslation.x,4.0f);
			else
				bestXCollision.minimumTranslation.x = max(bestXCollision.minimumTranslation.x,-4.0f);
			position += bestXCollision.minimumTranslation;
			if((velocity.x > 0 && bestXCollision.leftCollision) || 	(velocity.x < 0 && bestXCollision.rightCollision))	
				velocity.x = 0.0f;
		}
	}

	if(!xycol && area_sum > 1500.0f)
	{
		isSquished = true;
	}

	if(isSquished)
	{
		position.y += 2.0f; //reverse gravity
		ycol = true;
		velocity.y = 0.0f;
		landing();
		position.x -= direction*5;
	}
}

void Player::resolveItemCollisions(TileMap* pmap)
{
	Point BottomLeft = worldToTile(position +  Vector2(leftPadding,bottomPadding) - pmap->position, pmap->w_tileSize);
	Point TopRight = worldToTile(position -  Vector2(rightPadding,topPadding) + size - pmap->position, pmap->w_tileSize);

	Point min;
	Point max;

	min.x = max(0, BottomLeft.x-1);
	min.y = max(0, BottomLeft.y-1);
	max.x = min(TopRight.x + 2, pmap->t_mapSize.x);
	max.y = min(TopRight.y + 2, pmap->t_mapSize.y);

	float tx,ty;
	float area;
	for (int y = min.y; y < max.y; y++)
	{
		for (int x = min.x; x < max.x; x++)
		{
			tx = (float)(x * pmap->w_tileSize.x) + pmap->position.x;
			ty = (float)(y * pmap->w_tileSize.y) + pmap->position.y;
			StaticObject tile = StaticObject(Vector2(tx,ty),Vector2(pmap->w_tileSize.x,pmap->w_tileSize.y));
			area = intersectionArea( &tile );
			if(area)
			{
				if (pmap->imageLayer[x][y].atileIndex == 241)
				{
					score++;
					for(int i=0;i<pmap->animatedTiles.size();i++)
					{
						if(pmap->animatedTiles[i].position.x == x && pmap->animatedTiles[i].position.y == y)
							remove(pmap->animatedTiles, i);
					}
					pmap->imageLayer[x][y].atileIndex = 0;
					sound.PlaySfx(SOUND_COIN);
					pmap->imageLayer[x][y].tileIndex = -1;
				}
				else if (position.y < ty+pmap->w_tileSize.y/4 && (pmap->imageLayer[x][y].atileIndex == 246 || pmap->imageLayer[x][y].tileIndex == 144)) //water
				{
					swimming = true;
				}
			}
		}
	}
}

void Entity::landing()
{
	if(toMove.y != -1) 
		toMove.y = 0;
	jumpCountDown = JUMPFRAMES;
	if (velocity.x > 0) 
		currentState = CS_MOVINGRIGHT;
	else if (velocity.x < 0) 
		currentState = CS_MOVINGLEFT;
	else if(!toMove.x || currentState == CS_FALLING || currentState == CS_JUMPING) 
		currentState = CS_IDLE;
}

void Entity::advanceAnimationFrame(int elapsedframes, float delaydivider, int endframe ,bool loop)
{
	currentFrame += elapsedframes;
	if (currentFrame > getFrame()->frameDelay / delaydivider)
	{
		currentAnimationFrame++;
		currentFrame = 0;
	}
	if(currentAnimationFrame > endframe)
		currentAnimationFrame = loop?0:endframe;
}

void Player::resolveFrame(int elapsedframes)
{
	lastFrame = currentAtlasFrame;

	if (velocity.y < 0) currentState = CS_FALLING;

	float delaydivider = 1.0f;
	int endframe = frames->animations[animationState].size() - 1;
	bool loop = false;
	bool advance = true;

	if(kicking)
	{
		setAnimationState(P_KICK,0);
	}
	else if(lookingUp && holding && currentState != CS_MOVINGLEFT && currentState != CS_MOVINGRIGHT)
	{
		setAnimationState(P_HOLD_LOOK,0);
	}
	else if(crouching)
	{
		setAnimationState(P_DUCK,0);
	}	
	else
	{
		switch(currentState)
		{
		case CS_IDLE:
			if(holding)
			{	
				setAnimationState(P_HOLD_IDLE,0);
			}
			else 
			{
				setAnimationState(P_IDLE,0);
			}
			break;

		case CS_MOVINGLEFT:case CS_MOVINGRIGHT:
			if(!holding)
			{
				setAnimationState(P_MOVE,currentAnimationFrame);
			}
			else
			{
				setAnimationState(P_HOLD_MOVE,currentAnimationFrame);
			}
			delaydivider = (1.0f + (2.0f*(int)isRunning));
			advanceAnimationFrame(elapsedframes, delaydivider, endframe, true);
			break;

		case CS_JUMPING:
			if(!holding)
			{				
				if(swimming)
				{
					setAnimationState(P_SWIM,currentAnimationFrame);
					advanceAnimationFrame(elapsedframes, delaydivider, endframe, false);
				}
				else
				{		
					setAnimationState(P_JUMP,currentAnimationFrame);
					advanceAnimationFrame(elapsedframes, delaydivider, endframe, false);
				}
			}
			else
			{
				setAnimationState(P_HOLD_JUMP,(health<2)?0:1);
			}
			break;

		case CS_FALLING:
			if(!holding)
			{
				if(swimming)
				{
					setAnimationState(P_SWIM,currentAnimationFrame);
					advanceAnimationFrame(elapsedframes, delaydivider, endframe, false);
				}
				else
				{
					setAnimationState(P_FALL,0);
				}				
			}
			else
			{
				setAnimationState(P_HOLD_JUMP,(health<2)?0:1);
			}
			break;
		}
	}
	//char* anims[16] = {"idle","move","jump","fall","kick","swim","duck","attack","slide","hold look","hold idle","hold move","hold jump","","","",};
	//cout<<"state: "<<anims[animationState]<<"  frame: "<<currentAnimationFrame<<endl;
}

Enemy::Enemy(ENEMY_TYPE enemy_type, float x, float y, float w, float h, float ivx, float ivy, float pl, float pr,float pt, float pb, TileAtlas* ptexatlas)
{
	beingHeld = false;
	cantHitPlayer = CANTHITPLAYERFRAMES;
	toRemove = false;
	isAlive = true;
	type = enemy_type;
	atlas = ptexatlas;
	isRunning = false;
	inFront = true;
	position = Vector2(x, y);
	size = Vector2(w, h);
	velocity = Vector2(ivx, ivy);
	direction = velocity.x>=0?1:-1;
	leftPadding = pl;
	rightPadding = pr;
	topPadding = pt;
	bottomPadding = pb;
	toMove.x = 0;
	toMove.y = 0;
	currentFrame = 0;
	jumpCountDown = JUMPFRAMES;
	canJump = true;

	switch(type)
	{
	case ET_KOOPA:		
		currentAtlasFrame = Point(0,3);
		currentState = CS_MOVINGRIGHT;
		break;
	case ET_GOOMBA:
		currentAtlasFrame = Point(0,4);
		currentState = CS_MOVINGRIGHT;
		break;
	case ET_SHELL:
		currentAtlasFrame = Point(2,3);
		currentState = CS_IDLE;
		break;
	}	
};

Enemy::~Enemy()
{
};

void Enemy::draw(Camera* camera)
{
	ScreenRect srcRect = atlas->sourceRectangles[currentAtlasFrame.x+(currentAtlasFrame.y)*atlas->frames.x];
	if(direction == -1)
	{
		srcRect.x += atlas->textureFrameSize.x;
		srcRect.w *= -1;
	}
	ScreenRect dstRect((int)position.x, (int)position.y, size.x, size.y);
	if(!isAlive && (type == ET_SHELL || type == ET_GOOMBA))
	{
		dstRect.y += (int)(size.y - topPadding) ;
		dstRect.h *= -1;
	}
	glBindTexture(GL_TEXTURE_2D, *(atlas->texture));
	glBegin(GL_QUADS);
	{
		glTexCoord2d(srcRect.fLeft(), srcRect.fTop());	glVertex2f(dstRect.fLeft(), dstRect.fBottom());
		glTexCoord2d(srcRect.fRight(), srcRect.fTop());	glVertex2f(dstRect.fRight(), dstRect.fBottom());
		glTexCoord2d(srcRect.fRight(), srcRect.fBottom());	glVertex2f(dstRect.fRight(),  dstRect.fTop());
		glTexCoord2d(srcRect.fLeft(), srcRect.fBottom());	glVertex2f(dstRect.fLeft(),  dstRect.fTop());
	}
	glEnd();
}

void Enemy::update( WorldRect* pconstraints,TileMap* pcollisionmap, std::vector<TileMap>* pdcollisionmaps, Vector2 world_accel,
				   int elapsedFrames,Camera* pcamera,ScreenRect* pscreen,int index)
{
	lastPosition = position;
	lastVelocity = velocity;

	if(beingHeld)
	{
		if(pHolder->direction == 1)
		{
			position = Vector2(pHolder->position.x + pHolder->size.x - 24.0f, pHolder->position.y + 24.0f);
		}
		else
		{
			position = Vector2(pHolder->position.x - size.x + 24.0f, pHolder->position.y + 24.0f);
		}
	}
	else
	{
		if (toMove.y == 1 && currentState != CS_FALLING)
		{
			if (jumpCountDown > 0)
			{
				//JUMP
				if (jumpCountDown == JUMPFRAMES) 
				{
					currentAtlasFrame.x = 0;
					currentFrame = 0.0;
				}
				currentState = CS_JUMPING;
				velocity.y += Y_ACCELERATION;

				if(dcol)
					velocity.y += D_GRAVITY;
				else
				{
					if(xycol || scol)
						velocity.y += S_GRAVITY;

					if(xycol == 2)
						velocity.y += S_GRAVITY;
				}

				jumpCountDown--;
			}
		}

		world_accel.y = (type == ET_SHELL)?world_accel.y*0.5f:world_accel.y;
		velocity += world_accel;

		if(dcol)
			velocity.y += -D_GRAVITY;
		else if(xycol || scol)
			velocity.y += -S_GRAVITY;

		float max_y_v = (type == ET_SHELL)?MAX_SHELL_Y_VELOCITY:MAX_Y_VELOCITY;
		if(velocity.y > max_y_v)
			velocity.y = max_y_v;
		if(velocity.y < -MAX_Y_VELOCITY)
			velocity.y = -MAX_Y_VELOCITY;

		position += velocity;

	}

	if(isAlive)
	{
		if(!beingHeld)
		{
			ldcol = dcol;
			dcol = false;

			for(int i=0;i<pdcollisionmaps->size();i++)
			{
				populateCollisionTileArray(&(*pdcollisionmaps)[i]);
				resolveTileCollisions(true);
			}

			populateCollisionTileArray(pcollisionmap);
			resolveTileCollisions(false);
		}
		if(type == ET_SHELL && (beingHeld || velocity.x || velocity.y))
		{
			Collision collision;
			for(int i=0; i<world.enemies.size(); i++)
			{
				collision.Initialize();
				if(world.enemies[i].isAlive && index != i)
				{
					if(entityIntersectionTest(&world.enemies[i], collision))
					{
						sound.PlaySfx(SOUND_STOMP);
						switch(world.enemies[i].type)
						{
						case ET_KOOPA:
							world.enemies[i].currentAtlasFrame = Point(4,3);
							world.enemies[i].isAlive = false;
							world.enemies[i].velocity.y = 10.0f;
							world.enemies[i].velocity.x = direction * 8.0f;
							if(beingHeld)
							{
								beingHeld = false;
								pHolder->holding = 0;
								isAlive = false;
								velocity.y = 10.0f;
								velocity.x = -direction * 8.0f;
							}
							break;
						case ET_GOOMBA:
							world.enemies[i].isAlive = false;
							world.enemies[i].velocity.y = 10.0f;
							world.enemies[i].velocity.x = direction * 8.0f;
							if(beingHeld)
							{
								beingHeld = false;
								pHolder->holding = 0;
								isAlive = false;
								velocity.y = 10.0f;
								velocity.x = -direction * 8.0f;
							}
							break;
						case ET_SHELL:
							//*
							world.enemies[i].isAlive = false;
							world.enemies[i].velocity.y = 10.0f;
							int dir = (world.enemies[i].position.x > position.x)?1:-1;
							world.enemies[i].velocity.x = dir * 8.0f;
							if(world.enemies[i].beingHeld)
							{
								world.enemies[i].beingHeld = false;
								world.enemies[i].pHolder->holding = 0;
								isAlive = false;
								velocity.y = 10.0f;
								velocity.x = -dir * 8.0f;
							}
							if(beingHeld)
							{
								beingHeld = false;
								pHolder->holding = 0;
								isAlive = false;
								velocity.y = 10.0f;
								velocity.x = -dir * 8.0f;
							}
							//*/
							break;
						}
					}
				}
			}
		}

		resolveFrame(elapsedFrames);	
	}

	if(cantHitPlayer) cantHitPlayer--;

	if(position.y + size.y < pconstraints->y )
	{
		toRemove = true;
	}

	if(!xycol && lxycol && !ycol)
	{
		position.y += S_GRAVITY;
		velocity.y += S_GRAVITY;
	}

	if(!dcol && ldcol && !ycol)
	{
		position.y += D_GRAVITY;
		velocity.y += D_GRAVITY;
	}
}

void Enemy::resolveTileCollisions(bool dynamic)
{
	numCollisions = 0;
	totalCollisions = 0;
	collisionInfos.clear();

	Collision collision;
	std::vector<Collision> collisions;
	Collision bestXCollision;
	Collision bestYCollision;
	Collision bestXYCollision;
	bestYCollision.y = -1;
	float area = 0.0f;
	float tileheight = 0.0f;
	if(collidableTiles.tileObjects.size() > 0)
		tileheight = collidableTiles.tileObjects[0].size.y;

	for (int i = 0; i < collidableTiles.tileObjects.size(); i++)
	{
		collision.Initialize();
		if (tileIntersectionTest(&collidableTiles.tileObjects[i], collision, collidableTiles.tileFlags[i]))
		{
			totalCollisions++;
			area = intersectionArea(&collidableTiles.tileObjects[i]);
			std::string axis = (collision.topCollision || collision.bottomCollision) ? "y" : "x";
			axis = (collision.leftSlopeCollision || collision.rightSlopeCollision) ? "xy" : axis;

			if(area > 0.0f)
			{
				collisionInfos.push_back( CollisionInfo(area,axis,collidableTiles.tileIndices[i]));
				collisions.push_back( Collision(collision.minimumTranslation,area,
					collision.leftCollision,
					collision.rightCollision,
					collision.bottomCollision,
					collision.topCollision,
					collision.leftSlopeCollision,
					collision.rightSlopeCollision,
					collidableTiles.tileObjects[i].position.x, 
					collidableTiles.tileObjects[i].position.y,
					collision.flags,
					collision.centerInBetween,
					collidableTiles.tileIndices[i],collidableTiles.velocity));
			}
		}
	}

	std::sort(	collisions.begin(), collisions.end(), 
		[](Collision const & a, Collision const & b) -> bool
	{ return a.area > b.area; });

	int newcount = collisions.size() > 2 ? collisions.size() - 1 : collisions.size();
	for (int i = 0; i < newcount; i++)
	{
		if (collisions[i].leftCollision || collisions[i].rightCollision)
		{
			if (collisions[i].area > bestXCollision.area)
				bestXCollision = collisions[i];
		}
		else if (collisions[i].bottomCollision || collisions[i].topCollision)
		{
			if (collisions[i].area > bestYCollision.area)
				bestYCollision = collisions[i];
		}
		else if (collisions[i].leftSlopeCollision || collisions[i].rightSlopeCollision)
		{
			if (collisions[i].centerInBetween > bestXYCollision.centerInBetween)
				bestXYCollision = collisions[i];
		}
	}

	xcol = false;
	ycol = false;
	lxycol = xycol;
	xycol = 0;	
	scol = false; 

	Point ytile = Point(bestYCollision.x/world.level.layer[2].w_tileSize.x,bestYCollision.y/world.level.layer[2].w_tileSize.y);
	Point xtile = Point(bestXCollision.x/world.level.layer[2].w_tileSize.x,bestXCollision.y/world.level.layer[2].w_tileSize.y);

	if (bestXCollision.y != bestYCollision.y) //x
	{		
		if (bestXCollision.leftCollision || bestXCollision.rightCollision)
		{
			xcol = true;
			numCollisions++;
			if(bestXCollision.minimumTranslation.x > 0)
				bestXCollision.minimumTranslation.x = min(bestXCollision.minimumTranslation.x,4.0f);
			else
				bestXCollision.minimumTranslation.x = max(bestXCollision.minimumTranslation.x,-4.0f);
			position += bestXCollision.minimumTranslation;
			if((velocity.x > 0 && bestXCollision.leftCollision) || 	(velocity.x < 0 && bestXCollision.rightCollision))	
			{
				if(type == ET_SHELL) sound.PlaySfx(SOUND_THUD);
				direction *= -1;
				velocity.x *= -1.0f;
				if(world.level.layer[2].imageLayer[xtile.x][xtile.y].atileIndex == 240 && !world.level.layer[2].imageLayer[xtile.x][xtile.y].bounce)
				{
					world.level.layer[2].imageLayer[xtile.x][xtile.y].value--;
					world.level.layer[2].imageLayer[xtile.x][xtile.y].bounce = 10.0f;
					sound.PlaySfx(SOUND_COIN);
				}
			}
		}
	}

	if(bestXYCollision.centerInBetween && bestXYCollision.minimumTranslation.y > 0.0f &&
		(velocity.y <= 0) && 
		(lastPosition.y + tileheight * 0.5 >= (position.y + bestXYCollision.minimumTranslation.y))) //XY
	{
		if(bestXYCollision.flags & CT_0_TO_15)
			xycol = 2;
		else
			xycol = 1;
		numCollisions++;
		position += bestXYCollision.minimumTranslation;
		landing();
		velocity.y = 0.0f;
	}

	if (bestXCollision.x != bestYCollision.x && !xycol) //y
	{
		if (bestYCollision.topCollision && velocity.y <= 0) //ENEMY LAND
		{
			if(((bestYCollision.flags & CT_TOP) && !(bestYCollision.flags & CT_BOTTOM) &&
				!(bestYCollision.flags & CT_SPECIAL)) && 
				(lastPosition.y < (position.y + bestYCollision.minimumTranslation.y)))
				return;

			ycol = true;
			if((bestYCollision.flags & CT_SPECIAL) )
				scol = true;
			numCollisions++;
			position += bestYCollision.minimumTranslation;
			landing();
			velocity.y = 0.0f;
			if(dynamic)
			{
				dcol = true; 
				position.x += bestYCollision.velocity.x;
			}	
		}
		else if (bestYCollision.bottomCollision && velocity.y >= 0) //ENEMY HIT HEAD
		{
			ycol = true;
			numCollisions++;
			position += bestYCollision.minimumTranslation;
			velocity.y = 0.0f;
			currentState = CS_FALLING;
			sound.PlaySfx(SOUND_THUD);
			if(world.level.layer[2].imageLayer[ytile.x][ytile.y].atileIndex == 240 && !world.level.layer[2].imageLayer[ytile.x][ytile.y].bounce)
			{
				world.level.layer[2].imageLayer[ytile.x][ytile.y].value--;
				world.level.layer[2].imageLayer[ytile.x][ytile.y].bounce = 10.0f;
				sound.PlaySfx(SOUND_COIN);
			}
		}
		if(type == ET_KOOPA)
		{
			if(direction == 1 && (ytile.x + 1 < world.level.layer[2].t_cmapSize.x))
			{
				if(!world.level.layer[2].collisionLayer[ ytile.x + 1][ytile.y].isCollidable && position.x > bestYCollision.x) 
				{
					velocity.x *= -1;
					direction = -1;
				}
			}
			else if(direction == -1 && (ytile.x > 0))
			{
				if(!world.level.layer[2].collisionLayer[ ytile.x - 1][ytile.y].isCollidable && position.x < bestYCollision.x) 
				{
					velocity.x *= -1;
					direction = 1;
				}
			}
		}
	}
}

void Enemy::resolveFrame(int elapsedFrames)
{
	lastFrame = currentAtlasFrame;

	if (velocity.y < 0) currentState = CS_FALLING;

	int frame_y;
	int idle_frame;
	int move_start_frame;
	int move_end_frame;
	float frameDelay;

	switch(type)
	{
	case ET_KOOPA:
		frame_y = 3;
		idle_frame = 1;
		move_start_frame = 0;
		move_end_frame = 1;
		frameDelay = 80.0f;
		break;

	case ET_GOOMBA:
		frame_y = 4;
		idle_frame = 1;
		move_start_frame = 0;
		move_end_frame = 1;
		frameDelay = 80.0f;
		break;

	case ET_SHELL:
		frame_y = 3;
		idle_frame = 4;
		move_start_frame = 2;
		move_end_frame = 5;
		frameDelay = 40.0f;
		break;
	}

	if (currentState == CS_IDLE)
	{
		currentAtlasFrame.x = idle_frame;
		currentAtlasFrame.y = frame_y;
	}
	else
	{
		if (currentState == CS_MOVINGLEFT || currentState == CS_MOVINGRIGHT)
		{
			currentAtlasFrame.y = frame_y;

			currentFrame += elapsedFrames;
			if (currentFrame >= frameDelay)
			{
				currentAtlasFrame.x++;
				currentFrame = 0;
			}

			if (currentAtlasFrame.x > move_end_frame)
				currentAtlasFrame.x = move_start_frame;
		}
	}
}
