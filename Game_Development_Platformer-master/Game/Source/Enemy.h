#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();

	bool Awake();
	bool Start();
	bool Update();
	bool PostUpdate();
	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);
	void Enemy::DebugKeys();
	void LoadAnimations();
	void SetPos(int x, int y);

public:

private:

	bool alive;
	bool idle, leftID;
	bool isGrounded;
	bool stairs;
	int remainingJumpSteps;

	//SFX
	uint SFX = 0;
	uint attackSFX = 0;
	uint deathSFX = 0;

	//Physics
	PhysBody* ebody;

	//Texture
	SDL_Texture* texture;
	const char* texturePath;

	//Animation
	Animation IdleL, IdleR;	// jump?
	Animation RunL, RunR, JumpL, JumpR;
	Animation death, iddledeathanim;
	Animation* currentAnim = nullptr;


};

#endif // __ENEMY_H__
