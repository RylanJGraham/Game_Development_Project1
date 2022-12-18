#pragma once
#include "Module.h"
#include "Entity.h"

#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -10.0f

#define PIXELS_PER_METER 32.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.03125f // this is 1 / PIXELS_PER_METER !
#define RAD_PER_DEG ((float)180/b2_pi)
#define DEG_PER_RAD ((float)b2_pi/180)

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)
#define RAD_TO_DEG(r) ((float) RAD_PER_DEG * r)
#define DEG_TO_RAD(r) ((float) DEG_PER_RAD * r)
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

// types of bodies
enum bodyType {
	DYNAMIC,
	STATIC,
	KINEMATIC
};

enum class ColliderType {
	PLAYER,
	PLAYERATTACK,
	GROUNDENEMYHITBOX,
	AIRENEMYHITBOX,
	ITEM,
	AIRENEMY,
	GROUNDENEMY,
	ENEMY,
	GROUND,
	PLATFORM,
	WALL,
	UNKNOWN,
	DEATH
};

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL), cType(ColliderType::UNKNOWN)
	{}

	void GetPosition(int& x, int& y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int width, height;
	b2Body* body;
	Entity* listener;
	ColliderType cType;
};

// Module --------------------------------------
class Physics : public Module, public b2ContactListener // TODO
{
public:
	Physics();
	~Physics();

	bool Awake() { return true; }
	bool Start();
	bool PreUpdate();
	bool PostUpdate();
	bool CleanUp();

	PhysBody* CreateRectangle(int x, int y, int width, int height, bodyType type, ColliderType ctype);
	PhysBody* CreateCircle(int x, int y, int radious, bodyType type, ColliderType ctype);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, bodyType type, ColliderType ctype);
	PhysBody* CreateChain(int x, int y, int* points, int size, bodyType type, ColliderType ctype);
	PhysBody* CreateSensorChain(int x, int y, int* points, int size, bodyType type, ColliderType ctype);
	b2RevoluteJoint* CreateRevoluteJoint(PhysBody* A, b2Vec2 anchorA, PhysBody* B, b2Vec2 anchorB, float angle, bool collideConnected, bool enableLimit);
	b2PrismaticJoint* CreatePrismaticJoint(PhysBody* A, b2Vec2 anchorA, PhysBody* B, b2Vec2 anchorB, b2Vec2 axys, float maxHeight, bool collideConnected, bool enableLimit);
	b2WeldJoint* CreateWeldJoint(PhysBody* A, b2Vec2 anchorA, PhysBody* B, b2Vec2 anchorB, float angle, bool collideConnected, bool enableLimit);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

	b2World* world;
	bool debug = true;

private:

	b2MouseJoint* mouse_joint;
	b2Body* ground;
};