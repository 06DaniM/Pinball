#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "Player.h"

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleGame::~ModuleGame()
{
}

bool ModuleGame::Start()
{
    printf("Loading game assets\n");

    ball = App->player;
    CreateWalls();

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

void ModuleGame::CreateWalls()
{
    float restitution = 0.8f;

    // Left wall
    leftWallPos = { 100, SCREEN_HEIGHT / 2 };
    leftWall = App->physics->CreateRectangle(leftWallPos.x, leftWallPos.y, wallsSizeW, wallsSizeH, restitution, false, ColliderType::PLATFORM, STATIC);

    // Right wall
    rightWallPos = { SCREEN_WIDTH - 100, SCREEN_HEIGHT / 2 };
    rightWall = App->physics->CreateRectangle(rightWallPos.x, rightWallPos.y, wallsSizeW, wallsSizeH, restitution, false, ColliderType::PLATFORM, STATIC);

    // Void to respawn
    downVoidPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT + 100 };
    downVoid = App->physics->CreateRectangle(downVoidPos.x, downVoidPos.y, SCREEN_WIDTH, 50, true, 0.0f,ColliderType::VOID, STATIC);

    // Launch zone
    startleftWall = App->physics->CreateRectangle(SCREEN_WIDTH - 180, 600, 50, 600, restitution, false, ColliderType::PLATFORM, STATIC);
    startGround = App->physics->CreateRectangle(SCREEN_WIDTH - 140, 800, 30, 50, restitution, false, ColliderType::PLATFORM, STATIC);

    // Create the arc
    CreateTopArc();
}

void ModuleGame::CreateTopArc()
{

}

void ModuleGame::DrawTable()
{
    DrawWall(leftWall, BLACK);
    DrawWall(rightWall, BLACK);
    App->renderer->DrawRectangleCentered(SCREEN_WIDTH - 180, 600, 50, 600, DARKBLUE);
    App->renderer->DrawRectangleCentered(SCREEN_WIDTH - 140, 800, 30, 50, RED);

    DrawTopArc();
}

void ModuleGame::DrawWall(PhysBody* wall, Color color)
{
    if (!wall) return;
    int x, y;
    wall->GetPosition(x, y);
    App->renderer->DrawRectangleCentered(x, y, wallsSizeW, wallsSizeH, color);
}

void ModuleGame::DrawTopArc()
{

}

void ModuleGame::HandleInput()
{
    // Disparar la bola
    if (IsKeyPressed(KEY_DOWN))
    {
        ball->Launch();
    }

    // Mover flippers (aún no implementado)
    if (IsKeyPressed(KEY_LEFT))
    {
        // Flipper izquierdo
    }

    if (IsKeyPressed(KEY_RIGHT))
    {
        // Flipper derecho
    }
}
