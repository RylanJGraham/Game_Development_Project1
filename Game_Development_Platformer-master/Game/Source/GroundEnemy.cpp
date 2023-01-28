#include "GroundEnemy.h"
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

GroundEnemy::GroundEnemy() : Entity(EntityType::ENEMY)
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
	return true;
}

bool GroundEnemy::Start()
{
	startPos.x = parameters.attribute("x").as_int();
	startPos.y = parameters.attribute("y").as_int();

	damagedSFX = app->audio->LoadFx("Assets/Audio/Fx/pigHurt.wav");

	origin.x = startPos.x;
	origin.y = startPos.y;

	width = 32;
	height = 32;

	texturePath = parameters.attribute("texturepath").as_string();

	LoadAnimations();

	texture = app->tex->Load(texturePath);

	currentAnim = &AttackL;
	alive = true;
	isHurt = false;
	hp = 3;

	//initilize textures

	//id = app->tex->LoadSprite(texturePath, 15, 8);

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(startPos.x, startPos.y - 30, 10, bodyType::DYNAMIC, ColliderType::ENEMY);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	hitbox = app->physics->CreateRectangleSensor(METERS_TO_PIXELS(pbody->body->GetTransform().p.x), METERS_TO_PIXELS(pbody->body->GetTransform().p.y) + 50, 5, 2, bodyType::STATIC, ColliderType::GROUNDENEMYHITBOX);

	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	//jumpSFX = app->audio->LoadFx("Assets/Audio/Fx/JumpKnight.wav");
	damagedSFX = app->audio->LoadFx("Assets/Audio/Fx/AirEnemyDamage.wav");
	refreshPathTime = 0;

	jump = false;
	ableAttack = true;

	return true;
}

bool GroundEnemy::PreUpdate() {

	return true;
}

//void GroundEnemy::SetPos(int x, int y) {
//	b2Vec2 pos = { PIXEL_TO_METERS(x), PIXEL_TO_METERS(y) };
//	pbody->body->SetTransform(pos, 0);
//}

bool GroundEnemy::Update(float dt)
{
	currentAnim = &AttackL;
	velocity.y = -GRAVITY_Y;

	iPoint playerTile = app->map->WorldToMap(app->scene->player->position.x + 32 , app->scene->player->position.y);

	if (pbody->body->GetPosition().x > app->render->camera.x - app->render->camera.w / 2 && pbody->body->GetPosition().x < app->render->camera.x + app->render->camera.w / 2)
	{
		float distance = playerTile.x - origin.x;

		//Test compute path function
		if (originSelected == true && distance <= 10 && distance >= -10)
		{
			app->pathfinding->CreatePath(origin, playerTile);
			refreshPathTime++;
			originSelected = false;
			MovementDirection(origin, playerTile, dt);
			Attack(origin, playerTile);
		}
		else
		{
			origin.x = pbody->body->GetPosition().x * app->win->GetScale();
			origin.y = pbody->body->GetPosition().y * app->win->GetScale();
			originSelected = true;
			app->pathfinding->ClearLastPath();
			refreshPathTime = 0;
		}

		/*MovementDirection(origin, playerTile);*/
	}
	else
	{
		app->pathfinding->ClearLastPath();
		refreshPathTime = 0;
	}

	if (jump == false)
		pbody->body->SetLinearVelocity(velocity);
	else if (jump == true)
		currentAnim = leftID ? &JumpR : &JumpL;
	if (ableAttack == false) {
		attackCooldown++;
		if (attackCooldown >= 50)
			ableAttack = true;
	}

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x - (width / 4));
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y - (height / 3));

	//hitbox->body->SetGravityScale(0);
	hitboxPos.x = pbody->body->GetTransform().p.x;
	hitboxPos.y = pbody->body->GetTransform().p.y - PIXEL_TO_METERS(10);
	hitbox->body->SetTransform({ hitboxPos.x, hitboxPos.y }, 0);


	if (alive == false)
	{
		currentAnim = leftID ? &DeathR : &DeathL;

		//Destroy entity
		app->entityManager->DestroyEntity(app->scene->groundenemy);
		app->physics->world->DestroyBody(pbody->body);
		app->physics->world->DestroyBody(hitbox->body);
	/*	app->audio->PlayFx(powerUpSFX);*/
		alive = true;
	}

	DebugKeys();

	if (isHurt) {
			currentAnim = leftID ? &HitR : &HitL;
			app->audio->PlayFx(damagedSFX);
	}

	if (HitL.GetCurrentFrameint() == 1 || HitR.GetCurrentFrameint() == 1) {
			HitL.Reset();
			HitR.Reset();
			isHurt = false;
	}


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
		app->render->DrawTexture(app->scene->originTex, originScreen.x + 8, originScreen.y + 5);
	}

	SDL_Rect rect2 = currentAnim->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x - 20, position.y - 8, &rect2);
	currentAnim->Update();

	return true;
}

bool GroundEnemy::PostUpdate()
{
	return true;
}

bool GroundEnemy::CleanUp()
{
	return true;
}

void GroundEnemy::MovementDirection(const iPoint& origin, const iPoint& destination, float dt) {

	float resX = destination.x - origin.x;
	float resY = destination.y - origin.y;

	if (app->pathfinding->IsWalkable(destination) != 0) {
		//Check if player is to the right or the left of the origin
		if (resX < 0) {
			velocity.x = -0.12*dt;
			/*fliped = SDL_FLIP_NONE;*/
		}
		if (resX > 0) {
			velocity.x = +0.12*dt;
			//fliped = SDL_FLIP_HORIZONTAL;
		}
		if (resY < 0 && jump == false && app->pathfinding->GetNextTileY(2) < 15 && app->pathfinding->GetNextTileY(2) > 13) {
			leftID ? JumpR.Reset() : JumpL.Reset();
			jump = true;
			pbody->body->ApplyLinearImpulse({ 0, -1.2 }, pbody->body->GetWorldCenter(), true);
		}
		/*ENEMY JUMP FOR 4 TILES HEIGHT, 15 iS The Lowest Platform Tile Height*/
		if (resY < 0 && jump == false && app->pathfinding->GetNextTileY(2) < 14 && app->pathfinding->GetNextTileY(2) > 12) {
			leftID ? JumpR.Reset() : JumpL.Reset();
			jump = true;
			pbody->body->ApplyLinearImpulse({ 0, -1.5 }, pbody->body->GetWorldCenter(), true);
		}
	}
	else {
		velocity.x = 0;
	}
}

void GroundEnemy::Attack(const iPoint& origin, const iPoint& destination) {

	float resX = destination.x - origin.x;

	/*Attack Left*/
	if (resX < -2 && resX > -7 && jump == false && ableAttack == true) {
		ableAttack = false;
		attackCooldown = 0;
		pbody->body->ApplyLinearImpulse({ -25, 0 }, pbody->body->GetLocalCenter(), true); //Dash to Player
	}

	/*Attack Right*/
	if (resX > 2 && resX < 7 && jump == false && ableAttack == true) {
		ableAttack = false;
		attackCooldown = 0;
		pbody->body->ApplyLinearImpulse({ 25, 0 }, pbody->body->GetLocalCenter(), true); //Dash to Player
	}
}


void GroundEnemy::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->cType)
	{
	case ColliderType::WALL:
		LOG("Collision WALL");
		break;
	case ColliderType::GROUND:
		LOG("Collision GROUND");
		onGround = true;
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		if (jump == true)
			jump = false;
		break;
	case ColliderType::DEATH:
		LOG("Collision DEATH");
		hp = 0;
		break;
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		break;
	case ColliderType::PLAYERATTACK:
		LOG("Collision ATTACK");
		isHurt = true;
		hp--;
		if (hp <= 0) {
			alive = false;
		}
		break;
	}
}

void GroundEnemy::DebugKeys()
{

	// F9: View colliders / logic
	if (app->input->GetKey(SDL_SCANCODE_F9 || SDL_SCANCODE_N) == KEY_DOWN ) {
		app->physics->debug = !app->physics->debug;
	}

	// F10: God Mode (fly around, cannot be killed, etc)
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
	}
}

void GroundEnemy::ResetGroundEnemy() {

	pbody->body->SetSleepingAllowed(false);


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
	HitL.PushBack({ 5, 110, 43, 26 });
	HitL.PushBack({ 53, 110, 43, 26 });
	HitL.speed = 0.1f;
	HitL.loop = false;

	DeathL.PushBack({ 5, 110, 43, 26 });
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