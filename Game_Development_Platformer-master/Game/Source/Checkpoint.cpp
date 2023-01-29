#include "Checkpoint.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "EntityManager.h"
#include "FadeToBlack.h"
#include "UI.h"

Checkpoint::Checkpoint() : Entity(EntityType::CHECKPOINT)
{
	name.Create("checkpoint");
}

Checkpoint::~Checkpoint() {}

bool Checkpoint::Awake() {



	return true;
}

bool Checkpoint::Start() {

	//initilize textures

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	texture = app->tex->Load(texturePath);

	// L07 DONE 4: Add a physics to an item - initialize the physics body
	pbody = app->physics->CreateRectangleSensor(position.x + 16, position.y + 16, 16, 32, bodyType::STATIC, ColliderType::CHECKPOINT);
	pbody->cType = ColliderType::CHECKPOINT;
	pbody->listener = this;
	isPicked = false;
	section = { 26, 0, 26, 30 };
	saveSpot = false;

	return true;
}

bool Checkpoint::Update(float dt)
{
	if (saveSpot) {
		app->SaveGameRequest();
		saveSpot = false;
		app->scene->saveCounter = 2 * dt;
	}
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	if (isPicked) {
		section = { 0, 0, 26, 30 };
		//pbody->body->SetActive(false);
		////this->Disable();
		//if (pbody != nullptr) {
		//	app->physics->world->DestroyBody(pbody->body);
		//}
		//pbody = nullptr;
	}

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 10;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 14;

	app->render->DrawTexture(texture, position.x, position.y, &section);
	//CleanUp();
	

	return true;
}

bool Checkpoint::PostUpdate()
{

	// L07 TODO 4: Add a physics to an item - update the position of the object from the physics.  
	//app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

bool Checkpoint::CleanUp()
{
	return true;
}

void Checkpoint::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->cType)
	{
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		if (!isPicked) {
			saveSpot = true;
		}
		isPicked = true;

		//this->Disable();
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}
}