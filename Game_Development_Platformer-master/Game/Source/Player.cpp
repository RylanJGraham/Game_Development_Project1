#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Map.h"
#include "FadeToBlack.h"

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

	attackCooldownMax = (parameters.attribute("attcooldown").as_float()) * 0.06; //*0.06 because the cooldown is in milliseconds and the game runs at 60 fps

	return true;
}

bool Player::Start()
{
	//initilize textures
	texture = app->tex->Load(texturePath);

	remainingJumpSteps = 0;
	idle = true;

	//id = app->tex->LoadSprite(texturePath, 15, 8);
	
	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x - 20, position.y - 5, 18, bodyType::DYNAMIC, ColliderType::PLAYER);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	

	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	chestopenSFX = app->audio->LoadFx("Assets/Audio/Fx/chestopen.wav");
	//jumpSFX = app->audio->LoadFx("Assets/Audio/Fx/JumpKnight.wav");
	
	LoadAnimations();

	alive = true;
	isAttacking = false;
	godMode = false;
	attackCooldown = attackCooldownMax;
	lives = 3;

	return true;
}

bool Player::PreUpdate() {

	return true;
}

void Player::SetPos(int x, int y) {
	b2Vec2 pos = { PIXEL_TO_METERS(x), PIXEL_TO_METERS(y) };
	pbody->body->SetTransform(pos, 0);
}

bool Player::Update()
{
	currentAnim = &IdleR;

	b2Vec2 vel;
	int speed = 5;

	DebugKeys();

	if (godMode == true)
	{
		velocity = { 0, 0 };
		alive = true;
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			velocity.x = -speed;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			velocity.x = speed;
		}
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			velocity.y = -speed;
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			velocity.y = speed;
		}
	}
	else if (godMode == false && alive == true)
	{
		velocity = { 0, -GRAVITY_Y };
		pbody->body->SetGravityScale(1);
		vel = pbody->body->GetLinearVelocity() + b2Vec2(0, -GRAVITY_Y * 0.0166);
	}

	if (!alive)
	{
		idle = false;
		currentAnim = leftID ? &DeathL : &DeathR;
		pbody->body->SetAwake(false);
	}
	else
	{
		idle = true;

		//Left
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !isAttacking) {
			currentAnim = &RunR;
			velocity.x = -speed;
			idle = false;
			leftID = true;
				app->render->camera.x += 5;
		}
		//Right
		else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !isAttacking) {
			currentAnim = &RunL;
			velocity.x = speed;
			idle = false;
			leftID = false;
				app->render->camera.x -= 5;
		}
		else
			velocity.x = 0;

		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && isGrounded && remainingJumpSteps == 0 && !isAttacking) {
			currentAnim = leftID ? &JumpL : &JumpR;
			remainingJumpSteps = 6;
			idle = false;
			isGrounded = false;
			app->audio->PlayFx(jumpSFX);

		}

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isGrounded && attackCooldown == attackCooldownMax) {
			vel.x = 0;
			isAttacking = true;
			currentAnim = leftID ? &AttackR : &AttackL;
			attackCooldown = 0;
			if (!leftID) {
				hitbox = app->physics->CreateRectangleSensor(position.x + 82, position.y + 60, 30, 40, bodyType::STATIC, ColliderType::PLAYERATTACK);
			}
			else {
				hitbox = app->physics->CreateRectangleSensor(position.x + 5, position.y + 60, 30, 40, bodyType::STATIC, ColliderType::PLAYERATTACK);
			}
			hitbox->cType = ColliderType::PLAYERATTACK;
		}
	}

	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(velocity);

	//Apply Jump Force
	if (remainingJumpSteps > 0)
	{
		float force = pbody->body->GetMass() * 10 / 0.01666; //F = mv/t (t = 1/60fps)
		force /= 6.0;
		pbody->body->ApplyForce(b2Vec2(0, -force), pbody->body->GetWorldCenter(), true);
		remainingJumpSteps--;
	}

	if (attackCooldown < attackCooldownMax) {
		attackCooldown += 1;
	}

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 46;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 60;

	//Animations
	if (AttackL.GetCurrentFrameint() == 3 || AttackR.GetCurrentFrameint() == 3) {
		isAttacking = false;
		AttackL.Reset();
		AttackR.Reset();
		app->physics->world->DestroyBody(hitbox->body);
	}
	if (idle && !isAttacking) { currentAnim = leftID ? &IdleR : &IdleL; }
	if (!isGrounded) { currentAnim = leftID ? &JumpR : &JumpL; }
	SDL_Rect rect2 = currentAnim->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x-11, position.y, &rect2);
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
	switch (physB->cType)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		LOG("Collision COIN");
		Items++;
		//app->audio->PlayFx(pickCoinSFX);
		LOG("Items: %d", Items);
		app->audio->PlayFx(chestopenSFX);
		app->fade->FadeBlack((Module*)app->scene, (Module*)app->titleScreen, 60);
		/*app->physics->Disable();
		app->scene->Disable();*/
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
		lives--;
		if (lives == 0) {
			app->fade->FadeBlack((Module*)app->scene, (Module*)app->endScreen, 60);
		}
		break;
	case ColliderType::ENEMY:
		LOG("Collision ENEMY SLIME");
		lives--;
		if (lives <= 0) {
			alive = false;
		/*	app->audio->PlayFx(dieSFX);*/
			app->fade->FadeBlack((Module*)app->scene, (Module*)app->endScreen, 60);
		}
		else {
			app->scene->player->ResetPlayerPos();
			//app->audio->PlayFx(hurtSFX);
		}
		break;
	case ColliderType::PLAYERATTACK:
		LOG("Collision PLAYERATTACK");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}
}

void Player::DebugKeys()
{

	// F9: View colliders / logic
	//if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
	//	app->physics->debug = !app->physics->debug;
	//}

	// F10: God Mode (fly around, cannot be killed, etc)
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		godMode = !godMode;
	}
}

void Player::LoadAnimations()
{
	IdleL.PushBack({ 0, 320, 120, 80 });
	IdleL.PushBack({ 120, 320, 120, 80 });
	IdleL.PushBack({ 240, 320, 120, 80 });
	IdleL.PushBack({ 360, 320, 120, 80 });
	IdleL.PushBack({ 480, 320, 120, 80 });
	IdleL.PushBack({ 600, 320, 120, 80 });
	IdleL.PushBack({ 720, 320, 120, 80 });
	IdleL.PushBack({ 840, 320, 120, 80 });
	IdleL.PushBack({ 960, 320, 120, 80 });
	IdleL.PushBack({ 1080, 320, 120, 80 });
	IdleL.speed = 0.1f;
	IdleL.loop = true;

	IdleR.PushBack({ 1080, 800, 120, 80 });
	IdleR.PushBack({ 960, 800, 120, 80 });
	IdleR.PushBack({ 840, 800, 120, 80 });
	IdleR.PushBack({ 720, 800, 120, 80 });
	IdleR.PushBack({ 600, 800, 120, 80 });
	IdleR.PushBack({ 480, 800, 120, 80 });
	IdleR.PushBack({ 360, 800, 120, 80 });
	IdleR.PushBack({ 240, 800, 120, 80 });
	IdleR.PushBack({ 120, 800, 120, 80 });
	IdleR.PushBack({ 0, 800, 120, 80 });
	IdleR.speed = 0.1f;
	IdleR.loop = true;

	currentAnim = &IdleL;


	RunL.PushBack({ 0, 400, 120, 80 });
	RunL.PushBack({ 120, 400, 120, 80 });
	RunL.PushBack({ 240, 400, 120, 80 });
	RunL.PushBack({ 360, 400, 120, 80 });
	RunL.PushBack({ 480, 400, 120, 80 });
	RunL.PushBack({ 600, 400, 120, 80 });
	RunL.PushBack({ 720, 400, 120, 80 });
	RunL.PushBack({ 840, 400, 120, 80 });
	RunL.PushBack({ 960, 400, 120, 80 });
	RunL.PushBack({ 1080, 400, 120, 80 });
	RunL.speed = 0.2f;
	RunL.loop = true;

	RunR.PushBack({ 1080, 880, 120, 80 });
	RunR.PushBack({ 960, 880, 120, 80 });
	RunR.PushBack({ 840, 880, 120, 80 });
	RunR.PushBack({ 720, 880, 120, 80 });
	RunR.PushBack({ 600, 880, 120, 80 });
	RunR.PushBack({ 480, 880, 120, 80 });
	RunR.PushBack({ 360, 880, 120, 80 });
	RunR.PushBack({ 240, 880, 120, 80 });
	RunR.PushBack({ 120, 880, 120, 80 });
	RunR.PushBack({ 0, 880, 120, 80 });
	RunR.speed = 0.2f;
	RunR.loop = true;

	JumpL.PushBack({ 0, 160, 120, 80 });
	JumpL.PushBack({ 120, 160, 120, 80 });
	JumpL.PushBack({ 240, 160, 120, 80 });
	JumpL.PushBack({ 360, 160, 120, 80 });
	JumpL.PushBack({ 480, 160, 120, 80 });
	JumpL.PushBack({ 600, 160, 120, 80 });
	JumpL.speed = 0.5f;
	JumpL.loop = false;

	JumpR.PushBack({ 1080, 640, 120, 80 });
	JumpR.PushBack({ 960, 640, 120, 80 });
	JumpR.PushBack({ 840, 640, 120, 80 });
	JumpR.PushBack({ 720, 640, 120, 80 });
	JumpR.PushBack({ 600, 640, 120, 80 });
	JumpR.PushBack({ 480, 640, 120, 80 });
	JumpR.speed = 0.5f;
	JumpR.loop = false;

	DeathL.PushBack({ 120, 240, 120, 80 });
	DeathL.PushBack({ 120, 0, 120, 80 });
	DeathL.PushBack({ 240, 0, 120, 80 });
	DeathL.PushBack({ 320, 0, 120, 80 });
	DeathL.PushBack({ 480, 0, 120, 80 });
	DeathL.PushBack({ 600, 0, 120, 80 });
	DeathL.PushBack({ 720, 0, 120, 80 });
	DeathL.PushBack({ 840, 0, 120, 80 });
	DeathL.PushBack({ 960, 0, 120, 80 });
	DeathL.PushBack({ 1080, 0, 120, 80 });
	DeathL.speed = 0.5f;
	DeathL.loop = false;

	DeathR.PushBack({ 960, 720, 120, 80 });
	DeathR.PushBack({ 120, 480, 120, 80 });
	DeathR.PushBack({ 240, 480, 120, 80 });
	DeathR.PushBack({ 320, 480, 120, 80 });
	DeathR.PushBack({ 480, 480, 120, 80 });
	DeathR.PushBack({ 600, 480, 120, 80 });
	DeathR.PushBack({ 720, 480, 120, 80 });
	DeathR.PushBack({ 840, 480, 120, 80 });
	DeathR.PushBack({ 960, 480, 120, 80 });
	DeathR.PushBack({ 1080, 480, 120, 80 });
	DeathR.speed = 0.5f;
	DeathR.loop = false;

	AttackL.PushBack({ 0, 80, 120, 80 });
	AttackL.PushBack({ 125, 80, 120, 80 });
	AttackL.PushBack({ 250, 80, 120, 80 });
	AttackL.PushBack({ 375, 80, 120, 80 });
	AttackL.speed = 0.35f;
	AttackL.loop = false;

	AttackR.PushBack({ 1080, 560, 120, 80 });
	AttackR.PushBack({ 955, 560, 120, 80 });
	AttackR.PushBack({ 830, 560, 120, 80 });
	AttackR.PushBack({ 705, 560, 120, 80 });
	AttackR.speed = 0.35f;
	AttackR.loop = false;
}

void Player::ResetPlayerPos() {

	pbody->body->SetSleepingAllowed(false);
	velocity = { 0, 0 };
	pbody->body->SetTransform(PIXEL_TO_METERS(startPos), 0.0f);
	//app->scene->cameraFix2 = false;
	//app->scene->cameraFix = false;
	app->render->camera.x = 0;
	alive = true;

	LOG("--RESETING PLAYER--");
}