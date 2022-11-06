#ifndef __LOGO_H__
#define __LOGO_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"

struct SDL_Texture;

class Logo : public Module
{
public:

	Logo(bool startEnabled);

	// Destructor
	~Logo();

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
	SDL_Texture* img = nullptr;
};

#endif // __LOGO_H__