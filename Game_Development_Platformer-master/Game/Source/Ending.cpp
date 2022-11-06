#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Ending.h"
#include "EntityManager.h"
#include "Map.h"
#include "Scene.h"
#include "FadeToBlack.h"

#include "Defs.h"
#include "Log.h"

Ending::Ending(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene");
}

// Destructor
Ending::~Ending()
{}

// Called before render is available
bool Ending::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	/*for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}*/

	return ret;
}

// Called before the first frame
bool Ending::Start()
{
	LOG("--STARTS ENDING SCENE--");
	/*app->entityManager->Disable();
	app->scene->Disable();
	app->physics->Disable();
	app->map->Disable();*/
	app->render->camera.x = 0;
	app->render->camera.y = 0;
	img = app->tex->Load("Assets/Textures/endscreen.png");
	/*app->scene->player->active = true;
	app->scene->player.rese*/
	selectSFX = app->audio->LoadFx("Assets/Audio/Fx/swordswing.wav");

	//app->scene->player->godMode = true;

	return true;
}

// Called each loop iteration
bool Ending::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Ending::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		LOG("PASA A GAME SCENE");
		app->fade->FadeBlack(this, (Module*)app->scene, 90);
		//app->scene->cameraFix = false;
		/*app->audio->PlayFx(FX);*/
	}

	return true;
}

// Called each loop iteration
bool Ending::PostUpdate()
{
	app->render->DrawTexture(img, 0, 0);

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		return false;
	}
		

	return true;
}

// Called before quitting
bool Ending::CleanUp()
{
	if (img != nullptr) {
		app->tex->UnLoad(img);
	}

	return true;
}