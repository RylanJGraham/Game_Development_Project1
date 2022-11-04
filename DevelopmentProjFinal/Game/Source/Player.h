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

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void ResetPlayerPos();

	bool PreUpdate();

	bool PostUpdate();

public:

	bool dead = false;
	bool godMode = false;
	PhysBody* pbody;

private:

	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	const char* texturePath;
	Animation IdleL;
	Animation RunL;
	Animation JumpL;
	Animation IdleR;
	Animation RunR;
	Animation JumpR;
	Animation* currentAnim = nullptr;

	b2Vec2 startPos;

	int width;
	int height;

	bool isJumping = false;
	bool FacingFront = true;
	b2Vec2 vel;
	float Jump = 5;

	// L07 DONE 5: Add physics to the player - declare a Physics body


};

#endif // __PLAYER_H__