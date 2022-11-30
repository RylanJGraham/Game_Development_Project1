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
	void LoadAnimations();
	void SetPos(int x, int y);

public:

private:

	bool alive;
	bool idle;
	bool isGrounded;
	bool stairs;
	int remainingJumpSteps;
	int health;

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


};

#endif // __AIRENEMY_H__

