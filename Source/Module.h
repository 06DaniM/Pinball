#pragma once

#include "Globals.h"
#include "box2d/box2d.h"
#include <string>
#include <memory>

class Application;

enum bodyType {
    DYNAMIC,
    STATIC,
    KINEMATIC
};

enum class ColliderType {
    PLAYER,
    PLATFORM,
    ITEM,
    VOID,
    UNKNOWN
};

class PhysBody
{
public:
    PhysBody() : body(nullptr), ctype(ColliderType::UNKNOWN) {}
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
};

class Module
{
private:
    bool enabled;

public:
    Application* App;

    Module(Application* parent, bool start_enabled = true)
        : App(parent), enabled(start_enabled) {
    }

    virtual ~Module() {}

    bool IsEnabled() const { return enabled; }

    void Enable()
    {
        if (!enabled)
        {
            enabled = true;
            Start();
        }
    }

    void Disable()
    {
        if (enabled)
        {
            enabled = false;
            CleanUp();
        }
    }

    virtual bool Init() { return true; }
    virtual bool Start() { return true; }
    virtual update_status PreUpdate() { return UPDATE_CONTINUE; }
    virtual update_status Update() { return UPDATE_CONTINUE; }
    virtual update_status PostUpdate() { return UPDATE_CONTINUE; }
    virtual bool CleanUp() { return true; }

    virtual void OnCollision(PhysBody* bodyA, PhysBody* bodyB) {}
};
