#include "Item.h"
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

Item::Item() : Entity(EntityType::ITEM)
{
	name.Create("item");
}

Item::~Item() {}

bool Item::Awake() {



	return true;
}

bool Item::Start() {

	//initilize textures

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	texture = app->tex->Load(texturePath);
	
	// L07 DONE 4: Add a physics to an item - initialize the physics body
	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC, ColliderType::ITEM);
	pbody->cType = ColliderType::ITEM;
	pbody->listener = this;
	isPicked = false;

	return true;
}

bool Item::Update(float dt)
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	if (!isPicked) {
		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 18;
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 10;

		app->render->DrawTexture(texture, position.x, position.y);
	}
	else{
		pbody->body->SetActive(false);
		this->Disable();
		if (pbody != nullptr) {
		app->physics->world->DestroyBody(pbody->body);
		}
		app->fade->FadeBlack((Module*)app->scene, (Module*)app->titleScreen, 60 * (16.0f / dt));
	pbody = nullptr;
	CleanUp();
	}

	return true;
}

bool Item::PostUpdate()
{

	// L07 TODO 4: Add a physics to an item - update the position of the object from the physics.  
	app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

bool Item::CleanUp()
{
	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->cType)
	{
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
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