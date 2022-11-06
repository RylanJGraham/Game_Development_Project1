#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene(bool startEnabled);

	// Destructor
	virtual ~Scene();

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

	SDL_Texture* NumChests1 = nullptr;
	SDL_Texture* NumChests2 = nullptr;
	SDL_Texture* NumChests3 = nullptr;
	int numChests;
	//L02: DONE 3: Declare a Player attribute 
	Player* player;
	int movementmatchx;
	int movementmatchy;

private:
	SDL_Texture* img;
	SDL_Rect rectCamera;

};

#endif // __SCENE_H__