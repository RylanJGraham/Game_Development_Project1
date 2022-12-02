#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();	
	virtual ~Player();

	bool Awake();
	bool Start();
	bool Update();	
	bool PostUpdate();
	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);
	void Player::DebugKeys();
	void LoadAnimations();
	void SetPos(int x, int y);

public:

	bool isGrounded;

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

	//Physics
	PhysBody* pbody;
	PhysBody* hitbox;

	//Texture
	SDL_Texture* texture;
	const char* texturePath;

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