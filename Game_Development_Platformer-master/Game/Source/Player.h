#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Physics.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();	
	virtual ~Player();

	bool Awake();
	bool Start();
	bool PreUpdate();
	bool Update();	
	bool PostUpdate();
	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);
	void Player::DebugKeys();
	void LoadAnimations();
	void SetPos(int x, int y);
	void ResetPlayerPos();


public:

	int lives;

	bool isGrounded;


	//Physics
	PhysBody* pbody;

	PhysBody* hitbox;
	b2Vec2 hitboxPos;

private:
	

	uint jumpSFX = 0;
	uint chestopenSFX = 0;
	uint attackCooldownMax;
	uint attackCooldown;
	bool alive;
	bool idle, leftID;
	bool stairs;
	bool isAttacking;
	int remainingJumpSteps;


	bool godMode;

	int width;
	int height;

	//Texture
	SDL_Texture* texture;
	const char* texturePath;

	b2Vec2 velocity;
	b2Vec2 startPos;

	//Animation
	Animation IdleL, IdleR;	// jump?
	Animation RunL, RunR, JumpL, JumpR;
	Animation DeathL, DeathR;
	Animation AttackL, AttackR;
	Animation* currentAnim = nullptr;

	//FX
	int pickCoinFxId;
	
};

#endif // __PLAYER_H__