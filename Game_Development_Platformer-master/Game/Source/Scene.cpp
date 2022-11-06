#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Map.h"

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


	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	app->entityManager->Enable();

	app->render->camera.x = 0;
	app->render->camera.y = -448;

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

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y += camSpeed;

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y -= camSpeed;

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x += camSpeed;

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x -= camSpeed;

#pragma endregion DEBUG_KEYS


	//Left
 //	if (player->position.x + app->render->camera.x < app->render->camera.w / (3))
	//	app->render->camera.x = (-player->position.x + (app->render->camera.w / 3)) * scale;

	////Right
	//else if(player->position.x + app->render->camera.x > app->render->camera.w / (1.8))
	//	app->render->camera.x = (-player->position.x + (app->render->camera.w / 1.8)) * scale;

	////Up
	//if (player->position.y + app->render->camera.y < app->render->camera.h / (3))
	//	app->render->camera.y = (-player->position.y + (app->render->camera.h / 3) * scale);

	////Down
	//else if (player->position.y + app->render->camera.y > app->render->camera.h / (1.8))
	//	app->render->camera.y = (-player->position.y + (app->render->camera.h / 1.8) * scale);

	if (app->render->camera.x > 0)
		app->render->camera.x = 0;

	if (app->render->camera.y < -448)
		app->render->camera.y = -448;


	// Draw map
	app->map->Draw();

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	int scale = app->win->GetScale();
	//Draw Camera
	rectCamera.x = -app->render->camera.x + (app->render->camera.w / (3));
	rectCamera.y = -app->render->camera.y + (app->render->camera.h / (1.8));
	rectCamera.w = app->render->camera.w / (3);
	rectCamera.h = app->render->camera.h / (1.8);
	app->render->DrawRectangle(rectCamera, 0, 255, 0, 255, false, true);



	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		return false;


	return true;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
