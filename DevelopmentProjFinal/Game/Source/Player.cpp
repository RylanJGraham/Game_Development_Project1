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

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x+16, position.y-6, 32, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	return true;
}

bool isJumping = false;
b2Vec2 vel = b2Vec2(0, -GRAVITY_Y);
float Jump = 5;



bool Player::Update()
{

	// L07 DONE 5: Add physics to the player - updated player position using physics
	vel.x = 0;
	//pbody->listener->OnCollision(pbody, );
	int speed = 15; 

	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		//
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		//
	}
		
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		vel.x -= speed;
		app->render->camera.x += 2;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		vel.x += speed;
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
		else {
			isJumping = false;
		}
	}

	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x)-46;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y)-52;

	SDL_Rect Pdims;
	Pdims.x = 0;
	Pdims.y = 0;
	Pdims.h = 86;
	Pdims.w = 72;

	SDL_Rect* sect = &Pdims;

	

	app->render->DrawTexture(texture, position.x , position.y, sect);

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
		LOG("Collision PLATFORM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}



}

