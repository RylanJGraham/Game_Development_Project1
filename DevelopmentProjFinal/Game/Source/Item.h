#ifndef __ITEM_H__
#define __ITEM_H__

#include "Entity.h"
#include "Point.h"
#include "Physics.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Item : public Entity
{
public:

	Item();
	virtual ~Item();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	bool isPicked = false;

	//TODO 4: Add a physics to an item
	PhysBody* pbody;

private:

	SDL_Texture* texture;
	const char* texturePath;


	b2Vec2 velocity;

	int timeMov = 0;

	int width;
	int height;

};

#endif // __ITEM_H__