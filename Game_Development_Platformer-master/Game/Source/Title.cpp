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
#include "GuiManager.h"
#include "Map.h"
#include "FadeToBlack.h"
#include "Fonts.h"
#include "UI.h"

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
	/*Initialize*/
	font1Path = app->configNode.child("ui").child("font1").attribute("texturepath").as_string();

	//Loading font 1
	char lookupTableFont1[] = { "! @,_./0123456789$;<&?abcdefghijklmnopqrstuvwxyz" };
	font1_id = app->fonts->Load(font1Path, lookupTableFont1, 2);

	app->render->camera.x = 0;

	app->map->Disable();

	LOG("--STARTS TITLE SCENE--");

	selectSFX = app->audio->LoadFx("Assets/Audio/Fx/swordswing.wav");

	popImgCreditsPath = app->configNode.child("title").child("popImage").attribute("creditstexturepath").as_string();
	popImgSettingsPath = app->configNode.child("title").child("popImage").attribute("settingtexturepath").as_string();
	//ImgCreditsPath = app->configNode.child("title").child("popImage").attribute("creditstexturepath").as_string();
	img = app->tex->Load("Assets/Textures/titlescreen.png");

	// Music
	app->audio->PlayMusic("Assets/Audio/Music/title_screen.ogg");

	Img_settings = app->tex->Load(popImgSettingsPath);
	Img_credits = app->tex->Load(popImgCreditsPath);

	app->physics->debug = false;
	settingMenu = false;
	creditsMenu = false;

	uint w, h;
	app->win->GetWindowSize(w, h);

	playButton1 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "play", 5, { ((int)w / 2) - 490, (int(h) / 2) - 280, 227, 83 }, this);
	settingsButton2 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "setting", 8, { ((int)w / 2) - 490, (int(h) / 2) - 190, 227, 83 }, this);
	creditsButton3 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "credits", 8, { ((int)w / 2) - 490, (int(h) / 2) - 100, 227, 83 }, this);
	exitButton4 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "exit", 5, { ((int)w / 2) - (93 / 2), (int(h) - 110), 93, 29 }, this);

	pugi::xml_document gameStateFile;
	pugi::xml_parse_result result = gameStateFile.load_file("save_game.xml");

	/*if (result != NULL)
	{
		continueButton5 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "continue", 9, { ((int)w / 2) - (93 / 2), (int(h) - 240), 93, 29 }, this);
	}*/

	closeSettingMenuButton6 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "close", 6, { ((int)w / 2) - 490, (int(h) / 2) - 280, 227, 83 }, this);
	closeCreditsMenuButton7 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "close", 6, { ((int)w / 2) - 490, (int(h) / 2) - 280, 227, 83 }, this);

	decreaseMusicButton8 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "decrease", 9, { 378, 243, 93, 29 }, this);
	increaseMusicButton9 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "increase", 9, { 555, 243, 93, 29 }, this);

	decreaseSFXButton10 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 10, "decrease", 9, { 378, 322, 93, 29 }, this);
	increaseSFXButton11 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 11, "increase", 9, { 555, 322, 93, 29 }, this);

	fullscreenButton12 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 12, "fullscreen", 11, { ((int)w / 2) - 490, (int(h) / 2) - 280, 227, 83 }, this);

	vsyncButton13 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 13, "vsync", 6, { 466, 502, 93, 29 }, this);


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
	uint w, h;
	app->win->GetWindowSize(w, h);

	playButton1->state = GuiControlState::NORMAL;
	settingsButton2->state = GuiControlState::NORMAL;
	creditsButton3->state = GuiControlState::NORMAL;

	char Play[20];
	sprintf_s(Play, 20, "Play Game");
	app->fonts->BlitText(((int)w / 2) - 490, (int(h) / 2), font1_id, Play);

	char sfx[10];
	sprintf_s(sfx, 10, "%d", app->sfxValue);
	app->fonts->BlitText(500, 330, app->ui->font1_id, sfx);

	char fullscreen[10];
	sprintf_s(fullscreen, 10, "%s", app->win->fullscreenMode ? "on" : "off");
	app->fonts->BlitText(502, 428, app->ui->font1_id, fullscreen);

	char vsync[10];
	sprintf_s(vsync, 10, "%s", app->render->limitFPS ? "on" : "off");
	app->fonts->BlitText(502, 510, app->ui->font1_id, vsync);

	app->input->GetMousePosition(mouseX, mouseY);

	if ((mouseX > playButton1->bounds.x) && (mouseX < (playButton1->bounds.x + playButton1->bounds.w)) &&
		(mouseY > playButton1->bounds.y) && (mouseY < (playButton1->bounds.y + playButton1->bounds.h)))
	{
		playButton1->state = GuiControlState::FOCUSED;
		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
		{
			playButton1->state = GuiControlState::PRESSED;
			LOG("PASA A GAME SCENE");
			app->fade->FadeBlack(this, (Module*)app->scene, 60 * (16.0f / dt));
			app->audio->PlayFx(selectSFX);
		}
	}

	if ((mouseX > settingsButton2->bounds.x) && (mouseX < (settingsButton2->bounds.x + settingsButton2->bounds.w)) &&
		(mouseY > settingsButton2->bounds.y) && (mouseY < (settingsButton2->bounds.y + settingsButton2->bounds.h)))
	{
		settingsButton2->state = GuiControlState::FOCUSED;
		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
		{
			settingsButton2->state = GuiControlState::PRESSED;
			LOG("PASA A settings");
			LOG("SettingsPressed");
			settingMenu = !settingMenu;
			app->audio->PlayFx(selectSFX);
		}
	}

	if ((mouseX > creditsButton3->bounds.x) && (mouseX < (creditsButton3->bounds.x + creditsButton3->bounds.w)) &&
		(mouseY > creditsButton3->bounds.y) && (mouseY < (creditsButton3->bounds.y + creditsButton3->bounds.h)))
	{
		creditsButton3->state = GuiControlState::FOCUSED;
		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
		{
			creditsButton3->state = GuiControlState::PRESSED;
			LOG("PASA A settings");
			LOG("SettingsPressed");
			creditsMenu = !creditsMenu;
			app->audio->PlayFx(selectSFX);
		}
	}
	
	pugi::xml_document gameStateFile;
	pugi::xml_parse_result result = gameStateFile.load_file("save_game.xml");

	/*if (result == NULL)
	{
		if (isSaved != false)
			continueButton5->state = GuiControlState::DISABLED;

		isSaved = false;
	}

	else
	{
		if (isSaved != true)
			continueButton5->state = GuiControlState::NORMAL;

		isSaved = true;
	}*/


	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		LOG("PASA A GAME SCENE");
		app->fade->FadeBlack(this, (Module*)app->scene, 60*(16.0f / dt));
		app->audio->PlayFx(selectSFX);
	}

	if (app->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
	{
		LOG("DebugGUIPressed");
		app->render->viewGUIbounds = !app->render->viewGUIbounds;
		app->audio->PlayFx(selectSFX);
	}

	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		LOG("SettingsPressed");
		settingMenu = !settingMenu;
		app->audio->PlayFx(selectSFX);
	}

	if (app->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
	{
		LOG("CreditsPressed");
		creditsMenu = !creditsMenu;
		app->audio->PlayFx(selectSFX);
	}

	app->render->DrawTexture(img, 0, 0, NULL);

	app->guiManager->Draw();

	// Setting Menu
	closeSettingMenuButton6->state = GuiControlState::DISABLED;
	decreaseMusicButton8->state = GuiControlState::DISABLED;
	increaseMusicButton9->state = GuiControlState::DISABLED;
	decreaseSFXButton10->state = GuiControlState::DISABLED;
	increaseSFXButton11->state = GuiControlState::DISABLED;
	fullscreenButton12->state = GuiControlState::DISABLED;
	vsyncButton13->state = GuiControlState::DISABLED;

	if (settingMenu == true)
	{
		//continueButton5->state = GuiControlState::DISABLED;
		playButton1->state = GuiControlState::DISABLED;
		settingsButton2->state = GuiControlState::DISABLED;
		creditsButton3->state = GuiControlState::DISABLED;
		exitButton4->state = GuiControlState::DISABLED;

		closeSettingMenuButton6->state = GuiControlState::NORMAL;
		decreaseMusicButton8->state = GuiControlState::NORMAL;
		increaseMusicButton9->state = GuiControlState::NORMAL;
		decreaseSFXButton10->state = GuiControlState::NORMAL;
		increaseSFXButton11->state = GuiControlState::NORMAL;
		fullscreenButton12->state = GuiControlState::NORMAL;
		vsyncButton13->state = GuiControlState::NORMAL;

		app->render->DrawTexture(Img_settings, 0, 0, NULL);

		char music[10];
		sprintf_s(music, 10, "%d", app->musicValue);
		app->fonts->BlitText(500, 250, app->ui->font1_id, music);

		char sfx[10];
		sprintf_s(sfx, 10, "%d", app->sfxValue);
		app->fonts->BlitText(500, 330, app->ui->font1_id, sfx);

		char fullscreen[10];
		sprintf_s(fullscreen, 10, "%s", app->win->fullscreenMode ? "on" : "off");
		app->fonts->BlitText(502, 428, app->ui->font1_id, fullscreen);

		char vsync[10];
		sprintf_s(vsync, 10, "%s", app->render->limitFPS ? "on" : "off");
		app->fonts->BlitText(502, 510, app->ui->font1_id, vsync);
	}

	// Credits Menu
	closeCreditsMenuButton7->state = GuiControlState::DISABLED;

	if (creditsMenu == true)
	{
		//continueButton5->state = GuiControlState::DISABLED;
		playButton1->state = GuiControlState::DISABLED;
		settingsButton2->state = GuiControlState::DISABLED;
		creditsButton3->state = GuiControlState::DISABLED;
		exitButton4->state = GuiControlState::DISABLED;

		app->render->DrawTexture(Img_credits, 0, 0, NULL);

		closeCreditsMenuButton7->state = GuiControlState::NORMAL;
	}

	return true;
}

// Called each loop iteration
bool Title::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	if (exitGame == true)
		ret = false;

	return ret;
}

// Called before quitting
bool Title::CleanUp()
{
	LOG("Freeing TITLE SCENE");

	if (img != nullptr && Img_settings != nullptr && Img_credits != nullptr) {
		app->tex->UnLoad(img);
		app->tex->UnLoad(Img_settings);
		app->tex->UnLoad(Img_credits);
	}

	//STORE IN A LIST THIS BUTTONS AND THEN CHECK HERE IF NULLPTR TO CLEAN THEM UP
	//if (continueButton5 != nullptr)
		/*continueButton5->state = GuiControlState::DISABLED;*/
	if (playButton1 != nullptr)
		playButton1->state = GuiControlState::DISABLED;
	if (settingsButton2 != nullptr)
		settingsButton2->state = GuiControlState::DISABLED;
	if (creditsButton3 != nullptr)
		creditsButton3->state = GuiControlState::DISABLED;
	if (exitButton4 != nullptr)
		exitButton4->state = GuiControlState::DISABLED;
	if (closeSettingMenuButton6 != nullptr)
		closeSettingMenuButton6->state = GuiControlState::DISABLED;
	if (closeCreditsMenuButton7 != nullptr)
		closeCreditsMenuButton7->state = GuiControlState::DISABLED;
	if (decreaseMusicButton8 != nullptr)
		decreaseMusicButton8->state = GuiControlState::DISABLED;
	if (increaseMusicButton9 != nullptr)
		increaseMusicButton9->state = GuiControlState::DISABLED;
	if (decreaseSFXButton10 != nullptr)
		decreaseSFXButton10->state = GuiControlState::DISABLED;
	if (increaseSFXButton11 != nullptr)
		increaseSFXButton11->state = GuiControlState::DISABLED;
	if (fullscreenButton12 != nullptr)
		fullscreenButton12->state = GuiControlState::DISABLED;
	if (vsyncButton13 != nullptr)
		vsyncButton13->state = GuiControlState::DISABLED;

	return true;
}

bool Title::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: TODO 5: Implement the OnGuiMouseClickEvent method

	switch (control->id)
	{
	case 5:
		// Continue button (only if "save_game.xml" exists)
		app->fade->FadeBlack(this, (Module*)app->scene, 90);
		app->scene->continueGame = true;
		app->audio->PlayFx(startSFX);
		break;

	case 1:
		// Play button
		app->fade->FadeBlack(this, (Module*)app->scene, 90);
		app->audio->PlayFx(startSFX);
		app->audio->PlayFx(selectSFX);
		if (remove("save_game.xml") != 0)
			LOG("Error at Deleting Save Game");
		else
			LOG("Save Game Successfully Deleted");
		break;

	case 2:
	case 6:
		// Settings button
		settingMenu = !settingMenu;

		if (settingMenu == false)
		{
			if (continueButton5 != nullptr)
				continueButton5->state = GuiControlState::NORMAL;
			playButton1->state = GuiControlState::NORMAL;
			settingsButton2->state = GuiControlState::NORMAL;
			creditsButton3->state = GuiControlState::NORMAL;
			exitButton4->state = GuiControlState::NORMAL;
		}
		app->audio->PlayFx(menuSelectionSFX);
		break;

	case 3:
	case 7:
		// Credits button
		creditsMenu = !creditsMenu;
		if (creditsMenu == false)
		{
			if (continueButton5 != nullptr)
				continueButton5->state = GuiControlState::NORMAL;
			playButton1->state = GuiControlState::NORMAL;
			settingsButton2->state = GuiControlState::NORMAL;
			creditsButton3->state = GuiControlState::NORMAL;
			exitButton4->state = GuiControlState::NORMAL;
		}
		app->audio->PlayFx(menuSelectionSFX);
		break;

	case 8:
		// Decrease music volume
		app->musicValue = app->musicValue - 1;
		if (app->musicValue <= 0)
			app->musicValue = 0;
		if (app->musicValue >= 100)
			app->musicValue = 100;
		Mix_VolumeMusic(app->musicValue);
		app->audio->PlayFx(menuSelectionSFX);
		break;

	case 9:
		// Increase music volume
		app->musicValue = app->musicValue + 1;
		if (app->musicValue <= 0)
			app->musicValue = 0;
		if (app->musicValue >= 100)
			app->musicValue = 100;
		Mix_VolumeMusic(app->musicValue);
		app->audio->PlayFx(menuSelectionSFX);
		break;

	case 10:
		// Decrease SFX volume
		app->sfxValue = app->sfxValue - 1;
		if (app->sfxValue <= 0)
			app->sfxValue = 0;
		if (app->sfxValue >= 100)
			app->sfxValue = 100;
		Mix_Volume(-1, app->sfxValue);
		app->audio->PlayFx(menuSelectionSFX);
		break;

	case 11:
		// Increase SFX volume
		app->sfxValue = app->sfxValue + 1;
		if (app->sfxValue <= 0)
			app->sfxValue = 0;
		if (app->sfxValue >= 100)
			app->sfxValue = 100;
		Mix_Volume(-1, app->sfxValue);
		app->audio->PlayFx(menuSelectionSFX);
		break;

	case 12:
		// Fullscreen button
		app->win->fullscreenMode = !app->win->fullscreenMode;
		if (app->win->fullscreenMode == true)
		{
			SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN);
		}
		else if (app->win->fullscreenMode == false)
		{
			SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_SHOWN);
		}
		app->audio->PlayFx(menuSelectionSFX);
		break;

	case 13:
		// V-Sync button
		app->render->limitFPS = !app->render->limitFPS;
		app->audio->PlayFx(menuSelectionSFX);
		break;

	case 4:
		// Exit button
		exitGame = !exitGame;
		app->audio->PlayFx(menuSelectionSFX);
		break;


	default:
		break;
	}

	return true;
}