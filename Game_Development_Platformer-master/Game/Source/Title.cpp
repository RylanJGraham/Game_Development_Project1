#include "Title.h"

#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Entity.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Scene.h"
#include "Map.h"
#include "FadeToBlack.h"

#include "SDL/include/SDL_render.h"

#include "Defs.h"
#include "Log.h"

Title::Title(bool startEnabled) : Module(startEnabled)
{
	name.Create("TitleScreen");
}

// Destructor
Title::~Title()
{}

// Called before render is available
bool Title::Awake(pugi::xml_node& config)
{
	LOG("Loading TitleScreen");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Title::Start()
{

	app->render->camera.x = 0;

	LOG("--STARTS TITLE SCENE--");

	selectSFX = app->audio->LoadFx("Assets/Audio/Fx/swordswing.wav");

	img = app->tex->Load("Assets/Textures/titlescreen.png");

	// Music
	app->audio->PlayMusic("Assets/Audio/Music/title_screen.ogg");

	// Load SFXs
	/*startSFX = app->audio->LoadFx("Assets/Audio/Fx/start_game.wav");*/

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
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		LOG("PASA A GAME SCENE");
		app->fade->FadeBlack(this, (Module*)app->scene, 60);
		app->audio->PlayFx(selectSFX);
	}

	app->render->DrawTexture(img, 0, 0, NULL);

	return true;
}

// Called each loop iteration
bool Title::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Title::CleanUp()
{
	LOG("Freeing TITLE SCENE");

	if (img != nullptr) {
		app->tex->UnLoad(img);
	}

	return true;
}
