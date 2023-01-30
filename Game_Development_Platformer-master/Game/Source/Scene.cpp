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
#include "GuiManager.h"
#include "Title.h"
#include "Audio.h"
#include "Fonts.h"

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

	imgPausePath = app->configNode.child("scene").child("imgPause").attribute("imgPausePath").as_string();

	selectSFX = app->audio->LoadFx("Assets/Audio/Fx/swordswing.wav");

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	int i = 0;
	for (pugi::xml_node itemNode = app->configNode.child("scene").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
		item->id = i;
		i++;
	}

	i = 0;
	for (pugi::xml_node medkitNode = app->configNode.child("scene").child("medkit"); medkitNode; medkitNode = medkitNode.next_sibling("medkit"))
	{
		medkit = (Medkit*)app->entityManager->CreateEntity(EntityType::MEDKIT);
		medkit->parameters = medkitNode;
		medkit->id = i;
		i++;
	}

	i = 0;
	for (pugi::xml_node checkpointNode = app->configNode.child("scene").child("checkpoint"); checkpointNode; checkpointNode = checkpointNode.next_sibling("checkpoint"))
	{
		checkpoint = (Checkpoint*)app->entityManager->CreateEntity(EntityType::CHECKPOINT);
		checkpoint->parameters = checkpointNode;
		checkpoint->id = i;
		i++;
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
	exitGame = false;
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

	app->win->SetTitle(title.GetString());

	img_pause = app->tex->Load(imgPausePath);

	// L15: TODO 2: Declare a GUI Button and create it using the GuiManager
	uint w, h;
	app->win->GetWindowSize(w, h);
	resumeButton14 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 14, "resume", 7, { ((int)w / 2) - 490, (int(h) / 2) - 280, 227, 83 }, this);
	backToTitleButton15 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 15, "back to title", 14, { ((int)w / 2) - 340, (int(h) / 2) - 150, 100, 83 }, this);
	exitButton16 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 16, "exit", 5, { ((int)w / 2) - 200, (int(h) / 2) - 380, 227, 83 }, this);
	closeButton17 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 17, "close", 6, { ((int)w / 2) - 490, (int(h) / 2) - 150, 100, 83 }, this);
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

	uint w, h;
	app->win->GetWindowSize(w, h);

	app->input->GetMousePosition(mouseX, mouseY);

	if (continueGame == true)
	{
		app->LoadGameRequest();
		app->audio->PlayFx(selectSFX);
		continueGame = false;
	}

	int scale = app->win->GetScale();
	int camSpeed = (0 / scale)*dt;

	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
#pragma region DEBUG_KEYS

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		app->fade->FadeBlack((Module*)app->scene, (Module*)app->scene, 60 * (16.0f / dt));
	}

	if (app->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
	{
		app->render->viewGUIbounds = !app->render->viewGUIbounds;
	}

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		if (!gamePaused) {
			previousCameraPos.x = app->render->camera.x;
			previousCameraPos.y = app->render->camera.y;
		}
		else {
			app->render->camera.x = previousCameraPos.x;
			app->render->camera.y = previousCameraPos.y;
		}
		gamePaused = !gamePaused;
		Mix_PauseMusic();
	}


	if (gamePaused != true)
	{
		Mix_ResumeMusic();
		if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
				
		}

		if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		app->fade->FadeBlack((Module*)app->scene, (Module*)app->scene, 60 * (16.0f / dt));
		}

		if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		app->SaveGameRequest();
		saveCounter = 2 * dt;
		//player->savedPosition.x = player->position.x;
		//player->savedPosition.y = player->position.y;

		}

		if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		app->LoadGameRequest();
		loadCounter = 2 * dt;
		//player->SetPos(player->savedPosition.x + 46, player->savedPosition.y + 60);
		}

		//if (app->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		//{
		//	app->render->limitFPS = !app->render->limitFPS;
		//	app->audio->PlayFx(selectSFX);
		//}

		//if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
		//{
		//capTo30fps = !capTo30fps;
		//}
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
	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN) {
		//FPS control
		if (app->maxFrameDuration != 33) {
			app->maxFrameDuration = 33;
			app->vsync = true;
		}
		else if (app->maxFrameDuration == 33) {
			app->maxFrameDuration = 16;
			app->vsync = false;
		}
	}

#pragma endregion DEBUG_KEYS

	app->render->camera.x = (-player->position.x)*1.5f;


	// Draw map
	app->map->Draw();

	app->guiManager->Draw();

	//Blit UI
	app->ui->BlitItems();
	app->ui->BlitLives();
	app->ui->BlitFPS();

	if (app->physics->debug) {
		app->ui->BlitPlayerXPos();
		app->ui->BlitPlayerYPos();
	}

	if (loadCounter > 0) {
		app->ui->BlitGameLoaded();
		loadCounter--;
	}

	if (saveCounter > 0) {
		app->ui->BlitGameSaved();
		saveCounter--;
	}

	resumeButton14->state = GuiControlState::DISABLED;
	backToTitleButton15->state = GuiControlState::DISABLED;
	exitButton16->state = GuiControlState::DISABLED;
	closeButton17->state = GuiControlState::DISABLED;

	if (gamePaused == true)
	{
		// Display pause menu
		LOG("PauseMenuDrawing");
		app->render->camera.x = 0;
		app->render->camera.y = 0;
		app->render->DrawTexture(img_pause, 0, 0, NULL);
			
		resumeButton14->state = GuiControlState::NORMAL;
		backToTitleButton15->state = GuiControlState::NORMAL;
		exitButton16->state = GuiControlState::NORMAL;
		closeButton17->state = GuiControlState::NORMAL;

		if ((mouseX > resumeButton14->bounds.x) && (mouseX < (resumeButton14->bounds.x + resumeButton14->bounds.w)) &&
			(mouseY > resumeButton14->bounds.y) && (mouseY < (resumeButton14->bounds.y + resumeButton14->bounds.h)))
		{
			resumeButton14->state = GuiControlState::FOCUSED;
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
			{
				resumeButton14->state = GuiControlState::PRESSED;
				LOG("Leaving pause Menu Pressed");
				gamePaused = !gamePaused;
				app->audio->PlayFx(selectSFX);
			}
		}

		if ((mouseX > backToTitleButton15->bounds.x) && (mouseX < (backToTitleButton15->bounds.x + backToTitleButton15->bounds.w)) &&
			(mouseY > backToTitleButton15->bounds.y) && (mouseY < (backToTitleButton15->bounds.y + backToTitleButton15->bounds.h)))
		{
			backToTitleButton15->state = GuiControlState::FOCUSED;
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
			{
				backToTitleButton15->state = GuiControlState::PRESSED;
				LOG("Leaving pause Menu Pressed");
				gamePaused = !gamePaused;
				exitGame = !exitGame;
				app->audio->PlayFx(selectSFX);
			}
		}

		if ((mouseX > exitButton16->bounds.x) && (mouseX < (exitButton16->bounds.x + exitButton16->bounds.w)) &&
			(mouseY > exitButton16->bounds.y) && (mouseY < (exitButton16->bounds.y + exitButton16->bounds.h)))
		{
			exitButton16->state = GuiControlState::FOCUSED;
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
			{
				exitButton16->state = GuiControlState::PRESSED;
				LOG("Leaving Game Pressed");
				gamePaused = !gamePaused;
				app->audio->PlayFx(selectSFX);
			}
		}

		if ((mouseX > closeButton17->bounds.x) && (mouseX < (closeButton17->bounds.x + closeButton17->bounds.w)) &&
			(mouseY > closeButton17->bounds.y) && (mouseY < (closeButton17->bounds.y + closeButton17->bounds.h)))
		{
			closeButton17->state = GuiControlState::FOCUSED;
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
			{
				closeButton17->state = GuiControlState::PRESSED;
				LOG("Leaving Game Pressed");
				app->fade->FadeBlack(this, (Module*)app->titleScreen, 90 * (16.0f / app->dt));
				app->audio->PlayFx(app->titleScreen->startSFX);
				app->audio->PlayFx(selectSFX);
			}
		}

	

	}


	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (exitGame == true)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing GAME SCENE");

	app->render->camera.x = 0;
	app->render->camera.y = 0;
	//if (player != nullptr) {
	//	if (player->lives == 0) 

		if (player) { app->entityManager->DestroyEntity(player); }		
		if (airenemy) { app->entityManager->DestroyEntity(airenemy); }
		if (groundenemy) { app->entityManager->DestroyEntity(groundenemy); }
		if (item) { app->entityManager->DestroyEntity(item); }


	gamePaused = false;
	Mix_ResumeMusic();

	app->tex->UnLoad(img_pause);

	if (resumeButton14 != nullptr)
		resumeButton14->state = GuiControlState::DISABLED;
	if (backToTitleButton15 != nullptr)
		backToTitleButton15->state = GuiControlState::DISABLED;
	if (exitButton16 != nullptr)
		exitButton16->state = GuiControlState::DISABLED;
	if (closeButton17 != nullptr)
		closeButton17->state = GuiControlState::DISABLED;

	app->entityManager->Disable();
	app->physics->Disable();
	app->map->Disable();

	return true;
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: TODO 5: Implement the OnGuiMouseClickEvent method
	switch (control->id)
	{
	case 14:
		gamePaused = !gamePaused;
		app->audio->PlayFx(app->titleScreen->menuSelectionSFX);
		break;

	case 15:
		app->fade->FadeBlack(this, (Module*)app->titleScreen, 90 * (16.0f / app->dt));
		app->audio->PlayFx(app->titleScreen->startSFX);
		break;

	case 16:
		exitGame = !exitGame;
		app->audio->PlayFx(app->titleScreen->menuSelectionSFX);
		break;

	default:
		break;
	}

	return true;
}


void Scene::ResetScene() {

	app->audio->PlayMusic("Assets/Audio/Music/medieval.ogg", 1.0f);
	player->ResetPlayerPos();
	player->lives = player->baseLives;

	//coin->ResetCoin();
}


bool Scene::LoadState(pugi::xml_node& data)
{
	// Load previous saved player position
	b2Vec2 playerPos = { data.child("player").attribute("x").as_float(), data.child("player").attribute("y").as_float() };
	player->pbody->body->SetTransform(playerPos, 0);

	//Load previous saved player number of lives
	player->lives = data.child("player").attribute("lives").as_int();
	player->Items = data.child("player").attribute("items").as_int();

	if (groundenemy != nullptr) {
		b2Vec2 groundenemyPos = { data.child("groundenemyPosition").attribute("x").as_float(), data.child("groundenemyPosition").attribute("y").as_float() };
		groundenemy->pbody->body->SetTransform(groundenemyPos, 0);
		groundenemy->hp = data.child("groundenemyLives").attribute("groundenemyLives").as_int();
	}

	if (airenemy != nullptr) {
		b2Vec2 airenemyPos = { data.child("airenemyPosition").attribute("x").as_float(), data.child("airenemyPosition").attribute("y").as_float() };
		airenemy->pbody->body->SetTransform(airenemyPos, 0);
		airenemy->health = data.child("airenemyLives").attribute("airenemyLives").as_int();
	}

	return true;
}

bool Scene::SaveState(pugi::xml_node& data)
{
	// Save current player position
	pugi::xml_node playerPos = data.append_child("player");
	playerPos.append_attribute("x") = player->pbody->body->GetTransform().p.x;
	playerPos.append_attribute("y") = player->pbody->body->GetTransform().p.y;

	// Save current player number of lives
	playerPos.append_attribute("lives") = player->lives;

	playerPos.append_attribute("items") = player->Items;

	// Save current airenemy position
	pugi::xml_node airenemyPos = data.append_child("airenemyPosition");
	airenemyPos.append_attribute("x") = airenemy->pbody->body->GetTransform().p.x;
	airenemyPos.append_attribute("y") = airenemy->pbody->body->GetTransform().p.y;

	// Save current groundenemy position
	pugi::xml_node groundenemyPos = data.append_child("groundenemyPosition");
	groundenemyPos.append_attribute("x") = groundenemy->pbody->body->GetTransform().p.x;
	groundenemyPos.append_attribute("y") = groundenemy->pbody->body->GetTransform().p.y;

	// Save current air number of lives
	pugi::xml_node airenemyLives = data.append_child("airenemyLives");
	airenemyLives.append_attribute("airenemyLives") = airenemy->health;

	// Save current ground number of lives
	pugi::xml_node groundenemyLives = data.append_child("groundenemyLives");
	groundenemyLives.append_attribute("groundenemyLives") = groundenemy->hp;

	return true;
}