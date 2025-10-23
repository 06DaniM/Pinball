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

    // Load flippers
    Flippers(leftFlipper, leftFlipperJoint, leftFlipperPositionX, leftFlipperPositionY, true);      // Left flipper
    Flippers(rightFlipper, rightFlipperJoint, rightFlipperPositionX, rightFlipperPositionY, false);   // Right flipper

    // Load table
    CreateWalls(); // momentaneo
	CreateTable();


    ball = App->player;

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

void ModuleGame::HandleInput()
{
    if (IsKeyPressed(KEY_DOWN))
        ball->Launch();

    // Left flipper
    if (IsKeyDown(KEY_LEFT))
        leftFlipperJoint->SetMotorSpeed(-10.0f);
    else
        leftFlipperJoint->SetMotorSpeed(10.0f);

    // Right flipper
    if (IsKeyDown(KEY_RIGHT))
        rightFlipperJoint->SetMotorSpeed(10.0f);
    else
        rightFlipperJoint->SetMotorSpeed(-10.0f);
}

void ModuleGame::DrawTable()
{
    DrawWall(rightWall, BLACK);
    App->renderer->DrawRectangleCentered(SCREEN_WIDTH - 180, 600, 50, 600, DARKBLUE);
    App->renderer->DrawRectangleCentered(SCREEN_WIDTH - 140, 800, 30, 50, RED);

    /*DrawCircle(leftFlipperPositionX, leftFlipperPositionY, 5, RED);
    DrawCircle(rightFlipperPositionX, rightFlipperPositionY, 5, RED);*/
}

void ModuleGame::DrawWall(PhysBody* wall, Color color)
{
    if (!wall) return;
    int x, y;
    wall->GetPosition(x, y);
    App->renderer->DrawRectangleCentered(x, y, wallsSizeW, wallsSizeH, color);
}

void ModuleGame::Flippers(PhysBody*& flipper, b2RevoluteJoint*& joint, float x, float y, bool isLeft)
{
    // Creates the flipper body
    float flipperWidth = 80.0f;
    float flipperHeight = 20.0f;

    // Creates the dynamic body
    flipper = App->physics->CreateRectangle(x, x, flipperWidth, flipperHeight, false, this, ColliderType::PLATFORM, DYNAMIC);

    // Creates the pivot point
    b2Body* body = flipper->body;

    b2BodyDef anchorDef;
    anchorDef.type = b2_staticBody;
    anchorDef.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));
    b2Body* anchor = App->physics->world->CreateBody(&anchorDef);

    // Creates the revolute joint
    b2RevoluteJointDef jointDef;
    jointDef.bodyA = anchor;
    jointDef.bodyB = body;
    jointDef.enableMotor = true;
    jointDef.maxMotorTorque = 500.0f;

    // To put the anchors to the ends
    float halfWidth = PIXELS_TO_METERS(flipperWidth * 0.5f);

    // Limites the angle that can turn
    jointDef.enableLimit = true;

    if (isLeft)
    {
        jointDef.lowerAngle = -30 * DEG2RAD;
        jointDef.upperAngle = 30 * DEG2RAD;
        jointDef.localAnchorB.Set(-halfWidth, 0);
    }
    else
    {
        jointDef.lowerAngle = -30 * DEG2RAD;
        jointDef.upperAngle = 30 * DEG2RAD;
        jointDef.localAnchorB.Set(halfWidth, 0);
    }

    joint = (b2RevoluteJoint*)App->physics->world->CreateJoint(&jointDef);
}
