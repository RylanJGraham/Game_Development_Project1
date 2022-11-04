#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Animation.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";

	//L02: DONE 5: Get Player parameters from XML
	startPos.x = parameters.attribute("x").as_int();
	startPos.y = parameters.attribute("y").as_int();


	texturePath = parameters.attribute("texturepath").as_string();

	width = 32;
	height = 32;

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x + 16, position.y - 6, 32, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	IdleL.PushBack({ 0, 0, 72, 86 });
	IdleL.PushBack({ 0, 0, 72, 86 });
	IdleL.PushBack({ 67, 0, 72, 86 });
	IdleL.PushBack({ 134, 0, 72, 86 });
	IdleL.PushBack({ 134, 0, 72, 86 });
	IdleL.PushBack({ 201,0, 72, 86 });
	IdleL.speed = 0.05f;
	IdleL.loop = true;

	IdleR.PushBack({ 0, 86, 72, 86 });
	IdleR.PushBack({ 0, 86, 72, 86 });
	IdleR.PushBack({ 67, 86, 72, 86 });
	IdleR.PushBack({ 134, 86, 72, 86 });
	IdleR.PushBack({ 134, 86, 72, 86 });
	IdleR.PushBack({ 201, 86, 72, 86 });
	IdleR.speed = 0.05f;
	IdleR.loop = true;

	RunL.PushBack({ 0, 172, 72, 86 });
	RunL.PushBack({ 72, 172, 72, 86 });
	RunL.PushBack({ 144, 172, 72, 86 });
	RunL.PushBack({ 216, 172, 72, 86 });
	RunL.PushBack({ 288, 172, 72, 86 });
	RunL.PushBack({ 360, 172, 72, 86 });
	RunL.PushBack({ 432, 172, 72, 86 });
	RunL.speed = 0.1f;
	RunL.loop = true;

	RunR.PushBack({ 432, 258, 72, 86 });
	RunR.PushBack({ 360, 258, 72, 86 });
	RunR.PushBack({ 288, 258, 72, 86 });
	RunR.PushBack({ 216, 258, 72, 86 });
	RunR.PushBack({ 144, 258, 72, 86 });
	RunR.PushBack({ 72, 258, 72, 86 });
	RunR.PushBack({ 0, 258, 72, 86 });
	RunR.speed = 0.1f;
	RunR.loop = true;

	JumpL.PushBack({ 0, 344, 80, 86 });
	JumpL.PushBack({ 80, 344, 80, 86 });
	JumpL.PushBack({ 160, 344, 80, 86 });
	JumpL.speed = 0.01f;
	JumpL.loop = false;

	JumpR.PushBack({ 400, 430, 80, 86 });
	JumpR.PushBack({ 320, 430, 80, 86 });
	JumpR.PushBack({ 240, 430, 80, 86 });
	JumpR.speed = 0.01f;
	JumpR.loop = false;

	return true;
}


bool Player::PreUpdate() {
	return true;
}


bool Player::Update()
{

	// L07 DONE 5: Add physics to the player - updated player position using physics
	vel.x = 0;
	//pbody->listener->OnCollision(pbody, );
	int speed = 15; 
	vel = b2Vec2(0, -GRAVITY_Y);

	if (FacingFront) {
		currentAnim = &IdleL;
	}
	else {
		currentAnim = &IdleR;
	}

	if (godMode == true) {

		vel = { 0, 0 };

		// Fly around the map
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			vel.y = -5;
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			vel.y = 5;
		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			vel.x = -5;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			vel.x = 5;
		}
	}
	else {
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			//
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			//
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			vel.x -= speed;
			FacingFront = false;
			currentAnim = &RunR;
			app->render->camera.x += 2;

		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			vel.x += speed;
			FacingFront = true;
			currentAnim = &RunL;
			app->render->camera.x -= 2;
		}

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
			if (!isJumping) {
				isJumping = true;
				Jump = 5.5;
				vel.y = 0;
			}
		}

		if (isJumping && Jump > 0) {
			vel.y -= Jump;
			Jump += GRAVITY_Y;
		}

		if (Jump <= 0) {
			if (vel.y < 20) {
				vel.y -= GRAVITY_Y;
			}
		}

	}

	if (isJumping) {
		if (FacingFront) {
			currentAnim = &JumpL;
		}
		else {
			currentAnim = &JumpR;
		}
	}

	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	
	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x)-46;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y)-52;

	SDL_Rect frame = currentAnim->GetCurrentFrame();

	app->render->DrawTexture(texture, position.x, position.y, &frame);

	currentAnim->Update();

	return true;
}

bool Player::PostUpdate() {
	return true;
}

bool Player::CleanUp()
{
	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	// L07 DONE 7: Detect the type of collision

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		//app->audio->PlayFx(pickCoinFxId); 
		break;
	case ColliderType::PLATFORM:
		isJumping = false;
		startPos = startPos;
		LOG("Collision PLATFORM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}

}

void Player::ResetPlayerPos() {

	pbody->body->SetSleepingAllowed(false);
	vel = { 0, 0 };
	pbody->body->SetTransform(PIXEL_TO_METERS(startPos), 0.0f);
	//position = startingPosition;
	//app->scene->cameraFix = false;

	LOG("--RESETING PLAYER--");
}


