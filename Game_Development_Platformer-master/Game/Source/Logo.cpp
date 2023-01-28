#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Logo.h"
#include "EntityManager.h"
#include "FadeToBlack.h"
#include "Map.h"

#include "Defs.h"
#include "Log.h"

Logo::Logo(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene");
}

// Destructor
Logo::~Logo()
{}

// Called before render is available
bool Logo::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Logo::Start()
{
	img = app->tex->Load("Assets/Textures/XRLogo.png");
	selectSFX = app->audio->LoadFx("Assets/Audio/Fx/swordswing.wav");
	BackgroundSFX = app->audio->LoadFx("Assets/Audio/Music/medieval.wav");

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	return true;
}

// Called each loop iteration
bool Logo::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Logo::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		app->fade->FadeBlack(this, (Module*)app->titleScreen, 60 * (16.0f / dt));
		app->audio->PlayFx(BackgroundSFX, 1);
		app->audio->PlayFx(selectSFX);
	}

	return true;
}

// Called each loop iteration
bool Logo::PostUpdate()
{
	app->render->DrawTexture(img, 0, 0);
	
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		return false;
	}

	return true;
}

// Called before quitting
bool Logo::CleanUp()
{
	LOG("Freeing scene");

	return true;
}