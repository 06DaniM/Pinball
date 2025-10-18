#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

class ModulePlayer : public Module
{
public:
    ModulePlayer(Application* app, bool start_enabled = true);
    ~ModulePlayer();

    bool Start();
    update_status Update();
    bool CleanUp();

    void Launch(float initialForce);
    void Reset();

private:
    PhysBody* playerBody = nullptr; // Body of the ball
    int radius = 12;                // Radius of the ball
    Vector2 position;               // Position of the ball
    Vector2 initialPosition;        // Initial position of the ball
};