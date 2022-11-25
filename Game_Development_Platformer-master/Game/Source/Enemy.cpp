#include "Enemy.h"
#include "App.h"
#include "FadeToBlack.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Ending.h"
#include "Title.h"
#include "Player.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("Enemy");
}

Enemy::~Enemy() {

}

bool Enemy::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";

	//L02: DONE 5: Get Player parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Enemy::Start()
{
	alive = true;

	//initilize textures
	texture = app->tex->Load(texturePath);

	remainingJumpSteps = 0;
	idle = true;

	//id = app->tex->LoadSprite(texturePath, 15, 8);

	// L07 DONE 5: Add physics to the player - initialize physics body
	ebody = app->physics->CreateCircle(position.x - 20, position.y - 5, 20, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	ebody->listener = this;
	ebody->ctype = ColliderType::ENEMY;

	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	//jumpSFX = app->audio->LoadFx("Assets/Audio/Fx/JumpKnight.wav");

	LoadAnimations();

	return true;
}

void Enemy::SetPos(int x, int y) {
	b2Vec2 pos = { PIXEL_TO_METERS(x), PIXEL_TO_METERS(y) };
	ebody->body->SetTransform(pos, 0);
}

bool Enemy::Update()
{
	b2Vec2 vel;
	int speed = 5;

	DebugKeys();


		/*if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			vel.x = -speed;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			vel.x = speed;
		}
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			vel.y = -speed;
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			vel.y = speed;
		}*/
	
	if(1)
	{
		ebody->body->SetGravityScale(1);
		vel = ebody->body->GetLinearVelocity() + b2Vec2(0, -GRAVITY_Y * 0.0166);
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
			currentAnim = &RunR;
			vel.x = -speed;
			idle = false;
			leftID = true;
			app->render->camera.x += 5;
		}
		//Right
		else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			currentAnim = &RunL;
			vel.x = speed;
			idle = false;
			leftID = false;
			app->render->camera.x -= 5;
		}
		else
			vel.x = 0;

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isGrounded && remainingJumpSteps == 0) {
			currentAnim = leftID ? &JumpL : &JumpR;
			remainingJumpSteps = 6;
			idle = false;
			isGrounded = false;
			//app->audio->PlayFx(jumpSFX);

		}
	}

	//Set the velocity of the pbody of the player
	ebody->body->SetLinearVelocity(vel);

	//Apply Jump Force
	if (remainingJumpSteps > 0)
	{
		float force = ebody->body->GetMass() * 10 / 0.01666; //F = mv/t (t = 1/60fps)
		force /= 6.0;
		ebody->body->ApplyForce(b2Vec2(0, -force), ebody->body->GetWorldCenter(), true);
		remainingJumpSteps--;
	}

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(ebody->body->GetTransform().p.x) - 46;
	position.y = METERS_TO_PIXELS(ebody->body->GetTransform().p.y) - 60;

	//Animations
	if (idle) { currentAnim = leftID ? &IdleR : &IdleL; }
	if (!isGrounded) { currentAnim = leftID ? &JumpR : &JumpL; }
	SDL_Rect rect2 = currentAnim->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect2);
	currentAnim->Update();

	return true;
}

bool Enemy::PostUpdate()
{
	//For highscore

	return true;
}

bool Enemy::CleanUp()
{

	return true;
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
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
	RunL.PushBack({ 76, 172, 72, 86 });
	RunL.PushBack({ 150, 172, 72, 86 });
	RunL.PushBack({ 220, 172, 72, 86 });
	RunL.PushBack({ 292, 172, 72, 86 });
	RunL.PushBack({ 364, 172, 72, 86 });
	RunL.PushBack({ 436, 172, 72, 86 });
	RunL.speed = 0.1f;
	RunL.loop = true;

	RunR.PushBack({ 428, 258, 72, 86 });
	RunR.PushBack({ 356, 258, 72, 86 });
	RunR.PushBack({ 284, 258, 72, 86 });
	RunR.PushBack({ 212, 258, 72, 86 });
	RunR.PushBack({ 140, 258, 72, 86 });
	RunR.PushBack({ 68, 258, 72, 86 });
	RunR.PushBack({ 0, 258, 72, 86 });
	RunR.speed = 0.1f;
	RunR.loop = true;

	JumpL.PushBack({ 0, 344, 80, 86 });
	JumpL.PushBack({ 80, 344, 80, 86 });
	JumpL.PushBack({ 160, 344, 80, 86 });
	JumpL.speed = 0.5f;
	JumpL.loop = false;

	JumpR.PushBack({ 405, 430, 80, 86 });
	JumpR.PushBack({ 325, 430, 80, 86 });
	JumpR.PushBack({ 245, 430, 80, 86 });
	JumpR.speed = 0.5f;
	JumpR.loop = false;
}