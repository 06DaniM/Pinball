#pragma once

#include "Module.h"
#include "Globals.h"
#include "box2d/box2d.h"

enum bodyType {
    DYNAMIC,
    STATIC,
    KINEMATIC
};

enum class ColliderType {
    PLAYER,
    PLATFORM,
    ITEM,
    SUMLIFE,
    BOUNCE,
    VOID,
    UNKNOWN
};

class PhysBody
{
public:
    PhysBody() : body(nullptr), listener(nullptr), ctype(ColliderType::UNKNOWN) {}
    ~PhysBody() {}

    void GetPosition(int& x, int& y) const
    {
        if (body)
        {
            b2Vec2 pos = body->GetPosition();
            x = METERS_TO_PIXELS(pos.x);
            y = METERS_TO_PIXELS(pos.y);
        }
    }

    float GetRotation() const
    {
        return body ? RADTODEG * body->GetAngle() : 0.0f;
    }

    bool Contains(int x, int y) const
    {
        if (!body) return false;

        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext())
        {
            if (f->GetShape()->TestPoint(body->GetTransform(), b2Vec2(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y))))
                return true;
        }
        return false;
    }

public:
    b2Body* body = nullptr;
    ColliderType ctype;
    Module* listener;
    int itemScore;
    int isActive = true;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener
{
public:
    ModulePhysics(Application* app, bool start_enabled = true);
    ~ModulePhysics();

    bool Start() override;
    update_status PreUpdate() override;
    update_status PostUpdate() override;
    bool CleanUp() override;

    PhysBody* CreateRectangle(int x, int y, int width, int height, bool isSensor, Module* listener, ColliderType ctype, bodyType type);
    PhysBody* CreateCircle(int x, int y, int radius, bool isSensor, ColliderType ctype = ColliderType::UNKNOWN, bodyType type = bodyType::DYNAMIC);
    PhysBody* CreateChain(int x, int y, int* points, int size, bool isSensor, ColliderType ctype = ColliderType::UNKNOWN, bodyType type = bodyType::STATIC);

    void SetBodyPosition(PhysBody* pbody, int x, int y, bool resetRotation);

    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);

    b2World* world = nullptr;

private:
    bool debug = true;
};
