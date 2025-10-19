#pragma once

#include "Module.h"
#include "Globals.h"
#include "box2d/box2d.h"

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

    PhysBody* CreateRectangle(int x, int y, int width, int height, bool isSensor, ColliderType ctype, bodyType type);
    PhysBody* CreateCircle(int x, int y, int radius, ColliderType ctype = ColliderType::UNKNOWN, bodyType type = bodyType::DYNAMIC);
    PhysBody* CreateChain(int x, int y, int* points, int size, bodyType type = bodyType::STATIC);

    void   SetBodyPosition(PhysBody* pbody, int x, int y, bool resetRotation);

    b2World* world = nullptr;

private:
    bool debug = true;
};
