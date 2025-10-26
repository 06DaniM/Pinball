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
    mapTexture = LoadTexture("Assets/Pinball table pokemon.png");

    // === LOAD THE FLIPPERS ===
    Flippers(leftFlipper, leftFlipperJoint, SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT - 55, true);      // Left flipper
    Flippers(rightFlipper, rightFlipperJoint, SCREEN_WIDTH / 2 + 45, SCREEN_HEIGHT - 55, false);   // Right flipper

    // === LOAD THE SPRING
    Spring(base, plunger, joint, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 60);

    // === LOAD THE TABLE (MAP)
    CreateTable();
    CreateVoid();
    CreateItems();

    return true;
}

update_status ModuleGame::Update()
{
    HandleInput();
    Draw();

    return UPDATE_CONTINUE;
}

void ModuleGame::CreateTable()
{
    const int p = 92;
    // Cambiar cuando se tenga el mapa
	static int points[p] = {
    20, 270,
    20, 300,
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
    70, 190,
	};
    
	physTable = App->physics->CreateChain(0, 0, points, p, false, ColliderType::PLATFORM);
	physTable->listener = this;
}

void ModuleGame::CreateVoid()
{
    // Void to respawn (fijo, no es necesario cambiarlo)
    downVoidPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT + 100 };
    downVoid = App->physics->CreateRectangle(downVoidPos.x, downVoidPos.y, SCREEN_WIDTH, 50, true, this, ColliderType::VOID, STATIC);
}

void ModuleGame::CreateItems()
{
    /*PhysBody* item1 = App->physics->CreateCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 5, true, ColliderType::ITEM, STATIC);
    item1->itemScore = 100;

    PhysBody* item2 = App->physics->CreateCircle(SCREEN_WIDTH / 2 + 100, SCREEN_HEIGHT / 2, 5, true, ColliderType::ITEM, STATIC);
    item2->itemScore = 500;*/
}

void ModuleGame::HandleInput()
{
    if (IsKeyDown(KEY_DOWN)) {
        // Pull down the spring
        plunger->body->ApplyForceToCenter(b2Vec2(0, +10), true);
    }

    if (IsKeyReleased(KEY_DOWN)) {
        // Release spring and propel ball
        plunger->body->ApplyForceToCenter(b2Vec2(0, -2000), true);
    }

    // Left flipper
    if (IsKeyDown(KEY_LEFT)) leftFlipperJoint->SetMotorSpeed(-10.0f);
    else leftFlipperJoint->SetMotorSpeed(10.0f);

    // Right flipper
    if (IsKeyDown(KEY_RIGHT)) rightFlipperJoint->SetMotorSpeed(10.0f);
    else rightFlipperJoint->SetMotorSpeed(-10.0f);
}

void ModuleGame::Draw()
{
    DrawTexture(mapTexture, 0, 0, WHITE);

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
    float springH = 80.0f;

    b2Vec2 worldAxis(0.0f, 1.0f);

    base = App->physics->CreateRectangle(poxX, posY, rectangleW, rectangleH, false, this, ColliderType::PLATFORM, STATIC);
    plunger = App->physics->CreateRectangle(poxX, posY - rectangleW, rectangleW, rectangleH, false, this, ColliderType::PLATFORM, DYNAMIC);
    
    b2PrismaticJointDef jointDef;
    jointDef.bodyA = base->body;
    jointDef.bodyB = plunger->body; 
    jointDef.Initialize(jointDef.bodyA, jointDef.bodyB, jointDef.bodyA->GetWorldCenter(), worldAxis);
    jointDef.enableLimit = true;
    jointDef.lowerTranslation = -5.0f;
    jointDef.upperTranslation = 0;



    joint = (b2PrismaticJoint*)App->physics->world->CreateJoint(&jointDef);

}