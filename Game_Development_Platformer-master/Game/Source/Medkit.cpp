#include "Medkit.h"
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

Medkit::Medkit() : Entity(EntityType::MEDKIT)
{
	name.Create("medkit");
}

Medkit::~Medkit() {}

bool Medkit::Awake() {



	return true;
}

bool Medkit::Start() {

	//initilize textures

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	texture = app->tex->Load(texturePath);

	// L07 DONE 4: Add a physics to an item - initialize the physics body
	pbody = app->physics->CreateCircle(position.x + 12, position.y + 12, 12, bodyType::STATIC, ColliderType::MEDKIT);
	pbody->cType = ColliderType::MEDKIT;
	pbody->listener = this;
	isPicked = false;

	return true;
}

bool Medkit::Update(float dt)
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	if (!isPicked) {
		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 8;
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 12;

		app->render->DrawTexture(texture, position.x, position.y);
	}
	else {
		pbody->body->SetActive(false);
		this->Disable();
		if (pbody != nullptr) {
			app->physics->world->DestroyBody(pbody->body);
		}

		pbody = nullptr;
		CleanUp();
	}

	return true;
}

bool Medkit::PostUpdate()
{

	// L07 TODO 4: Add a physics to an item - update the position of the object from the physics.  
	app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

bool Medkit::CleanUp()
{
	return true;
}

void Medkit::OnCollision(PhysBody* physA, PhysBody* physB)
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