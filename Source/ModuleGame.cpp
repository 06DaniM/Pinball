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
	CreateTable();

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
    // Right wall (MOMENTANEO, En cuanto se tenga el mapa se quitará y se sustituirá por el CreateChain)
    rightWallPos = { SCREEN_WIDTH - 100, SCREEN_HEIGHT / 2 };
    rightWall = App->physics->CreateRectangle(rightWallPos.x, rightWallPos.y, wallsSizeW, wallsSizeH, false, this, ColliderType::PLATFORM, STATIC);

    // Void to respawn (fijo, no es necesario cambiarlo)
    downVoidPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT + 100 };
    downVoid = App->physics->CreateRectangle(downVoidPos.x, downVoidPos.y, SCREEN_WIDTH, 50, true, this, ColliderType::VOID, STATIC);

    // Launch zone (MOMENTANEO, En cuanto se tenga el mapa se quitará y se sustituirá por el CreateChain)
    startleftWall = App->physics->CreateRectangle(SCREEN_WIDTH - 180, 600, 50, 600, false, this, ColliderType::PLATFORM, STATIC);
    startGround = App->physics->CreateRectangle(SCREEN_WIDTH - 140, 800, 30, 50, false, this, ColliderType::PLATFORM, STATIC);
}

void ModuleGame::CreateTable()
{
    // Cambiar cuando se tenga el mapa
	static int points[8] = {
	10, 0,
    10, 800,
    910, 800,
    910, 0
	};
    
	physTable = App->physics->CreateChain(0, 0, points, 8, false, ColliderType::PLATFORM);
	physTable->listener = this;
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
