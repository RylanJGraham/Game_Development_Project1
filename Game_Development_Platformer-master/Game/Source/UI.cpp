#include "UI.h"

#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Input.h"
#include "Player.h"
#include "Scene.h"
#include "Window.h"

#include "SDL/include/SDL.h"

#include "Defs.h"
#include "Log.h"


UI::UI() :Module()
{
	name.Create("UI_Module");
}

UI::~UI()
{}

bool UI::Awake(pugi::xml_node& config)
{
	LOG("Loading UI_Module");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool UI::Start()
{
	LOG("--STARTS UI MODULE--");

	/*Initialize*/
	font1Path = app->configNode.child("ui").child("font1").attribute("texturepath").as_string();

	//Loading font 1
	char lookupTableFont1[] = { "! @,_./0123456789$;<&?abcdefghijklmnopqrstuvwxyz" };
	font1_id = app->fonts->Load(font1Path, lookupTableFont1, 2);

	return true;
}

// Called each loop iteration
bool UI::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool UI::Update(float dt)
{
	return true;
}

// Called each loop iteration
bool UI::PostUpdate()
{
	return true;
}

// Called before quitting
bool UI::CleanUp()
{
	LOG("Freeing UI_Module");

	app->fonts->UnLoad(font1_id);
	//app->fonts->UnLoad(font2_id);

	return true;
}


void UI::BlitLives()
{
	char playerLives[20];
	sprintf_s(playerLives, 20, "lives; %d", app->scene->player->lives);
	app->fonts->BlitText(50, 50, font1_id, playerLives);
}

void UI::BlitItems()
{
	char playerItems[20];
	sprintf_s(playerItems, 20, "chests; %d", app->scene->player->Items);
	app->fonts->BlitText(20, 35, font1_id, playerItems);
}


/* UI DEBUG INFO WITH FONT 2 */
void UI::BlitPlayerXPos()
{
	char playerXPos[25];
	sprintf_s(playerXPos, 25, "position x; %d", app->scene->player->position.x);
	app->fonts->BlitText(20, 55, font1_id, playerXPos);
}

void UI::BlitPlayerYPos()
{
	char playerYPos[25];
	sprintf_s(playerYPos, 25, "position y; %d", app->scene->player->position.y);
	app->fonts->BlitText(20, 75, font1_id, playerYPos);
}

void UI::BlitBatLives()
{
	char BatLives[25];
	sprintf_s(BatLives, 20, "Bat lives; %d", app->scene->airenemy->health);
	app->fonts->BlitText(20, 95, font1_id, BatLives);
}

void UI::BlitPigLives()
{
	char PigLives[25];
	sprintf_s(PigLives, 20, "Pig lives; %d", app->scene->groundenemy->hp);
	app->fonts->BlitText(20, 95, font1_id, PigLives);
}

void UI::BlitFPS()
{
	char fps[25];
	sprintf_s(fps, 25, "fps: %d", app->GetFPS());
	app->fonts->BlitText(825, 15, font1_id, fps);
}

void UI::BlitAverageFPS()
{
	char averageFps[25];
	sprintf_s(averageFps, 25, "average fps; %.2f", app->GetAverageFPS());
	app->fonts->BlitText(825, 40, font1_id, averageFps);
}

void UI::BlitDT()
{
	char dt[25];
	sprintf_s(dt, 25, "delta time; %.2f ms", app->GetDT());
	app->fonts->BlitText(825, 50, font1_id, dt);
}

void UI::BlitTimeSinceStart()
{
	char timeSinceStart[25];
	sprintf_s(timeSinceStart, 25, "time since start; %.2f", app->GetTimesSinceStart());
	app->fonts->BlitText(825, 60, font1_id, timeSinceStart);
}

void UI::BlitFrameCount()
{
	char frameCount[25];
	sprintf_s(frameCount, 25, "frame count; %d", app->GetFrameCount());
	app->fonts->BlitText(825, 70, font1_id, frameCount);
}
