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

    void GetPhysics();
    void DrawBall();
    void Launch();
    void Reset();

    // Teleport 
    bool needsReset = false;          // Indica si el jugador debe resetearse
    float resetDelay = 0.0f;          // Tiempo restante antes de teletransportar
    const float resetDelayDuration = 0.5f; // Duración del retraso (en segundos)

private:
    PhysBody* playerBody = nullptr; // Body of the ball
    int radius = 12;                // Radius of the ball
    Vector2 position;               // Position of the ball
    Vector2 initialPosition;        // Initial position of the ball
};