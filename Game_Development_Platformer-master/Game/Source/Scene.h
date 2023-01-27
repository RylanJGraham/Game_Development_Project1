#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"
#include "GroundEnemy.h"
#include "AirEnemy.h"
#include "GuiButton.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

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

	void ResetScene();

	bool OnGuiMouseClickEvent(GuiControl* control);

	bool LoadState(pugi::xml_node&);
	bool SaveState(pugi::xml_node&);

public:
	//L02: DONE 3: Declare a Player attribute 
	Player* player;
	GroundEnemy* groundenemy;
	AirEnemy* airenemy;
	Item* item;

	bool playing = false;

	bool capTo30fps = false;

	SDL_Texture* batTilePathTex = nullptr;
	const char* batTilePath = nullptr;

	SDL_Texture* originTex = nullptr;
	const char* origintexturePath = nullptr;

private:
	SDL_Texture* img;
	SDL_Rect rectCamera;

	iPoint startPosition;
	iPoint origin;
	bool originSelected = false;

};

#endif // __SCENE_H__