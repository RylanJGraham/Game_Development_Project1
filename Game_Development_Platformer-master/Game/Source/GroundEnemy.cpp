#include "GroundEnemy.h"
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

GroundEnemy::GroundEnemy() : Entity(EntityType::GROUNDENEMY)
{
	name.Create("GroundEnemy");
}

GroundEnemy::~GroundEnemy() {

}

bool GroundEnemy::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";

	//L02: DONE 5: Get Player parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool GroundEnemy::Start()
{
	alive = true;
	isHurt = false;
	hp = 3;

	//initilize textures
	texture = app->tex->Load(texturePath);

	remainingJumpSteps = 0;

	//id = app->tex->LoadSprite(texturePath, 15, 8);

	// L07 DONE 5: Add physics to the player - initialize physics body
	gebody = app->physics->CreateCircle(position.x - 5, position.y + 10, 15, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	gebody->listener = this;
	gebody->ctype = ColliderType::ENEMY;

	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	//jumpSFX = app->audio->LoadFx("Assets/Audio/Fx/JumpKnight.wav");

	LoadAnimations();

	return true;
}

void GroundEnemy::SetPos(int x, int y) {
	b2Vec2 pos = { PIXEL_TO_METERS(x), PIXEL_TO_METERS(y) };
	gebody->body->SetTransform(pos, 0);
}

bool GroundEnemy::Update()
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
		gebody->body->SetGravityScale(1);
		vel = gebody->body->GetLinearVelocity() + b2Vec2(0, -GRAVITY_Y * 0.0166);
	}

	if (hp <= 0) {
		alive = false;
	}

	if (!alive)
	{
		currentAnim = &death;
		app->physics->world->DestroyBody(gebody->body);
	}
	else
	{

		//Left
		//if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		//	currentAnim = &RunR;
		//	vel.x = -speed;
		//	leftID = true;
		//	app->render->camera.x += 5;
		//}
		//Right
		//else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		//	currentAnim = &RunL;
		//	vel.x = speed;
		//	leftID = false;
		//	app->render->camera.x -= 5;
		//}
		//else
		//	vel.x = 0;

		//if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isGrounded && remainingJumpSteps == 0) {
		//	currentAnim = leftID ? &JumpL : &JumpR;
		//	remainingJumpSteps = 6;
		//	isGrounded = false;
		//	//app->audio->PlayFx(jumpSFX);

		//}

		if (isHurt) {
			currentAnim = &HitL;
		}
		else {
			currentAnim = &AttackL;
		}

		if (HitL.GetCurrentFrameint() == 1 || HitR.GetCurrentFrameint() == 1) {
			HitL.Reset();
			HitR.Reset();
			isHurt = false;
		}
	}

	//Set the velocity of the pbody of the player
	gebody->body->SetLinearVelocity(vel);

	//Apply Jump Force
	//if (remainingJumpSteps > 0)
	//{
	//	float force = gebody->body->GetMass() * 10 / 0.01666; //F = mv/t (t = 1/60fps)
	//	force /= 6.0;
	//	gebody->body->ApplyForce(b2Vec2(0, -force), gebody->body->GetWorldCenter(), true);
	//	remainingJumpSteps--;
	//}

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(gebody->body->GetTransform().p.x);
	position.y = METERS_TO_PIXELS(gebody->body->GetTransform().p.y);

	//Animations
	//if (!isGrounded) { currentAnim = leftID ? &JumpR : &JumpL; }
	SDL_Rect rect2 = currentAnim->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x-20, position.y-8, &rect2);
	currentAnim->Update();

	return true;
}

bool GroundEnemy::PostUpdate()
{
	//For highscore

	return true;
}

bool GroundEnemy::CleanUp()
{

	return true;
}

void GroundEnemy::OnCollision(PhysBody* physA, PhysBody* physB)
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
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		break;
	case ColliderType::PLAYERATTACK:
		LOG("Collision ATTACK");
		isHurt = true;
		break;
	}
}

void GroundEnemy::DebugKeys()
{

	// F9: View colliders / logic
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		app->physics->debug = !app->physics->debug;
	}

	// F10: God Mode (fly around, cannot be killed, etc)
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
	}
}

void GroundEnemy::LoadAnimations()
{
	AttackL.PushBack({ 5, 14, 43, 26 });
	AttackL.PushBack({ 53, 14, 43, 26 });
	AttackL.PushBack({ 99, 14, 43, 26 });
	AttackL.PushBack({ 147, 14, 43, 26 });
	AttackL.speed = 0.15f;
	AttackL.loop = true;

	HitL.PushBack({ 5, 110, 43, 26 });
	HitL.PushBack({ 53, 110, 43, 26 });
	HitL.speed = 0.1f;
	HitL.loop = false;

	DeathL.PushBack({ 5, 62, 43, 26 });
	DeathL.PushBack({ 53, 62, 43, 26 });
	DeathL.PushBack({ 99, 62, 43, 26 });
	DeathL.PushBack({ 147, 62, 43, 26 });
	DeathL.speed = 0.1f;
	DeathL.loop = false;

	AttackR.PushBack({ 0, 86, 72, 86 });
	AttackR.PushBack({ 0, 86, 72, 86 });
	AttackR.PushBack({ 67, 86, 72, 86 });
	AttackR.PushBack({ 134, 86, 72, 86 });
	AttackR.PushBack({ 134, 86, 72, 86 });
	AttackR.PushBack({ 201, 86, 72, 86 });
	AttackR.speed = 0.15f;
	AttackR.loop = true;

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