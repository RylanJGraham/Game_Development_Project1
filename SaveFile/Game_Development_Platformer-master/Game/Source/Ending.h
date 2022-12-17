#ifndef __ENDING_H__
#define __ENDING_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"

struct SDL_Texture;

class Ending : public Module
{
public:

	Ending(bool startEnabled);

	// Destructor
	virtual ~Ending();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

private:
	uint selectSFX = 0;
	SDL_Texture* img = nullptr;
};

#endif // __ENDING_H__