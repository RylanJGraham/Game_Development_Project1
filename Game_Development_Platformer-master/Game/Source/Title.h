#ifndef __TITLE_H__
#define __TITLE_H__

#include "Module.h"
#include "SDL/include/SDL.h"
#include "GuiButton.h"

struct SDL_Texture;

class Title : public Module
{
public:

	Title(bool startEnabled);

	// Destructor
	virtual ~Title();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);


	uint startSFX = 0;
	uint menuSelectionSFX = 0;
	uint selectSFX = 0;

	// Declare a GUI Button and create it using the GuiManager
	GuiButton* playButton1;
	GuiButton* settingsButton2;
	GuiButton* creditsButton3;
	GuiButton* exitButton4;
	GuiButton* continueButton5; // Show only if there is a saved game in "save_game.xml"
	GuiButton* closeSettingMenuButton6; // Show only if bool "settingMenu" is true
	GuiButton* closeCreditsMenuButton7; // Show only if bool "creditsMenu" is true

	GuiButton* decreaseMusicButton8;
	GuiButton* increaseMusicButton9;

	GuiButton* decreaseSFXButton10;
	GuiButton* increaseSFXButton11;

	GuiButton* fullscreenButton12;

	GuiButton* vsyncButton13;

	bool settingMenu = false;
	bool creditsMenu = false;

	int mouseX, mouseY;
	uint font1_id = -1;


	
private:
	SDL_Texture* img = nullptr;
	SDL_Texture* Img_settings = nullptr;
	SDL_Texture* Img_credits = nullptr;

	const char* font1Path;

	const char* imgPath;
	const char* popImgSettingsPath;
	const char* popImgCreditsPath;
	const char* musicPath;
	const char* startSFXPath;
	const char* selectSFXPath;
	const char* select2SFXPath;

	bool isSaved = false;

	bool exitGame = false;

};

#endif // !__TITLE_H__
