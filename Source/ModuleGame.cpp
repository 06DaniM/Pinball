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

    ball = App->player; // Reference to the ball

    leftWallPos.x = 100;
    leftWallPos.y = SCREEN_HEIGHT / 2;
    leftWall = App->physics->CreateRectangle(leftWallPos.x, leftWallPos.y, wallsSizeW, wallsSizeH, false, ColliderType::PLATFORM, STATIC);

    rightWallPos.x = SCREEN_WIDTH - 100;
    rightWallPos.y = SCREEN_HEIGHT / 2;
    rightWall = App->physics->CreateRectangle(rightWallPos.x, rightWallPos.y, wallsSizeW, wallsSizeH, false, ColliderType::PLATFORM, STATIC);

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
        ball->Launch();
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
    App->renderer->DrawRectangleCentered(leftWallPos.x, leftWallPos.y, wallsSizeW, wallsSizeH, BLACK);
    App->renderer->DrawRectangleCentered(rightWallPos.x, rightWallPos.y, wallsSizeW, wallsSizeH, BLACK);
}