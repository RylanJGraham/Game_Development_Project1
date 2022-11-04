#include "TitleScreen.h"

#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Entity.h"
#include "Physics.h"
#include "EntityManager.h"
#include "ModuleFadeToBlack.h"

#include "SDL/include/SDL_render.h"

#include "Defs.h"
#include "Log.h"

TitleScreen::TitleScreen() : Module()
{
	name.Create("TitleScreen");
}

// Destructor
TitleScreen::~TitleScreen()
{}

// Called before render is available
bool TitleScreen::Awake(pugi::xml_node& config)
{
	app->entityManager->Disable();
	app->physics->Disable();
	LOG("Loading TitleScreen");
	bool ret = true;



	return ret;
}

// Called before the first frame
bool TitleScreen::Start()
{
	img = app->tex->Load("Assets/Textures/titlescreen.png");


	return true;
}

// Called each loop iteration
bool TitleScreen::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool TitleScreen::Update(float dt)
{


	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		LOG("otra escena");
		app->fade->FadeToBlack(this, (Module*)app->scene, 90);
	}
	app->render->DrawTexture(img, 0, 0, NULL);

	return true;
}

// Called each loop iteration
bool TitleScreen::PostUpdate()
{
	bool ret = true;


	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	//app->render->DrawTexture(img, 0, 0, NULL);

	return ret;
}

// Called before quitting
bool TitleScreen::CleanUp()
{
	LOG("Freeing scene");

	if (img != nullptr) {
		app->tex->UnLoad(img);
	}

	return true;
}

