#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Ending.h"
#include "EntityManager.h"
#include "Map.h"
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
	img = app->tex->Load("Assets/Textures/DeathScreen.png");


	selectSFX = app->audio->LoadFx("Assets/Audio/Fx/swordswing.wav");

	app->render->camera.x = 0;
	app->render->camera.y = 0;

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
<<<<<<< Updated upstream
	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y += 1;

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y -= 1;

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x += 1;

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x -= 1;

		//app->render->DrawTexture(img, 380, 100); // Placeholder not needed any more

		// Draw map
	app->map->Draw();
=======
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		app->fade->FadeBlack(this, (Module*)app->titleScreen, 90);
		app->audio->PlayFx(selectSFX);
	}
>>>>>>> Stashed changes

	return true;
}

// Called each loop iteration
bool Ending::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Ending::CleanUp()
{
	LOG("Freeing scene");

	return true;
}