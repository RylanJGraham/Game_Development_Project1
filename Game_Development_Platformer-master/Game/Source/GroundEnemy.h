#ifndef __GROUNDENEMY_H__
#define __GROUNDENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class GroundEnemy : public Entity
{
public:

	GroundEnemy();
	virtual ~GroundEnemy();

	bool Awake();
	bool Start();
	bool Update();
	bool PostUpdate();
	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);
	void GroundEnemy::DebugKeys();
	void LoadAnimations();
	void SetPos(int x, int y);

public:

private:

	bool alive;
	bool leftID;
	bool isGrounded;
	bool stairs;
	bool isHurt;
	int remainingJumpSteps;
	uint hp;

	//SFX
	uint SFX = 0;
	uint attackSFX = 0;
	uint deathSFX = 0;

	//Physics
	PhysBody* gebody;

	//Texture
	SDL_Texture* texture;
	const char* texturePath;

	//Animation
	Animation AttackL, AttackR;	// jump?
	Animation RunL, RunR, JumpL, JumpR;
	Animation HitL, HitR;
	Animation DeathL, DeathR;
	Animation* currentAnim = nullptr;


};

#endif // __ENEMY_H__
