#include "AirEnemy.h"
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
#include "PathFinding.h"
#include "FadeToBlack.h"
#include "EntityManager.h"

AirEnemy::AirEnemy() : Entity(EntityType::AIRENEMY)
{
	name.Create("AirEnemy");
}

AirEnemy::~AirEnemy() {

}

bool AirEnemy::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";

	//L02: DONE 5: Get Player parameters from XML

	return true;
}

bool AirEnemy::Start()
{
	texturePath = parameters.attribute("texturepath").as_string();

	texture = app->tex->Load(texturePath);

	startPos.x = parameters.attribute("x").as_int();
	startPos.y = parameters.attribute("y").as_int();

	origin.x = startPos.x;
	origin.y = startPos.y;

	currentAnim = &Idle;

	alive = true;
	health = 3;
	ishurt = false;
	//initilize textures

	/*idle = true;*/

	//id = app->tex->LoadSprite(texturePath, 15, 8);

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(startPos.x - 5, startPos.y + 10, 5, bodyType::DYNAMIC,ColliderType::ENEMY);
	//hitbox = app->physics->CreateRectangleSensor(METERS_TO_PIXELS(pbody->body->GetTransform().p.x), METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 15, 5, 2, bodyType::STATIC, ColliderType::SLIME_HITBOX);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	hitbox = app->physics->CreateRectangleSensor(METERS_TO_PIXELS(pbody->body->GetTransform().p.x), METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 15, 5, 2, bodyType::STATIC, ColliderType::AIRENEMYHITBOX);
	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	//jumpSFX = app->audio->LoadFx("Assets/Audio/Fx/JumpKnight.wav");
	damagedSFX = app->audio->LoadFx("Assets/Audio/Fx/AirEnemyDamage.wav");

	refreshPathTime = 0;

	LoadAnimations();

	return true;
}

bool AirEnemy::PreUpdate() {

	return true;
}

void AirEnemy::SetPos(int x, int y) {
	b2Vec2 pos = { PIXEL_TO_METERS(x), PIXEL_TO_METERS(y) };
	pbody->body->SetTransform(pos, 0);
}

bool AirEnemy::Update()
{
	b2Vec2 vel;
	int speed = 5;

	DebugKeys();

	velocity = { 0, 0 };
	
	iPoint playerTile = app->map->WorldToMap(app->scene->player->position.x + 32, app->scene->player->position.y);

	//Check if the enemy is visible on camera, if not, don't create path and don't move
	if (pbody->body->GetPosition().x > app->render->camera.x - app->render->camera.w / 2 && pbody->body->GetPosition().x < app->render->camera.x + app->render->camera.w / 2)
	{
		//Test compute path function
		if (originSelected == true)
		{
			app->pathfinding->CreatePath(origin, playerTile);
			refreshPathTime++;
			originSelected = false;
			/*if (refreshPathTime >= 150)
				originSelected = false;*/
		}
		else
		{
			origin.x = pbody->body->GetPosition().x;
			origin.y = pbody->body->GetPosition().y;
			originSelected = true;
			app->pathfinding->ClearLastPath();
			refreshPathTime = 0;
		}

		MovementDirection(origin, playerTile);
	}
	else
	{
		app->pathfinding->ClearLastPath();
		refreshPathTime = 0;
	}

	pbody->body->SetLinearVelocity(velocity);

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x - (width / 4));
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y - (height / 3));

	////hitbox->body->SetGravityScale(0);
	//hitboxPos.x = pbody->body->GetTransform().p.x;
	//hitboxPos.y = pbody->body->GetTransform().p.y - PIXEL_TO_METERS(10);
	//hitbox->body->SetTransform({ hitboxPos.x, hitboxPos.y }, 0);

	DebugKeys();

	if (app->physics->debug)
	{
		// L12: Get the latest calculated path and draw
		const DynArray<iPoint>* path = app->pathfinding->GetLastPath();
		//LOG("Path Count: %d", path->Count());
		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			app->render->DrawTexture(app->scene->batTilePathTex, pos.x, pos.y);
		}

		// L12: Debug pathfinding
		iPoint originScreen = app->map->MapToWorld(origin.x, origin.y);
		app->render->DrawTexture(app->scene->originTex, originScreen.x + 4, originScreen.y + 5);
	}

	////movement test keys
	//if (app->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT) {
	//	vel.x = -speed;
	//}
	//if (app->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT) {
	//	vel.x = speed;
	//}
	//if (app->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT) {
	//	vel.y = -speed;
	//}
	//if (app->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT) {
	//	vel.y = speed;
	//}

	if (alive != true)
	{
		currentAnim = &Death;

		//Destroy entity
		app->entityManager->DestroyEntity(app->scene->airenemy);
		app->physics->world->DestroyBody(pbody->body);
		//app->physics->world->DestroyBody(hitbox->body);
		//app->audio->PlayFx(powerUpSFX);
		alive = true;
	}

	else
	{
		/*idle = true;*/

		//Left
		//if (app->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT) {
		//	currentAnim = &Movement;
		//	vel.x = -speed;
		//	idle = false;
		//	app->render->camera.x += 5;
		//}
		////Right
		//else if (app->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT) {
		//	currentAnim = &Movement;
		//	vel.x = speed;
		//	idle = false;
		//	app->render->camera.x -= 5;
		//}
		//else
		//	vel.x = 0;

		//if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isGrounded && remainingJumpSteps == 0) {
		//	currentAnim = leftID ? &JumpL : &JumpR;
		//	remainingJumpSteps = 6;
		//	idle = false;
		//	isGrounded = false;
		//	//app->audio->PlayFx(jumpSFX);

		//}
		if (ishurt) {
			currentAnim = &Damaged;
			/*app->audio->PlayFx("")*/
		}
		else{
			currentAnim = &Attack;
		}
		if (Damaged.GetCurrentFrameint() == 1) {
			Damaged.Reset();
			ishurt = false;
		}

	}

	////Apply Jump Force
	//if (remainingJumpSteps > 0)
	//{
	//	float force = aebody->body->GetMass() * 10 / 0.01666; //F = mv/t (t = 1/60fps)
	//	force /= 6.0;
	//	aebody->body->ApplyForce(b2Vec2(0, -force), aebody->body->GetWorldCenter(), true);
	//}

	//Animations
	/*if (idle) { currentAnim = &Idle; }*/
	/*if (!isGrounded) { currentAnim = leftID ? &JumpR : &JumpL; }*/
	SDL_Rect rect2 = currentAnim->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x-20, position.y-8, &rect2);
	currentAnim->Update();

	return true;
}

bool AirEnemy::PostUpdate()
{
	//For highscore

	return true;
}

bool AirEnemy::CleanUp()
{

	return true;
}

void AirEnemy::MovementDirection(const iPoint& origin, const iPoint& destination) {

	float res = destination.x - origin.x;
	iPoint playerTile = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);
	if (app->pathfinding->IsWalkable(playerTile) != 0) {
		//Check if player is to the right or the left of the origin
		if (res < 0) {
			velocity.x = -2;
		}
		if (res > 0) {
			velocity.x = +2;
		}
	}
	else {
		velocity.x = 0;
	}
}


void AirEnemy::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->cType)
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
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
	case ColliderType::DEATH:
		LOG("Collision DEATH");
		alive = false;
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::PLAYERATTACK:
		LOG("Collision ATTACK");
		app->audio->PlayFx(damagedSFX);
		ishurt = true;
		health--;
		if (health <= 0) {
			alive = false;
		}
		break;
	}
}

void AirEnemy::DebugKeys()
{

	// F9: View colliders / logic
	if (app->input->GetKey(SDL_SCANCODE_F9 || SDL_SCANCODE_N) == KEY_DOWN) {
		app->physics->debug = !app->physics->debug;
	}

	// F10: God Mode (fly around, cannot be killed, etc)
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
	}
}

void AirEnemy::LoadAnimations()
{
	Idle.PushBack({ 7, 160, 42, 34 });
	Idle.PushBack({ 54, 160, 43, 34 });
	Idle.PushBack({ 102, 160, 43, 34 });
	Idle.PushBack({ 147, 160, 46, 34 });
	Idle.speed = 0.05f;
	Idle.loop = true;
	currentAnim = &Idle;


	Attack.PushBack({ 7, 6, 42, 41 });
	Attack.PushBack({ 54, 6, 43, 41 });
	Attack.PushBack({ 102, 6, 43, 41 });
	Attack.PushBack({ 147, 6, 46, 41 });
	Attack.speed = 0.1f;
	Attack.loop = true;

	Death.PushBack({ 7, 57, 42, 41 });
	Death.PushBack({ 54, 57, 43, 41 });
	Death.PushBack({ 102, 57, 43, 41 });
	Death.PushBack({ 147, 57, 46, 41 });
	Death.speed = 0.1f;
	Death.loop = false;

	Damaged.PushBack({ 7, 112, 42, 30 });
	Damaged.PushBack({ 54, 112, 43, 30 });
	Damaged.speed = 0.5f;
	Damaged.loop = false;

	Movement.PushBack({ 7, 207, 42, 32 });
	Movement.PushBack({ 54, 207, 43, 32 });
	Movement.PushBack({ 102, 207, 43, 32 });
	Movement.PushBack({ 147, 207, 46, 32 });
	Movement.speed = 0.5f;
	Movement.loop = true;
}
