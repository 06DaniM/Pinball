#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleGame::~ModuleGame()
{
}

bool ModuleGame::Start()
{
    printf("Loading game assets\n");

    // Referencia a la bola
    ball = App->player; // Reference to the ball
    return true;
}

update_status ModuleGame::Update()
{
    HandleInput();
    DrawTable();

    return UPDATE_CONTINUE;
}

bool ModuleGame::CleanUp()
{
    printf("Cleaning game scene\n");
    return true;
}

void ModuleGame::HandleInput()
{
    // Shoot the ball
    if (IsKeyPressed(KEY_DOWN))
    {
        ball->Launch(initialForce);
    }

    // Moving the flippers
    if (IsKeyPressed(KEY_LEFT))
    {
        // Left flipper
    }

    if (IsKeyPressed(KEY_LEFT))
    {
        // Right flipper
    }
}

void ModuleGame::DrawTable()
{
    // Draw the map
}
