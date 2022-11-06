#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Title.h"
#include "EntityManager.h"
#include "Map.h"
#include "FadeToBlack.h"
#include "Defs.h"
#include "Log.h"

Title::Title(bool startEnabled) : Module(startEnabled)
{
	name.Create("Title");
	active = false;
}

// Destructor
Title::~Title()
{}

// Called before render is available
bool Title::Awake(pugi::xml_node& config)
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
bool Title::Start()
{
	//img = app->tex->Load("Assets/Textures/test.png");
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	// L03: DONE: Load map
	app->map->Load();

	// L04: DONE 7: Set the window title with map/tileset info
	/*SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());*/

	/*app->win->SetTitle(title.GetString());*/

	return true;
}

// Called each loop iteration
bool Title::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Title::Update(float dt)
{
	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	/*if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
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
		app->render->camera.x -= 1;*/

	//app->render->DrawTexture(img, 380, 100); // Placeholder not needed any more

	// Draw map
	app->map->Draw();

	return true;
}

// Called each loop iteration
bool Title::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	
	// ENTER: BEGIN
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		app->fade->FadeBlack(this, (Module*)app->scene, 100);
	}

	return ret;
}

// Called before quitting
bool Title::CleanUp()
{
	LOG("Freeing scene");
	//app->tex->UnLoad();

	return true;
}