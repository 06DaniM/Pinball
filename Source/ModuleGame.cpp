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
    //Load spring
    Spring(base, plunger, joint, springGroundX, springGroundY);
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
    10, 980,
    910, 980,
    910, 0
	};
    
	physTable = App->physics->CreateChain(0, 0, points, 8, false, ColliderType::PLATFORM);
	physTable->listener = this;
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
    if (isLeft)
        jointDef.localAnchorB.Set(-halfWidth, 0);
    else
        jointDef.localAnchorB.Set(halfWidth, 0);

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
    float rectangleW = 60.0f;
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