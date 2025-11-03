#include "Globals.h"
#include "Application.h"
#include "Animation.h"
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

    InitializeTextures();

    // === LOAD THE FLIPPERS ===
    Flippers(leftFlipper, leftFlipperJoint, leftFlipperPositionX, leftFlipperPositionY, true);      // Left flipper
    Flippers(rightFlipper, rightFlipperJoint, rightFlipperPositionX, rightFlipperPositionY, false);   // Right flipper

    // === LOAD THE SPRING ===
    Spring(base, plunger, joint, springGroundX, springGroundY);

    // === LOAD THE TABLE (MAP) ===
    CreateTable();

    CreateSlide();
    CreateObjects();
    CreateObstacles();

    CreateVoid();
    CreateScoreItems();

    return true;
}

update_status ModuleGame::Update()
{
    float deltaTime = GetFrameTime();
    coroutineManager.Update(deltaTime);

    if (isLaunching) physTable->SetSensor(true);
    else physTable->SetSensor(false);

    HandleInput();
    Draw();

    return UPDATE_CONTINUE;
}

void ModuleGame::InitializeTextures()
{
    mapTexture = LoadTexture("Assets/Pinball table pokemon1.png");

    leftFlipperTexture = LoadTexture("Assets/LeftFlipperSprite.png");
    rightFlipperTexture = LoadTexture("Assets/RightFlipperSprite.png");

    spoinkTexture = LoadTexture("Assets/Spoink_Spritesheet.png");
    shroomish = LoadTexture("Assets/Shroomish_Spritesheet.png");
    whailLordTexture = LoadTexture("Assets/Whailord_Spritesheet.png");

    spoinkAnim = Animator(&spoinkTexture, 20, 40);
    spoinkAnim.AddAnim("idle", 0, 2, 2.0f, true);
    spoinkAnim.AddAnim("spring_down", 2, 5, 5, false);
    spoinkAnim.Play("idle");

    shroomishAnim1 = Animator(&shroomish, 27, 31);
    shroomishAnim1.AddAnim("idle", 0, 2, 2.0f, true);
    shroomishAnim1.AddAnim("hitted", 2, 2, 6.0f, false);
    shroomishAnim1.Play("idle");

    shroomishAnim2 = Animator(&shroomish, 27, 31);
    shroomishAnim2.AddAnim("idle", 0, 2, 2.0f, true);
    shroomishAnim2.AddAnim("hitted", 2, 2, 6.0f, false);
    shroomishAnim2.Play("idle");

    shroomishAnim3 = Animator(&shroomish, 27, 31);
    shroomishAnim3.AddAnim("idle", 0, 2, 2.0f, true);
    shroomishAnim3.AddAnim("hitted", 2, 2, 6.0f, false);
    shroomishAnim3.Play("idle");

    whailordAnim = Animator(&whailLordTexture, 40, 30);
    whailordAnim.AddAnim("idle", 0, 2, 2.0f, true);
    whailordAnim.AddAnim("pickingBall", 0, 5, 8.0f, false);
    whailordAnim.AddAnim("throwingBall", 5, 3, 8.0f, false);
    whailordAnim.Play("idle");
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
    
	physTable = App->physics->CreateChain(0, 0, points, p, false, ColliderType::PLATFORM, STATIC);
	physTable->listener = this;
}

void ModuleGame::CreateSlide()
{
    const int p = 8;

    static int slide[p] = {
        78, 456,
        50, 400,
        55, 400,
        84, 456,
    };

    physSlide = App->physics->CreateChain(0, 0, slide, p, false, ColliderType::PLATFORM, STATIC);
    physSlide->listener = this;
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

    leftTriangle = App->physics->CreateChain(0, 0, leftTrianglePoints, p1, false, ColliderType::BOUNCE, STATIC);
    leftTriangle->listener = this;

    rightTriangle = App->physics->CreateChain(0, 0, rightTrianglePoints, p1, false, ColliderType::BOUNCE, STATIC);
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

    leftPlatform = App->physics->CreateChain(0, 0, leftPlatformPoints, p2, false, ColliderType::PLATFORM, STATIC);
    leftPlatform->listener = this;

    rightPlatform = App->physics->CreateChain(0, 0, rightPlatformPoints, p2, false, ColliderType::PLATFORM, STATIC);
    rightPlatform->listener = this;

    const int p3 = 20;

    static int leftTvPoints[p3] = {
        80, 280,
        100, 270,
        135, 265,
        180, 348,
        150, 365,
        120, 330,
        100, 340,
        120, 390,
        90, 400,
        60, 320,
    };

    physLeftTv = App->physics->CreateChain(0, 0, leftTvPoints, p3, false, ColliderType::PLATFORM, STATIC);
    physLeftTv->listener = this;

    const int p4 = 86;

    static int terrainGrounPoints[p4] = {
        292, 380,
        302, 365,
        307, 345,
        305, 325,
        280, 315,
        267, 338,
        265, 338,
        272, 310,
        290, 270,
        330, 240,
        320, 230,
        310, 225,
        280, 220,
        260, 215,
        255, 207,
        262, 182,
        270, 182,
        280, 185,
        300, 192,
        325, 200,
        350, 224,
        360, 250,
        380, 280,
        390, 300,
        395, 350,
        397, 390,
        390, 398,
        360, 407,
        350, 405,
        350, 390,
        352, 380,
        375, 360,
        385, 340,
        380, 320,
        370, 317,
        350, 320,
        330, 335,
        325, 350,
        320, 370,
        300, 400,
        295, 408,
        287, 403,
        289, 390,
    };

    physGroundTerrain = App->physics->CreateChain(0, 0, terrainGrounPoints, p4, false, ColliderType::PLATFORM, STATIC);
    physGroundTerrain->listener = this;

    rectSumLife1 = App->physics->CreateRectangle(145, 217, 10, 35, false, this, ColliderType::PLATFORM, STATIC);
    rectSumLife2 = App->physics->CreateRectangle(185, 205, 10, 35, false, this, ColliderType::PLATFORM, STATIC);
    rectSumLife3 = App->physics->CreateRectangle(225, 200, 10, 35, false, this, ColliderType::PLATFORM, STATIC);
}

void ModuleGame::CreateObjects()
{
    changePokeBall = App->physics->CreateCircle(112, 340, 10, true, this, ColliderType::OBJECT, STATIC);

    shroomish1 = App->physics->CreateCircle(220, 250, 15, false, this, ColliderType::SHROOMISH, STATIC);
    shroomish2 = App->physics->CreateCircle(193, 290, 15, false, this, ColliderType::SHROOMISH, STATIC);
    shroomish3 = App->physics->CreateCircle(240, 285, 15, false, this, ColliderType::SHROOMISH, STATIC);

    whailord = App->physics->CreateCircle(355, 345, 22.5f, true, this, ColliderType::WHAILORD, STATIC);
}

void ModuleGame::CreateVoid()
{
    // Void to respawn (fijo, no es necesario cambiarlo)
    downVoidPos = { SCREEN_WIDTH / 2, SCREEN_HEIGHT + 100 };
    downVoid = App->physics->CreateRectangle(downVoidPos.x, downVoidPos.y, SCREEN_WIDTH, 50, true, this, ColliderType::VOID, STATIC);
}

void ModuleGame::CreateScoreItems()
{
    sumTvScore = App->physics->CreateCircle(109, 396, 10, true, this, ColliderType::ITEM, STATIC);
    sumTvScore->itemScore = 100;

    minusTvScore = App->physics->CreateCircle(169, 360, 10, true, this, ColliderType::ITEM, STATIC);
    minusTvScore->itemScore = -100;

    sumLife1 = App->physics->CreateCircle(164, 200, 10, true, this, ColliderType::SUMLIFE, STATIC);
    sumLife2 = App->physics->CreateCircle(204, 198, 10, true, this, ColliderType::SUMLIFE, STATIC);
    sumLife3 = App->physics->CreateCircle(243, 200, 10, true, this, ColliderType::SUMLIFE, STATIC);
}

void ModuleGame::WhailordAct()
{
    printf("Acting");
    whailordHitted = true;

    if (!inWhailord)
    {
        inWhailord = true;
        mPlayer->canDraw = false;
        whailordAnim.Play("pickingBall");

        coroutineManager.StartCoroutine(1.0f, [this]()
            {
                int x, y;
                whailord->GetPosition(x, y);
                App->physics->SetBodyPosition(mPlayer->playerBody, x, y, false);
                whailordHitted = false;

                whailordAnim.Play("throwingBall");
                mPlayer->canDraw = true;
                readyToShot = true;
                mPlayer->playerBody->body->ApplyLinearImpulseToCenter({-1, 1.5}, true);
            });
    }

    if (inWhailord && readyToShot)
    {
        coroutineManager.StartCoroutine(1.0f, [this]()
            {
                inWhailord = false;
                readyToShot = false;
                whailordHitted = false;
                whailordAnim.Play("idle");
            });
    }
}

void ModuleGame::HandleInput()
{
    // === SPRING / PLUNGER ===
    if (IsKeyPressed(KEY_DOWN)) {
        spoinkAnim.Play("spring_down", false);
        isPressing = true;
    }
    else if (IsKeyDown(KEY_DOWN)) {
        joint->SetMotorSpeed(pullSpeed);

        spoinkAnim.Update(GetFrameTime());
    }
    else if (IsKeyReleased(KEY_DOWN)) {
        joint->SetMotorSpeed(launchSpeed);

        isPressing = false;
        spoinkAnim.StopAnim();
        spoinkAnim.PlayReverse("spring_down", false);
    }

    spoinkAnim.Update(GetFrameTime());

    if (!isPressing && spoinkAnim.IsFinished()) spoinkAnim.Play("idle");

    // === FLIPPERS ===
    if (IsKeyDown(KEY_LEFT)) leftFlipperJoint->SetMotorSpeed(-12.5f);
    else leftFlipperJoint->SetMotorSpeed(12.5f);

    if (IsKeyDown(KEY_RIGHT)) rightFlipperJoint->SetMotorSpeed(12.5f);
    else rightFlipperJoint->SetMotorSpeed(-12.5f);
}

void ModuleGame::Draw()
{
    DrawTexture(mapTexture, 0, 0, WHITE);

    int x, y;
    sumLife1->GetPosition(x, y);  DrawCircle(x, y, 10, SKYBLUE);
    sumLife2->GetPosition(x, y);  DrawCircle(x, y, 10, SKYBLUE);
    sumLife3->GetPosition(x, y);  DrawCircle(x, y, 10, SKYBLUE);

    DrawTextureEx(leftFlipperTexture, { leftFlipperPositionX - 5, leftFlipperPositionY - 15 }, 0, 1.5f, WHITE);
    DrawTextureEx(rightFlipperTexture, { rightFlipperPositionX - 43, rightFlipperPositionY - 15 }, 0, 1.5f, WHITE);

    // === SPOINK/PLUNGER ANIM ===
    spoinkAnim.Draw({ springGroundX - 2, springGroundY-17 }, 1.5f);

    // === SHROOMISH ANIM ===
    shroomishAnim1.Update(GetFrameTime());
    shroomishAnim2.Update(GetFrameTime());
    shroomishAnim3.Update(GetFrameTime());

    // If the hitted animation finished return to idle
    if (shroomishAnim1.IsFinished() && shroomishAnim1.GetCurrentAnimName() == "hitted")
        shroomishAnim1.Play("idle");
    if (shroomishAnim2.IsFinished() && shroomishAnim2.GetCurrentAnimName() == "hitted")
        shroomishAnim2.Play("idle");
    if (shroomishAnim3.IsFinished() && shroomishAnim3.GetCurrentAnimName() == "hitted")
        shroomishAnim3.Play("idle");

    // === WHAILORD ANIM ===
    whailordAnim.Update(GetFrameTime());

    shroomish1->GetPosition(x, y);
    shroomishAnim1.Draw({ (float)x, (float)y - 10 }, 1.5f);

    shroomish2->GetPosition(x, y);
    shroomishAnim2.Draw({ (float)x, (float)y - 10 }, 1.5f);

    shroomish3->GetPosition(x, y);
    shroomishAnim3.Draw({ (float)x, (float)y - 10 }, 1.5f);

    whailord->GetPosition(x, y);
    whailordAnim.Draw({ float(x), (float)y }, 1.5f);

    mPlayer->DrawBall();

    /*DrawCircle(leftFlipperPositionX, leftFlipperPositionY, 5, RED);
    DrawCircle(rightFlipperPositionX, rightFlipperPositionY, 5, RED);*/
}

bool ModuleGame::CleanUp()
{
    printf("Cleaning game scene\n");
    return true;
}

void ModuleGame::OnCollision(PhysBody* physA, PhysBody* physB)
{
    switch (physB->ctype)
    {
    case ColliderType::PLAYER:
        if (physA->ctype == ColliderType::SHROOMISH)
        {
            printf("Collide with a shroomish\n");

            if (physA == shroomish1)
            {
                shroomishAnim1.Play("hitted", false);
            }
            else if (physA == shroomish2)
            {
                shroomishAnim2.Play("hitted", false);
            }
            else if (physA == shroomish3)
            {
                shroomishAnim3.Play("hitted", false);
            }
        }

        if (physA->ctype == ColliderType::WHAILORD && !whailordHitted)
        {
            printf("Collide with whailord");

            WhailordAct();
        }
        break;

    default:
        break;
    }
}

// MOVER LOS FLIPPERS Y EL SPRING AL MODULEPHYSICS ¿coña? (NOPE xd)<--------------------------------------------------------------------------------------------------------------------------
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
    float springH = 40.0f;

    // Y axis points downward
    b2Vec2 worldAxis(0.0f, 1.0f);

    base = App->physics->CreateRectangle(poxX, posY, rectangleW, rectangleH, false, this, ColliderType::PLATFORM, STATIC);
    plunger = App->physics->CreateRectangle(poxX, posY - springH, rectangleW, rectangleH, false, this, ColliderType::PLATFORM, DYNAMIC);

    b2PrismaticJointDef jointDef;
    jointDef.Initialize(base->body, plunger->body, base->body->GetWorldCenter(), worldAxis);

    jointDef.enableLimit = true;
    jointDef.lowerTranslation = 0.0f;     // top position (rest)
    jointDef.upperTranslation = 0.4f;     // how far it can go down

    jointDef.enableMotor = true;
    jointDef.maxMotorForce = 300.0f;
    jointDef.motorSpeed = 0.0f;

    joint = (b2PrismaticJoint*)App->physics->world->CreateJoint(&jointDef);

    plunger->body->SetGravityScale(0.0f);
}