#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

class ModuleGame;

class ModulePlayer : public Module
{
public:
    ModulePlayer(Application* app, bool start_enabled = true);
    ~ModulePlayer();

    bool Start();
    update_status Update();
    bool CleanUp();
    void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

    void DrawBall();

private:
    void GetPhysics();
    void Reset();
    void TeleportBallDebug();

public:
    ModuleGame* mGame = NULL;

    Texture2D ballTexture;

    // Teleport 
    bool needsReset = false;               // Declare if has to reset when falling
    float resetDelayTimer = 0.0f;          // Set the time left to reset
    const float resetDelayDuration = 0.5f; // Set the total time to reset

    // === LIFE === 
    int life = 0;               // Set the current life
    const int maxLife = 3;      // Set the life when starting a round
    int currentSumLife = 0;     // Set the the different number of times that has an item sumLife been hit

private:
    PhysBody* playerBody = nullptr; // Body of the ball
    float radius = 7.5f;                // Radius of the ball
    Vector2 position;               // Position of the ball
    Vector2 initialPosition;        // Initial position of the ball
};