#include "Module.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "box2d/box2d.h"

#include "p2Point.h"

#include <math.h>

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = false;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	printf("Creating Physics 2D environment\n");

	b2Vec2 gravity(0.0f, 10.0f);
	world = new b2World(gravity);
	world->SetContactListener(this);

	b2BodyDef bd;
	bd.type = b2_staticBody;
	ground = world->CreateBody(&bd);

	return true;
}

update_status ModulePhysics::PreUpdate()
{
	if (world != nullptr)
	{
		float dt = GetFrameTime();
		world->Step(dt, 6, 2);
	}

	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			b2BodyUserData data1 = c->GetFixtureA()->GetBody()->GetUserData();
			b2BodyUserData data2 = c->GetFixtureA()->GetBody()->GetUserData();

			PhysBody* pb1 = (PhysBody*)data1.pointer;
			PhysBody* pb2 = (PhysBody*)data2.pointer;
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}


	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, bool isSensor, Module* listener, ColliderType ctype, bodyType type)
{
	b2BodyDef bodyDef;
	if (type == DYNAMIC) bodyDef.type = b2_dynamicBody;
	else if (type == KINEMATIC) bodyDef.type = b2_kinematicBody;
	else bodyDef.type = b2_staticBody;
	bodyDef.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));

	b2Body* b = world->CreateBody(&bodyDef);

	b2PolygonShape box;
	box.SetAsBox(PIXELS_TO_METERS(width * 0.5f), PIXELS_TO_METERS(height * 0.5f));

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.isSensor = isSensor;
	fixtureDef.friction = 0;
	fixtureDef.density = 1.0f;

	if (ctype == ColliderType::BOUNCE)
		fixtureDef.restitution = 1.0f;

	else if (ctype == ColliderType::PLUNGER)
		fixtureDef.restitution = 0.8f;

	b->CreateFixture(&fixtureDef);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->ctype = ctype;
	pbody->listener = listener;

	b->GetUserData().pointer = (uintptr_t)pbody;

	return pbody;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, bool isSensor, Module* listener, ColliderType ctype, bodyType type)
{
	b2BodyDef bodyDef;
	if (type == DYNAMIC) bodyDef.type = b2_dynamicBody;
	else if (type == KINEMATIC) bodyDef.type = b2_kinematicBody;
	else bodyDef.type = b2_staticBody;

	bodyDef.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));
	bodyDef.angularDamping = 0.0f;

	b2Body* b = world->CreateBody(&bodyDef);

	b2CircleShape shape;
	shape.m_radius = PIXELS_TO_METERS(radius);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.isSensor = isSensor;
	fixtureDef.friction = 0.3f;
	fixtureDef.density = 1.0f;

	if (ctype == ColliderType::BOUNCE || ctype == ColliderType::EGG)
		fixtureDef.restitution = 1.0f;
	else
		fixtureDef.restitution = 0.6f;

	b->CreateFixture(&fixtureDef);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->ctype = ctype;
	pbody->listener = listener;

	b->GetUserData().pointer = (uintptr_t)pbody;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, bool isSensor, ColliderType ctype, bodyType type)
{
	b2BodyDef bodyDef;
	if (type == DYNAMIC) bodyDef.type = b2_dynamicBody;
	else if (type == KINEMATIC) bodyDef.type = b2_kinematicBody;
	else bodyDef.type = b2_staticBody;

	bodyDef.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));
	b2Body* b = world->CreateBody(&bodyDef);

	const int count = size / 2;
	std::vector<b2Vec2> verts(count);
	for (int i = 0; i < count; ++i)
	{
		verts[i].x = PIXELS_TO_METERS(points[i * 2 + 0]);
		verts[i].y = PIXELS_TO_METERS(points[i * 2 + 1]);
	}

	b2ChainShape chain;
	chain.CreateLoop(verts.data(), count);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &chain;
	fixtureDef.isSensor = isSensor;
	fixtureDef.density = 1.0f;

	if (ctype == ColliderType::BOUNCE)
		fixtureDef.restitution = 1.0f;

	b->CreateFixture(&fixtureDef);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;

	b->GetUserData().pointer = (uintptr_t)pbody;

	return pbody;
}

void ModulePhysics::Flippers(PhysBody*& flipper, b2RevoluteJoint*& joint, float x, float y, bool isLeft)
{
	// Creates the flipper body
	float flipperWidth = 45.0f;
	float flipperHeight = 10.0f;

	// Creates the dynamic body
	flipper = CreateRectangle(x, y, flipperWidth, flipperHeight, false, this, ColliderType::PLATFORM, DYNAMIC);

	// Creates the pivot point
	b2Body* body = flipper->body;

	b2BodyDef anchorDef;
	anchorDef.type = b2_staticBody;
	anchorDef.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));
	b2Body* anchor = world->CreateBody(&anchorDef);

	// Creates the revolute joint
	b2RevoluteJointDef jointDef;
	jointDef.bodyA = anchor;
	jointDef.bodyB = body;

	// Desactivates the engine when the torque is being created
	jointDef.enableMotor = false;
	jointDef.motorSpeed = 0.0f;
	jointDef.maxMotorTorque = 350.0f;

	// Puts the anchor at the ends
	float halfWidth = PIXELS_TO_METERS(flipperWidth * 0.5f);
	if (isLeft) jointDef.localAnchorB.Set(-halfWidth, 0);
	else jointDef.localAnchorB.Set(halfWidth, 0);

	// Limitate the angle of turn
	jointDef.enableLimit = true;
	if (isLeft)
	{
		jointDef.lowerAngle = -15 * DEG2RAD;
		jointDef.upperAngle = 30 * DEG2RAD;
	}
	else
	{
		jointDef.lowerAngle = -30 * DEG2RAD;
		jointDef.upperAngle = 15 * DEG2RAD;
	}

	// Creates the joint
	joint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);

	// Activates the engine when the flippers are in their position
	joint->EnableMotor(true); // The flippers move at the begining until they were in their positions, activate the engine after solve it
}

void ModulePhysics::Spring(PhysBody*& base, PhysBody*& plunger, b2PrismaticJoint*& joint, float poxX, float posY) {

	float rectangleH = 20.0f;
	float rectangleW = 40.0f;
	float springH = 40.0f;

	// Y axis points downward
	b2Vec2 worldAxis(0.0f, 1.0f);

	base = CreateRectangle(poxX, posY, rectangleW, rectangleH, false, this, ColliderType::PLATFORM, STATIC);
	plunger = CreateRectangle(poxX, posY - springH, rectangleW, rectangleH, false, this, ColliderType::PLUNGER, DYNAMIC);

	b2PrismaticJointDef jointDef;
	jointDef.Initialize(base->body, plunger->body, base->body->GetWorldCenter(), worldAxis);

	jointDef.enableLimit = true;
	jointDef.lowerTranslation = 0.0f;     // top position (rest)
	jointDef.upperTranslation = 0.4f;     // how far it can go down

	jointDef.enableMotor = true;
	jointDef.maxMotorForce = 500.0f;
	jointDef.motorSpeed = 0.0f;

	joint = (b2PrismaticJoint*)world->CreateJoint(&jointDef);

	plunger->body->SetGravityScale(0.0f);
}

void ModulePhysics::SetBodyPosition(PhysBody* pbody, int x, int y, bool resetRotation)
{
	if (pbody == nullptr || pbody->body == nullptr)
		return;

	b2Vec2 newPos(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));
	float angle = resetRotation ? 0.0f : pbody->body->GetAngle();

	pbody->body->SetTransform(newPos, angle);
	pbody->body->SetLinearVelocity(b2Vec2(0, 0));
	pbody->body->SetAngularVelocity(0);
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	b2BodyUserData dataA = contact->GetFixtureA()->GetBody()->GetUserData();
	b2BodyUserData dataB = contact->GetFixtureB()->GetBody()->GetUserData();

	PhysBody* physA = (PhysBody*)dataA.pointer;
	PhysBody* physB = (PhysBody*)dataB.pointer;

	if (physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}

void ModulePhysics::EndContact(b2Contact* contact)
{
	b2BodyUserData dataA = contact->GetFixtureA()->GetBody()->GetUserData();
	b2BodyUserData dataB = contact->GetFixtureB()->GetBody()->GetUserData();

	PhysBody* physA = (PhysBody*)dataA.pointer;
	PhysBody* physB = (PhysBody*)dataB.pointer;

	if (physA && physA->listener != NULL)
		physA->listener->EndCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->EndCollision(physB, physA);
}

update_status ModulePhysics::PostUpdate()
{
	if (IsKeyPressed(KEY_F1))
	{
		debug = !debug;
	}

	if (!debug)
		return UPDATE_CONTINUE;

	Vector2 mouse = GetMousePosition();
	b2Vec2 pMousePosition = b2Vec2(PIXELS_TO_METERS(mouse.x), PIXELS_TO_METERS(mouse.y));

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
		{
			if (b->GetType() != b2_dynamicBody) continue;

			for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			{
				if (f->TestPoint(pMousePosition))
				{
					mouseSelect = b;

					b2MouseJointDef def;
					def.bodyA = ground;
					def.bodyB = mouseSelect;
					def.target = pMousePosition;
					def.damping = 0.7f;
					def.stiffness = 20.f;
					def.maxForce = 100.0f * mouseSelect->GetMass();

					mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
					mouseSelect->SetAwake(true);
					break;
				}
			}
		}
	}

	if (mouse_joint && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		mouse_joint->SetTarget(pMousePosition);

		// Dibujar la línea entre el mouse y el cuerpo
		b2Vec2 anchor = mouse_joint->GetBodyB()->GetPosition();
		DrawLine(
			METERS_TO_PIXELS(anchor.x),
			METERS_TO_PIXELS(anchor.y),
			mouse.x,
			mouse.y,
			RED
		);
	}

	if (mouse_joint && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
	{
		world->DestroyJoint(mouse_joint);
		mouse_joint = nullptr;
		mouseSelect = nullptr;
	}

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();

				DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), (float)METERS_TO_PIXELS(shape->m_radius), Color{ 0, 0, 0, 128 });
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->m_count;
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->m_vertices[i]);
					if (i > 0)
						DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->m_vertices[0]);
				DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)
						DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), BLACK);
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), BLACK);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), BLUE);
			}
			break;
			}
		}
	}

	return UPDATE_CONTINUE;
}



// Called before quitting
bool ModulePhysics::CleanUp()
{
	printf("Destroying physics world\n");

	if (world != nullptr)
	{
		delete world;
		world = nullptr;
	}

	return true;
}