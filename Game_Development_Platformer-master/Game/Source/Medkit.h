#ifndef __MEDKIT_H__
#define __MEDKIT_H__

#include "Entity.h"
#include "Point.h"
#include "Physics.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Medkit : public Entity
{
public:

	Medkit();
	virtual ~Medkit();

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

private:

	SDL_Texture* texture;
	const char* texturePath;

	b2Vec2 velocity;

	int timeMov = 0;

	int width;
	int height;

};

#endif // __MEDKIT_H__
