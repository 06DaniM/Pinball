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
    void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

private:
    void GetPhysics();
    void DrawBall();
    void Reset();
    void TeleportBallDebug();

public:

    // Teleport 
    bool needsReset = false;               // Declare if has to reset when falling
    float resetDelayTimer = 0.0f;               // Set the time left to reset
    const float resetDelayDuration = 0.5f; // Set the total time to reset

    // === LIFE === 
    int life = 0;
    const int maxLife = 3;

private:
    PhysBody* playerBody = nullptr; // Body of the ball
    int radius = 12;                // Radius of the ball
    Vector2 position;               // Position of the ball
    Vector2 initialPosition;        // Initial position of the ball
};