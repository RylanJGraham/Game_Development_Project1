#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Map.h"
#include "Physics.h"
#include "PathFinding.h"
#include "FadeToBlack.h"
#include "Ending.h"
#include "UI.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	LOG("STARTING SCENE");

	batTilePath = app->configNode.child("scene").child("pathfinding").attribute("batPathTile").as_string();
	origintexturePath = app->configNode.child("scene").child("originTexture").attribute("origintexturePath").as_string();
	// Texture to highligh mouse position 
	batTilePathTex = app->tex->Load(batTilePath);
	// Texture to show path origin 
	originTex = app->tex->Load(origintexturePath);

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = app->configNode.child("scene").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = app->configNode.child("scene").child("player");

	groundenemy = (GroundEnemy*)app->entityManager->CreateEntity(EntityType::GROUNDENEMY);
	groundenemy->parameters = app->configNode.child("scene").child("groundEnemy");

	airenemy = (AirEnemy*)app->entityManager->CreateEntity(EntityType::AIRENEMY);
	airenemy->parameters = app->configNode.child("scene").child("airEnemy");

	app->physics->Enable();
	app->pathfinding->Enable();
	app->entityManager->Enable();
	app->map->Enable();
	app->ui->Enable();
	app->fonts->Enable();
	LOG("--STARTS GAME SCENE--");
	app->physics->debug = false;
	app->scene->airenemy->health = 2;

	app->render->camera.x = 0;
	app->render->camera.y = -498;

	//img = app->tex->Load("Assets/Textures/test.png");
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");
	
	// L03: DONE: Load map
	app->map->Load();

	// L12 Create walkability map
	if (app->map->Load()) {
		int w, h;
		uchar* data = NULL;

		bool retWalkMap = app->map->CreateWalkabilityMap(w, h, &data);
		if (retWalkMap) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	SString title("Game Name");

	// L04: DONE 7: Set the window title with map/tileset info
	/*SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());*/

	app->win->SetTitle(title.GetString());

	ResetScene();

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
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN) {
		app->physics->debug = !app->physics->debug;
	}

	if (!groundenemy->alive) {

	}

#pragma endregion DEBUG_KEYS

	app->render->camera.x = (-player->position.x)*1.5f;


	// Draw map
	app->map->Draw();

	//Blit UI
	app->ui->BlitItems();
	app->ui->BlitLives();
	app->ui->BlitFPS();

	if (app->physics->debug) {
		app->ui->BlitPlayerXPos();
		app->ui->BlitPlayerYPos();
		app->ui->BlitBatLives();
		app->ui->BlitPigLives();
		app->ui->BlitAverageFPS();
		app->ui->BlitDT();
		app->ui->BlitTimeSinceStart();
		app->ui->BlitFrameCount();
	}


	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing GAME SCENE");

	app->render->camera.x = 0;
	app->render->camera.y = 0;
	if (player->lives == 0) {
		if (player) { app->entityManager->DestroyEntity(player); }
		if (airenemy) { app->entityManager->DestroyEntity(airenemy); }
		if (groundenemy) { app->entityManager->DestroyEntity(groundenemy); }
		if (item) { app->entityManager->DestroyEntity(item); }
	}

	app->entityManager->Disable();
	app->physics->Disable();

	return true;
}

void Scene::ResetScene() {

	/*app->audio->PlayMusic("Assets/Audio/Music/song1.ogg", 1.0f);*/
	player->ResetPlayerPos();
	player->lives = 3;

	//coin->ResetCoin();
}


bool Scene::LoadState(pugi::xml_node& data)
{
	// Load previous saved player position
	b2Vec2 playerPos = { data.child("playerPosition").attribute("x").as_float(), data.child("playerPosition").attribute("y").as_float() };
	app->scene->player->pbody->body->SetTransform(playerPos, 0);

	// Load previous saved cameraFix & cameraFix2 parameters
	//app->scene->cameraFix = data.child("cameraIsFix").attribute("value").as_bool();
	//app->scene->cameraFix2 = data.child("cameraIsFix2").attribute("value").as_bool();

	//Load previous saved player number of lives
	app->scene->player->lives = data.child("playerLives").attribute("playerLives").as_int();


	// Load previous saved slime position
	b2Vec2 groundenemyPos = { data.child("groundenemyPosition").attribute("x").as_float(), data.child("groundenemyPosition").attribute("y").as_float() };
	app->scene->groundenemy->pbody->body->SetTransform(groundenemyPos, 0);

	//Load previous saved slime number of lives
	app->scene->groundenemy->hp = data.child("groundenemyLives").attribute("groundenemyLives").as_int();

	b2Vec2 airenemyPos = { data.child("airenemyPosition").attribute("x").as_float(), data.child("airenemyPosition").attribute("y").as_float() };
	app->scene->airenemy->pbody->body->SetTransform(groundenemyPos, 0);

	//Load previous saved slime number of lives
	app->scene->airenemy->health = data.child("airenemyLives").attribute("airenemyLives").as_int();

	return true;
}

bool Scene::SaveState(pugi::xml_node& data)
{
	// Save current player position
	pugi::xml_node playerPos = data.append_child("playerPosition");
	playerPos.append_attribute("x") = app->scene->player->pbody->body->GetTransform().p.x;
	playerPos.append_attribute("y") = app->scene->player->pbody->body->GetTransform().p.y;

	// Save cameraFix parameter
	//pugi::xml_node cameraIsFix = data.append_child("cameraIsFix");
	//cameraIsFix.append_attribute("value") = app->scene->cameraFix;

	//// Save cameraFix2 parameter
	//pugi::xml_node cameraIsFix2 = data.append_child("cameraIsFix2");
	//cameraIsFix2.append_attribute("value") = app->scene->cameraFix2;

	// Save current player number of lives
	pugi::xml_node playerLives = data.append_child("playerLives");
	playerLives.append_attribute("playerLives") = app->scene->player->lives;


	// Save current airenemy position
	pugi::xml_node airenemyPos = data.append_child("airenemyPosition");
	airenemyPos.append_attribute("x") = app->scene->airenemy->pbody->body->GetTransform().p.x;
	airenemyPos.append_attribute("y") = app->scene->airenemy->pbody->body->GetTransform().p.y;


	// Save current groundenemy position
	pugi::xml_node groundenemyPos = data.append_child("groundenemyPosition");
	groundenemyPos.append_attribute("x") = app->scene->groundenemy->pbody->body->GetTransform().p.x;
	groundenemyPos.append_attribute("y") = app->scene->groundenemy->pbody->body->GetTransform().p.y;


	// Save current air number of lives
	pugi::xml_node airenemyLives = data.append_child("airenemyLives");
	airenemyLives.append_attribute("airenemyLives") = app->scene->airenemy->health;

	// Save current ground number of lives
	pugi::xml_node groundenemyLives = data.append_child("groundenemyLives");
	groundenemyLives.append_attribute("airenemyLives") = app->scene->groundenemy->hp;

	return true;
}