#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Map.h"
#include "GroundEnemy.h"
#include "AirEnemy.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene");
	active = false;
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");

	groundenemy = (GroundEnemy*)app->entityManager->CreateEntity(EntityType::GROUNDENEMY);
	groundenemy->parameters = config.child("groundEnemy");

	airenemy = (AirEnemy*)app->entityManager->CreateEntity(EntityType::AIRENEMY);
	airenemy->parameters = config.child("airEnemy");

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	app->entityManager->Enable();

	app->render->camera.x = 0;
	app->render->camera.y = -498;

	//img = app->tex->Load("Assets/Textures/test.png");
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");
	
	// L03: DONE: Load map
	app->map->Load();

	// L04: DONE 7: Set the window title with map/tileset info
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	app->win->SetTitle(title.GetString());

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	int scale = app->win->GetScale();
	int camSpeed = 10 / scale;

	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
#pragma region DEBUG_KEYS

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		player->SetPos(112, 511);
	}

	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {

	}

	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		player->SetPos(112, 511);
	}

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		app->SaveGameRequest();
		player->savedPosition.x = player->position.x;
		player->savedPosition.y = player->position.y;

	}

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		app->LoadGameRequest();
		player->SetPos(player->savedPosition.x + 46, player->savedPosition.y + 60);
	}

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y += camSpeed;

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y -= camSpeed;

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x += camSpeed;

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x -= camSpeed;
	// F9: View colliders / logic
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		app->physics->debug = !app->physics->debug;
	}

#pragma endregion DEBUG_KEYS

	app->render->camera.x = (-player->position.x)*1.5f;


	// Draw map
	app->map->Draw();

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		return false;

	return true;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing GAME SCENE");

	app->map->Disable();
	app->entityManager->Disable();
	app->physics->Disable();

	return true;
}
