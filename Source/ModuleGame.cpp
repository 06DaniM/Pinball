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

    mPlayer = App->player;
    mapTexture = LoadTexture("Assets/Pinball table pokemon1.png");

    // === LOAD THE FLIPPERS ===
    Flippers(leftFlipper, leftFlipperJoint, SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT - 55, true);      // Left flipper
    Flippers(rightFlipper, rightFlipperJoint, SCREEN_WIDTH / 2 + 45, SCREEN_HEIGHT - 55, false);   // Right flipper

    // === LOAD THE SPRING
    Spring(base, plunger, joint, springGroundX, springGroundY);

    // === LOAD THE TABLE (MAP)
    CreateTable();
    CreateVoid();
    CreateScoreItems();

    return true;
}

update_status ModuleGame::Update()
{
    if (isLaunching) physTable->SetSensor(true);
    else physTable->SetSensor(false);

    HandleInput();
    Draw();

    return UPDATE_CONTINUE;
}

void ModuleGame::CreateTable()
{
    const int p = 102;

	static int points[p] = {
    30, 250,
    26, 260,
    22, 270,
    18, 300,
    15, 330,
	15, 400,
    20, 430,
    28, 460,
    40, 490,
    55, 520,
    85, 550,
    85, 590,
    55, 595,
    45, 600,
    40, 620,
    40, 718,
    180, 795,
    180, 1200,
    270, 1200,
    270, 795,
    410, 718,
    410, 620,
    405, 600,
    395, 595,
    365, 590,
    365, 550,
    395, 520,
    410, 490,
    422, 460,
    430, 430,
    435, 400,
    435, 330,
    430, 300,
    420, 270,
    410, 240,
    390, 210,
    372, 190,
    350, 170,
    320, 150,
    290, 140,
    260, 135,
    230, 130,
    210, 132,
    190, 135,
    160, 145,
    130, 155,
    100, 170,
    75, 190,
    68, 200,
    60, 210,
    50, 220,
	};
    
	physTable = App->physics->CreateChain(0, 0, points, p, false, ColliderType::PLATFORM);
	physTable->listener = this;

    CreateObstacles();
}

void ModuleGame::CreateObstacles()
{
    const int p1 = 6;

    static int leftTrianglePoints[p1] = {
        160, 690,
        124, 672,
        124, 632,
    };

    static int rightTrianglePoints[p1] = {
        324, 632,
        324, 672,
        290, 690,
    };

    leftTriangle = App->physics->CreateChain(0, 0, leftTrianglePoints, p1, false, ColliderType::PLATFORM);
    leftTriangle->listener = this;

    rightTriangle = App->physics->CreateChain(0, 0, rightTrianglePoints, p1, false, ColliderType::PLATFORM);
    rightTriangle->listener = this;

    const int p2 = 12;

    static int leftPlatformPoints[p2] = {
        82, 702,
        82, 632,
        92, 632,
        92, 690,
        168, 740,
        162, 755,
    };

    static int rightPlatformPoints[p2] = {
        288, 755,
        284, 740,
        356, 690,
        356, 632,
        366, 632,
        366, 702,
    };

    leftPlatform = App->physics->CreateChain(0, 0, leftPlatformPoints, p2, false, ColliderType::PLATFORM);
    leftPlatform->listener = this;

    rightPlatform = App->physics->CreateChain(0, 0, rightPlatformPoints, p2, false, ColliderType::PLATFORM);
    rightPlatform->listener = this;
}

void ModuleGame::CreateVoid()
{
    // Void to respawn (fijo, no es necesario cambiarlo)
    downVoidPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT + 100 };
    downVoid = App->physics->CreateRectangle(downVoidPos.x, downVoidPos.y, SCREEN_WIDTH, 50, true, this, ColliderType::VOID, STATIC);
}

void ModuleGame::CreateScoreItems()
{
    sumLife1 = App->physics->CreateCircle(164, 200, 10, true, ColliderType::SUMLIFE, STATIC);

    sumLife2 = App->physics->CreateCircle(204, 198, 10, true, ColliderType::SUMLIFE, STATIC);

    sumLife3 = App->physics->CreateCircle(243, 200, 10, true, ColliderType::SUMLIFE, STATIC);
}

void ModuleGame::HandleInput()
{
    // === SPRING / PLUNGER ===
    float pullLimit = -1.0f; // how far down the plunger can go

    if (IsKeyDown(KEY_DOWN))
    {
        // Extend how far the plunger can move down
        joint->SetLimits(pullLimit, 0.0f);

        // Apply downward force
        plunger->body->ApplyForceToCenter(b2Vec2(0, +5.0f), true);
    }

    if (IsKeyReleased(KEY_DOWN))
    {
        // Reset limit
        joint->SetLimits(-5.0f, 0.0f);

        // Launch upward
        plunger->body->ApplyLinearImpulseToCenter(b2Vec2(0, -5.0f), true);
    }



    // Left flipper
    if (IsKeyDown(KEY_LEFT)) leftFlipperJoint->SetMotorSpeed(-12.5f);
    else leftFlipperJoint->SetMotorSpeed(12.5f);

    // Right flipper
    if (IsKeyDown(KEY_RIGHT)) rightFlipperJoint->SetMotorSpeed(12.5f);
    else rightFlipperJoint->SetMotorSpeed(-12.5f);
}

void ModuleGame::Draw()
{
    DrawTexture(mapTexture, 0, 0, WHITE);
    int x, y;

    sumLife1->GetPosition(x, y);
    DrawCircle(x, y, 10, SKYBLUE);

    sumLife2->GetPosition(x, y);
    DrawCircle(x, y, 10, SKYBLUE);

    sumLife3->GetPosition(x, y);
    DrawCircle(x, y, 10, SKYBLUE);

    mPlayer->DrawBall();
    /*DrawCircle(leftFlipperPositionX, leftFlipperPositionY, 5, RED);
    DrawCircle(rightFlipperPositionX, rightFlipperPositionY, 5, RED);*/
}

bool ModuleGame::CleanUp()
{
    printf("Cleaning game scene\n");
    return true;
}

// MOVER LOS FLIPPERS Y EL SPRING AL MODULEPHYSICS
void ModuleGame::Flippers(PhysBody*& flipper, b2RevoluteJoint*& joint, float x, float y, bool isLeft)
{
    // Creates the flipper body
    float flipperWidth = 45.0f;
    float flipperHeight = 10.0f;

    // Creates the dynamic body
    flipper = App->physics->CreateRectangle(x, y, flipperWidth, flipperHeight, false, this, ColliderType::PLATFORM, DYNAMIC);

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

    // Desactivates the engine when the torque is being created
    jointDef.enableMotor = false;
    jointDef.maxMotorTorque = 500.0f;

    // Puts the anchor at the ends
    float halfWidth = PIXELS_TO_METERS(flipperWidth * 0.5f);
    if (isLeft) jointDef.localAnchorB.Set(-halfWidth, 0);
    else jointDef.localAnchorB.Set(halfWidth, 0);

    // Limitate the angle of turn
    jointDef.enableLimit = true;
    jointDef.lowerAngle = -30 * DEG2RAD;
    jointDef.upperAngle = 30 * DEG2RAD;

    // Creates the joint
    joint = (b2RevoluteJoint*)App->physics->world->CreateJoint(&jointDef);

    // Activates the engine when the flippers are in their position
    joint->EnableMotor(true); // The flippers move at the begining until they were in their positions, activate the engine after solve it
}

void ModuleGame::Spring(PhysBody*& base, PhysBody*& plunger, b2PrismaticJoint*& joint, float poxX, float posY) {

    float rectangleH = 20.0f;
    float rectangleW = 40.0f;
    float springH = 50.0f;

    b2Vec2 worldAxis(0.0f, -1.0f);

    base = App->physics->CreateRectangle(poxX, posY, rectangleW, rectangleH, false, this, ColliderType::PLATFORM, STATIC);
    plunger = App->physics->CreateRectangle(poxX, posY - springH, rectangleW, rectangleH, false, this, ColliderType::PLATFORM, DYNAMIC);
    
    b2PrismaticJointDef jointDef;
    jointDef.bodyA = base->body;
    jointDef.bodyB = plunger->body; 
    jointDef.Initialize(jointDef.bodyA, jointDef.bodyB, jointDef.bodyA->GetWorldCenter(), worldAxis);
    jointDef.enableLimit = true;
    jointDef.lowerTranslation = 0;
    jointDef.upperTranslation = 0;

    jointDef.enableMotor = false;

    joint = (b2PrismaticJoint*)App->physics->world->CreateJoint(&jointDef);
    plunger->body->SetGravityScale(0.0f);
}