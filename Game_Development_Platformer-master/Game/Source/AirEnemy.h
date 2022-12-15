#ifndef __AIRENEMY_H__
#define __AIRENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class AirEnemy : public Entity
{
public:

	AirEnemy();
	virtual ~AirEnemy();

	bool Awake();
	bool Start();
	bool Update();
	bool PostUpdate();
	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);
	void AirEnemy::DebugKeys();
	void MovementDirection(const iPoint& origin, const iPoint& destination);
	void LoadAnimations();
	void SetPos(int x, int y);

public:

private:

	bool alive;
	bool idle;
	bool ishurt;
	bool isGrounded;
	bool stairs;
	int remainingJumpSteps;
	uint health;

	SDL_RendererFlip flipped = SDL_FLIP_NONE;

	//SFX
	uint SFX = 0;
	uint attackSFX = 0;
	uint deathSFX = 0;
	uint damagedSFX = 0;

	//Physics
	PhysBody* aebody;

	//Texture
	SDL_Texture* texture;
	const char* texturePath;

	//Animation
	Animation Idle;
	Animation Movement, Damaged, Attack;
	Animation Death;
	Animation* currentAnim = nullptr;

	b2Vec2 velocity;
	b2Vec2 startPos;


	//debug pathfinding
	iPoint origin;
	bool originSelected = false;
	uint refreshPathTime;


};

#endif // __AIRENEMY_H__

