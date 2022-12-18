#ifndef __GROUNDENEMY_H__
#define __GROUNDENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "Physics.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class GroundEnemy : public Entity
{
public:

	GroundEnemy();
	virtual ~GroundEnemy();

	bool Awake();
	bool Start();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	bool CleanUp();

	void MovementDirection(const iPoint& origin, const iPoint& destination);

	void OnCollision(PhysBody* physA, PhysBody* physB);
	void GroundEnemy::DebugKeys();
	void LoadAnimations();

	void ResetGroundEnemy();

public:

	//Animation
	Animation AttackL, AttackR;	// jump?
	Animation RunL, RunR, JumpL, JumpR;
	Animation HitL, HitR;
	Animation DeathL, DeathR;
	Animation* currentAnim = nullptr;

	//SFX
	uint SFX = 0;
	uint attackSFX = 0;
	uint deathSFX = 0;

	//Physics
	PhysBody* pbody;

	int hp = 3;

	PhysBody* hitbox;
	b2Vec2 hitboxPos;
	bool isGrounded;
	bool alive;

private:

	bool leftID;

	bool stairs;
	bool isHurt;
	int remainingJumpSteps;

	b2Vec2 velocity;
	b2Vec2 startPos;


	int width;
	int height;

	//Texture
	SDL_Texture* texture;
	const char* texturePath;

	iPoint origin;
	bool originSelected = false;
	uint refreshPathTime;


};

#endif // __ENEMY_H__
