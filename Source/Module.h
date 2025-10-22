#pragma once

#include "Globals.h"
#include "box2d/box2d.h"
#include <string>
#include <memory>

class Application;

class PhysBody;

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
    virtual void EndCollision(PhysBody* bodyA, PhysBody* bodyB) {}
};
