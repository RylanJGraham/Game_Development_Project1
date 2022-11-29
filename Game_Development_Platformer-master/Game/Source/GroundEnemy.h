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
	bool idle, leftID;
	bool isGrounded;
	bool stairs;
	int remainingJumpSteps;

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
	Animation IdleL, IdleR;	// jump?
	Animation RunL, RunR, JumpL, JumpR;
	Animation death, iddledeathanim;
	Animation* currentAnim = nullptr;


};

#endif // __ENEMY_H__
