#ifndef __CHECKPOINT_H__
#define __CHECKPOINT_H__

#include "Entity.h"
#include "Point.h"
#include "Physics.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Checkpoint : public Entity
{
public:

	Checkpoint();
	virtual ~Checkpoint();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	bool isPicked;

	//DONE 4: Add a physics to an item
	PhysBody* pbody;
	SDL_Rect section;

	uint id;

private:

	SDL_Texture* texture;
	const char* texturePath;

	b2Vec2 velocity;

	int timeMov = 0;

	int width;
	int height;

	bool saveSpot;

};

#endif // __CHECKPOINT_H__
