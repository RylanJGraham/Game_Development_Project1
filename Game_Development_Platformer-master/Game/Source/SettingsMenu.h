#ifndef __TITLE_H__
#define __TITLE_H__

#include "Module.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class SettingsMenu : public Module
{
public:

	SettingsMenu(bool startEnabled);

	// Destructor
	virtual ~SettingsMenu();

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
	uint selectSFX = 0;

private:
	SDL_Texture* img = nullptr;

};

#endif // !__TITLE_H__

