#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "box2d/box2d.h"

#include "p2Point.h"

#include <math.h>



ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;
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

	return true;
}

update_status ModulePhysics::PreUpdate()
{
	if (world != nullptr)
	{
		float dt = GetFrameTime();
		world->Step(dt, 8, 3);
	}


	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, bool isSensor, ColliderType ctype, bodyType type)
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
	fixtureDef.restitution = 0.6f;
	b->CreateFixture(&fixtureDef);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->ctype = ctype;

	return pbody;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, ColliderType ctype, bodyType type)
{
	b2BodyDef bodyDef;
	if (type == DYNAMIC) bodyDef.type = b2_dynamicBody;
	else if (type == KINEMATIC) bodyDef.type = b2_kinematicBody;
	else bodyDef.type = b2_staticBody;

	bodyDef.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));

	b2Body* b = world->CreateBody(&bodyDef);

	b2CircleShape shape;
	shape.m_radius = PIXELS_TO_METERS(radius);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.friction = 0;
	fixtureDef.density = 1.0f;
	fixtureDef.restitution = 0.6f;

	b->CreateFixture(&fixtureDef);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->ctype = ctype;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, bodyType type)
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
	fixtureDef.density = 1.0f;
	b->CreateFixture(&fixtureDef);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;

	return pbody;
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

update_status ModulePhysics::PostUpdate()
{
	

	if (IsKeyPressed(KEY_F1))
	{
		debug = !debug;
	}

	if (!debug)
	{
		return UPDATE_CONTINUE;
	}

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					
					DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), (float)METERS_TO_PIXELS(shape->m_radius), Color{0, 0, 0, 128});
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->m_count;
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->m_vertices[i]);
						if(i > 0)
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

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
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