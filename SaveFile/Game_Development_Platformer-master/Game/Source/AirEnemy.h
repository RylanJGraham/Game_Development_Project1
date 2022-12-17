#ifndef __AIRENEMY_H__
#define __AIRENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Physics.h"

struct SDL_Texture;

class AirEnemy : public Entity
{
public:

	AirEnemy();
	virtual ~AirEnemy();

	bool Awake();
	bool Start();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	bool CleanUp();

	void AirEnemy::DebugKeys();

	void MovementDirection(const iPoint& origin, const iPoint& destination);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void LoadAnimations();

	void SetPos(int x, int y);

	
public:

	int health;

	//Animation
	Animation Idle;
	Animation Movement, Damaged, Attack;
	Animation Death;
	Animation* currentAnim = nullptr;

	//Physics
	PhysBody* pbody;

	bool alive;
	bool idle;
	bool ishurt;
	bool isGrounded;

	//SFX
	uint SFX = 0;
	uint attackSFX = 0;
	uint deathSFX = 0;
	uint damagedSFX = 0;

private:

	bool stairs;
	int remainingJumpSteps;


	SDL_RendererFlip flipped = SDL_FLIP_NONE;


	//Texture
	SDL_Texture* texture;
	const char* texturePath;

	int width;
	int height;

	b2Vec2 velocity;
	b2Vec2 startPos;

	//debug pathfinding
	iPoint origin;
	bool originSelected = false;
	uint refreshPathTime;

};

#endif // __AIRENEMY_H__

