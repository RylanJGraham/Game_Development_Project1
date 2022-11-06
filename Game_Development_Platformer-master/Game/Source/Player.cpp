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
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Player::Start()
{
	alive = true;
	godMode = false;
	//initilize textures
	texture = app->tex->Load(texturePath);

	remainingJumpSteps = 0;
	idle = true;

	//id = app->tex->LoadSprite(texturePath, 15, 8);
	
	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x+16, position.y+16, 12, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	LoadAnimations();

	return true;
}

bool Player::Update()
{
	b2Vec2 vel;
	int speed = 5;

	DebugKeys();

	if (godMode)
	{
		alive = true;
		pbody->body->SetGravityScale(0);	
	}
	else
	{
		pbody->body->SetGravityScale(1);
		vel = pbody->body->GetLinearVelocity() + b2Vec2(0, -GRAVITY_Y * 0.0166);
	}

	if (!alive)
	{
		idle = false;
		currentAnim = &death;
	}
	else
	{
		idle = true;

		//Left
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			currentAnim = &RunL;
			vel.x = -speed;
			idle = false;
			leftID = true;
		}
		//Right
		else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			currentAnim = &RunR;
			vel.x = speed;
			idle = false;
			leftID = false;
		}
		else
			vel.x = 0;

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isGrounded && remainingJumpSteps == 0) {
			currentAnim = leftID ? &JumpL : &JumpR;
			remainingJumpSteps = 6;
			idle = false;
			isGrounded = false;
		}
	}
	

	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//Apply Jump Force
	if (remainingJumpSteps > 0)
	{
		float force = pbody->body->GetMass() * 10 / 0.01666; //F = mv/t (t = 1/60fps)
		force /= 6.0;
		pbody->body->ApplyForce(b2Vec2(0, -force), pbody->body->GetWorldCenter(), true);
		remainingJumpSteps--;	
	}

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 18;

	//Animations
	if (idle) { currentAnim = leftID ? &IdleL : &IdleR; }
	if (!isGrounded) { currentAnim = leftID ? &JumpL : &JumpR; }
	SDL_Rect rect2 = currentAnim->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect2);
	currentAnim->Update();

	return true;
}

bool Player::PostUpdate()
{
	//For highscore
	
	return true;
}

bool Player::CleanUp()
{

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::WALL:
		LOG("Collision WALL");
		break;
	case ColliderType::GROUND:
		LOG("Collision GROUND");
		isGrounded = true;
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		isGrounded = true;
		break;
	case ColliderType::DEATH:
		LOG("Collision DEATH");
		alive = false;
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}
}

void Player::DebugKeys()
{
	// F1/F2: Start from the first/second level
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {

	}

	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {

	}

	// F3: Start from the beginning of the current level
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {

	}

	// F5: Save the current game state
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		savedPosition.x = position.x;
		savedPosition.y = position.y;
	}

	// F6: Load the previous state (even across levels)
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {

	}

	// F9: View colliders / logic
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		app->physics->debug = !app->physics->debug;
	}

	// F10: God Mode (fly around, cannot be killed, etc)
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		godMode = !godMode;
	}
}

void Player::LoadAnimations()
{
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

	currentAnim = &IdleL;


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
}