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
    App->physics->Flippers(leftFlipper, leftFlipperJoint, leftFlipperPositionX, leftFlipperPositionY, true);      // Left flipper
    App->physics->Flippers(rightFlipper, rightFlipperJoint, rightFlipperPositionX, rightFlipperPositionY, false);   // Right flipper

    // === LOAD THE SPRING ===
    App->physics->Spring(base, plunger, joint, springGroundX, springGroundY);

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

    if (minumAnim.GetCurrentAnimName() == "flip" && minumAnim.IsFinished())
        minumAnim.Play("idle");

    if (plusleAnim.GetCurrentAnimName() == "flip" && plusleAnim.IsFinished())
        plusleAnim.Play("idle");

    if (isLaunching)
    {
        physTable->SetSensor(true);
        physSlidepPelliperIn->SetSensor(true);
        physSlidepPelliperOut->SetSensor(true);
        physSlideInGame->SetSensor(true);
        physSlideBegin->SetSensor(false);
    }

    else if (!isLaunching && !inPelliperSlide)
    {
        physTable->SetSensor(false);
        physSlideInGame->SetSensor(false);
        physGroundTerrain->SetSensor(false);
        physSlidepPelliperIn->SetSensor(true);
        physSlidepPelliperOut->SetSensor(true);
        physSlideBegin->SetSensor(true);
    }

    else if (inPelliperSlide)
    {
        physSlidepPelliperIn->SetSensor(false);
        physSlidepPelliperOut->SetSensor(false);
        physTable->SetSensor(true);
        physSlideInGame->SetSensor(true);
        physSlideBegin->SetSensor(true);
        physGroundTerrain->SetSensor(true);
    }

    HandleInput();
    Pikachu();
    GameOver();
    Draw();

    return UPDATE_CONTINUE;
}

void ModuleGame::InitializeTextures()
{
    mapTexture = LoadTexture("Assets/Pinball table pokemon2.png");

    sumLifeTexture = LoadTexture("Assets/Extra_life.png");

    leftFlipperTexture = LoadTexture("Assets/LeftFlipperSprite.png");
    rightFlipperTexture = LoadTexture("Assets/RightFlipperSprite.png");

    spoinkTexture = LoadTexture("Assets/Spoink_Spritesheet.png");
    shroomish = LoadTexture("Assets/Shroomish_Spritesheet.png");
    whailLordTexture = LoadTexture("Assets/Whailord_Spritesheet.png");
    changePokeballTexture = LoadTexture("Assets/Evo_Mart_Spritesheet.png");
    pikachu = LoadTexture("Assets/Pikachu_Spritesheet.png");
    plusle = LoadTexture("Assets/Plusle_Spritesheet.png");
    minum = LoadTexture("Assets/Minum_Spritesheet.png");
    peliperTexture = LoadTexture("Assets/Pelliper_Spritesheet.png");
    eggTexture = LoadTexture("Assets/Huevo_Pokemon_Spritesheet.png");

    E_EVOTexture = LoadTexture("Assets/E_EVO.png");
    V_EVOTexture = LoadTexture("Assets/V_EVO.png");
    O_EVOTexture = LoadTexture("Assets/O_EVO.png");
    
    POINTTexture_1 = LoadTexture("Assets/1POINT.png");

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

    changePokeballAnim = Animator(&changePokeballTexture, 49, 36);
    changePokeballAnim.AddAnim("idle", 0, 3, 3.0f, true);
    changePokeballAnim.AddAnim("evo", 4, 6, 3.0f, false);
    changePokeballAnim.Play("idle");

    leftPikachuAnim = Animator(&pikachu, 38, 92);
    leftPikachuAnim.AddAnim("idle", 0, 2, 3.0f, true);
    leftPikachuAnim.AddAnim("attack", 2, 12, 6.0f, true);

    rightPikachuAnim = Animator(&pikachu, 38, 92);
    rightPikachuAnim.AddAnim("idle", 0, 3, 3.0f, true);
    rightPikachuAnim.AddAnim("attack", 2, 12, 6.0f, true);

    plusleAnim = Animator(&plusle, 30, 36);
    plusleAnim.AddAnim("idle", 0, 4, 2.5f, true);
    plusleAnim.AddAnim("flip", 3, 10, 4.0f, false);
    plusleAnim.Play("idle");

    minumAnim = Animator(&minum, 30, 36);
    minumAnim.AddAnim("idle", 0, 4, 2.5f, true);
    minumAnim.AddAnim("flip", 3, 10, 4.0f, false);
    minumAnim.Play("idle");

    peliperAnim = Animator(&peliperTexture, 43, 37);
    peliperAnim.AddAnim("idle", 0, 2, 3.0f, true);
    peliperAnim.AddAnim("pickBall", 2, 14, 3.0f, false);
    peliperAnim.Play("idle");

    eggAnim = Animator(&eggTexture, 18, 18);
    eggAnim.AddAnim("idle", 0, 4, 2.0f, true);
    eggAnim.AddAnim("hitted", 2, 6, 3.0f, false);
    eggAnim.Play("idle");
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
    const int p1 = 138;

    static int slideBegin[p1] = {
    437, 145,
    407, 110,
    390, 100,
    370, 94,
    280, 94,
    170, 113,
    140, 120,
    120, 130,
    106, 140,
    93, 150,
    71, 170,
    56, 200,
    52, 230,
    54, 280,
    60, 320,
    52, 200,
    55, 197,
    68, 167,
    90, 148,
    103, 138,
    117, 128,
    137, 118,
    167, 110,
    277, 93,
    367, 93,
    387, 97,
    410, 110,
    440, 145,
    440, 780,
    470, 780,
    470, 145,
    453, 105,
    449, 95,
    427, 81,
    367, 58,
    267, 58,
    130, 88,
    75, 120,
    33, 163,
    20, 203,
    17, 243,
    17, 263,
    18, 303,
    25, 323,
    32, 363,
    52, 403,
    68, 433,
    80, 460,
    85, 457,
    65, 430,
    47, 400,
    27, 360,
    18, 320,
    18, 300,
    16, 260,
    16, 240,
    27, 200,
    40, 160,
    77, 117,
    127, 85,
    260, 55,
    380, 55,
    430, 78,
    442, 87,
    452, 95,
    456, 100,
    475, 145,
    475, 790,
    438, 790,
    };

    physSlideBegin = App->physics->CreateChain(0, 0, slideBegin, p1, false, ColliderType::PLATFORM, STATIC);
    physSlideBegin->listener = this;

    const int p2 = 12;

    static int slideInGame[p2] = {
        78, 456,
        50, 400,
        60, 320,
        65, 330,
        55, 400,
        84, 456,
    };

    physSlideInGame = App->physics->CreateChain(0, 0, slideInGame, p2, false, ColliderType::PLATFORM, STATIC);
    physSlideInGame->listener = this;

    const int p3 = 80;

    static int slidePelipperOut[p3] = {
        309, 230,
        309, 205,
        307, 175,
        300, 147,
        260, 105,
        220, 86,
        180, 82,
        130, 88,
        75, 120,
        33, 163,
        20, 203,
        17, 243,
        18, 263,
        20, 303,
        25, 323,
        32, 363,
        52, 403,
        68, 433,
        80, 460,
        85, 457,
        83, 450,
        80, 440,
        65, 420,
        50, 390,
        31, 350,
        26, 320,
        21, 280,
        19, 240,
        23, 200,
        32, 170,
        55, 140,
        80, 120,
        132, 90,
        182, 86,
        224, 90,
        262, 107,
        297, 149,
        305, 178,
        307, 205,
        307, 230,
    };

    physSlidepPelliperOut = App->physics->CreateChain(0, 0, slidePelipperOut, p3, false, ColliderType::PLATFORM, STATIC);
    physSlidepPelliperOut->listener = this;

    const int p4 = 56;

    static int slidePelipperIn[p4] = {
        273, 228,
        273, 180,
        264, 148,
        247, 128,
        225, 115,
        197, 113,
        157, 113,
        113, 132,
        90, 152,
        63, 183,
        48, 223,
        48, 263,
        55, 298,
        58, 315,

        60, 315,
        57, 300,
        50, 265,
        50, 225,
        65, 185,
        92, 155,
        115, 135,
        155, 115,
        200, 115,
        227, 120,
        245, 130,
        262, 150,
        270, 180,
        270, 228,
    };

    physSlidepPelliperIn = App->physics->CreateChain(0, 0, slidePelipperIn, p4, false, ColliderType::PLATFORM, STATIC);
    physSlidepPelliperIn->listener = this;

    launchingSensor = App->physics->CreateCircle(97, 440, 10, true, this, ColliderType::LAUNCHING, STATIC);

    pelliperSlideSensor1 = App->physics->CreateCircle(70, 400, 10, true, this, ColliderType::PSlIDEBEGIN, STATIC);
    pelliperSlideSensor2 = App->physics->CreateCircle(290, 220, 10, true, this, ColliderType::PSLIDEEND, STATIC);
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
        320, 240,
        310, 230,
        290, 225,
        275, 220,
        260, 215,
        255, 207,
        262, 196,
        270, 196,
        280, 198,
        300, 199,
        325, 210,
        350, 230,
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
    shroomish1->itemScore = 200;
    shroomish2 = App->physics->CreateCircle(193, 290, 15, false, this, ColliderType::SHROOMISH, STATIC);
    shroomish2->itemScore = 200;
    shroomish3 = App->physics->CreateCircle(240, 285, 15, false, this, ColliderType::SHROOMISH, STATIC);
    shroomish2->itemScore = 200;

    whailord = App->physics->CreateCircle(355, 345, 22.5f, true, this, ColliderType::WHAILORD, STATIC);

    rightPikachu = App->physics->CreateRectangle(388, 690, 40, 100, true, this, ColliderType::PIKACHU, STATIC);
    leftPikachu = App->physics->CreateRectangle(58, 690, 40, 100, true, this, ColliderType::PIKACHU, STATIC);

    egg = App->physics->CreateCircle(SCREEN_WIDTH / 2-13, SCREEN_HEIGHT / 2 + 150, 20, false, this, ColliderType::BOUNCE, KINEMATIC);
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

    pHitPikachu = App->physics->CreateCircle(288, 325, 10, true, this, ColliderType::PHITPIKA, STATIC);

    POINTHitbox_1 = App->physics->CreateCircle(95, 445, 15, true, this, ColliderType::POINTSTRIANGLE_1, STATIC);

}

void ModuleGame::ChangeSkin()
{
    mPlayer->canDraw = false;
    if (!changingPokeball)
    {
        changingPokeball = true;
        changePokeballAnim.Play("evo");

        coroutineManager.StartCoroutine(2.0f, [this]()
            {
                int x, y;
                changePokeBall->GetPosition(x, y);
                App->physics->SetBodyPosition(mPlayer->playerBody, x, y, false);
                pokeballChanged = true;
                mPlayer->canDraw = true;
                changePokeballAnim.Play("idle");
                if (canChangeSkin)
                {
                    mPlayer->currentPokeball++;
                    if (mPlayer->currentPokeball == 0) scoreMultiplier = 1.0f;
                    else if (mPlayer->currentPokeball == 1) scoreMultiplier = 1.25f;
                    else if (mPlayer->currentPokeball == 2) scoreMultiplier = 1.5f;
                    else if (mPlayer->currentPokeball == 3) scoreMultiplier = 2.0f;
                    canChangeSkin = false;
                }

                if (pokeballChanged)
                {
                    coroutineManager.StartCoroutine(1.0f, [this]()
                        {
                            changingPokeball = false;
                            pokeballChanged = false;
                        });
                }
            });
    }
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

void ModuleGame::Pikachu()
{
    if (!collidingWithPikachu && !playingPikachuAnimation) return;

    if (!collidingWithPikachu)
    {
        if (collideWithRightPikachu)
        {
            rightPikachuAnim.StopAnim();
            rightPikachuAnim.Play("idle", true);
            playingPikachuAnimation = false;
            return;
        }

        else
        {
            leftPikachuAnim.StopAnim();
            leftPikachuAnim.Play("idle", true);
            playingPikachuAnimation = false;
            return;
        }
    }

    pikachuTime += GetFrameTime();

    if (!playingPikachuAnimation)
    {
        printf("Playing attack anim\n");
        if (collideWithRightPikachu) rightPikachuAnim.Play("attack", true);
        else leftPikachuAnim.Play("attack", true);

        playingPikachuAnimation = true;
    }

    if (pikachuTime >= 1.6f)
    {
        printf("Add force\n");
        if (collideWithRightPikachu) mPlayer->playerBody->body->ApplyLinearImpulseToCenter({ 0.5f, -1.5f }, true);
        else mPlayer->playerBody->body->ApplyLinearImpulseToCenter({ -0.5f, -1.5f }, true);

        pikachuTime = 0;
    }

    else printf("Timing: %f\n", pikachuTime);
}

void ModuleGame::HandleInput()
{
    if (gameOver) return;

    // === SPRING / PLUNGER ===
    if (IsKeyPressed(KEY_DOWN)) {
        spoinkAnim.Play("spring_down", false);
        isPressing = true;
    }
    else if (IsKeyDown(KEY_DOWN)) 
    {
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
    if (IsKeyDown(KEY_LEFT)) leftFlipperJoint->SetMotorSpeed(-20.5f);
    else leftFlipperJoint->SetMotorSpeed(12.5f);

    if (IsKeyDown(KEY_RIGHT)) rightFlipperJoint->SetMotorSpeed(20.5f);
    else rightFlipperJoint->SetMotorSpeed(-12.5f);
}

void ModuleGame::GameOver()
{
    if (!gameOver) return;

    if (!isRespawning)
    {
        isRespawning = true;

        coroutineManager.StartCoroutine(1.0f, [this]()
            {
                canContinue = true;
            });
    }
    
    if (canContinue)
    {
        if (GetKeyPressed() != 0)
        {
            gameOver = false;
            mPlayer->life = 3;
            mPlayer->needsReset = true;
            isRespawning = false;
            canContinue = false;
        }
    }
}

void ModuleGame::Draw()
{
    int x, y;

    DrawTexture(mapTexture, 0, 0, WHITE);


    sumLife1->GetPosition(x, y);  
    if(!sumLife1->isActive) DrawTextureEx(sumLifeTexture, { (float)x - 7, (float)y-8 }, 0, 2,WHITE);

    sumLife2->GetPosition(x, y);
    if (!sumLife2->isActive) DrawTextureEx(sumLifeTexture, { (float)x - 9, (float)y - 7 }, 0, 2, WHITE);

    sumLife3->GetPosition(x, y);
    if (!sumLife3->isActive) DrawTextureEx(sumLifeTexture, { (float)x - 7, (float)y - 8 }, 0, 2, WHITE);

    DrawTextureEx(leftFlipperTexture, { leftFlipperPositionX - 5, leftFlipperPositionY - 15 }, 0, 1.5f, WHITE);
    DrawTextureEx(rightFlipperTexture, { rightFlipperPositionX - 43, rightFlipperPositionY - 15 }, 0, 1.5f, WHITE);

    // === CHANGE POKEBALL SCREEN ===
    changePokeballAnim.Update(GetFrameTime());

    changePokeBall->GetPosition(x, y);
    changePokeballAnim.Draw({ float(x) - 26, float(y) - 96 }, 2);

    // === SPOINK/PLUNGER ===
    spoinkAnim.Draw({ springGroundX - 2, springGroundY-17 }, 1.5f);

    // === SHROOMISH ===
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

    shroomish1->GetPosition(x, y);
    shroomishAnim1.Draw({ (float)x, (float)y - 10 }, 1.5f);

    shroomish2->GetPosition(x, y);
    shroomishAnim2.Draw({ (float)x, (float)y - 10 }, 1.5f);

    shroomish3->GetPosition(x, y);
    shroomishAnim3.Draw({ (float)x, (float)y - 10 }, 1.5f);

    // === WHAILORD ===
    whailordAnim.Update(GetFrameTime());

    whailord->GetPosition(x, y);
    whailordAnim.Draw({ float(x), (float)y }, 1.5f);

    // === EGG ===
    eggAnim.Update(GetFrameTime());

    egg->GetPosition(x, y);
    eggAnim.Draw({ (float)x+2, (float)y-2 }, 2.5f);
    
    // === PIKACHU ===
    rightPikachuAnim.Update(GetFrameTime());
    leftPikachuAnim.Update(GetFrameTime());

    rightPikachu->GetPosition(x, y);
    if (pCount >= 1) rightPikachuAnim.Draw({ float(x), (float)y }, 1.5f);

    leftPikachu->GetPosition(x, y);
    if (pCount >= 2)leftPikachuAnim.Draw({ float(x), (float)y }, 1.5f);

    // === PLUSLE ===
    plusleAnim.Update(GetFrameTime());

    plusleAnim.Draw({ 106, 362 }, 1.5f);

    // === MINUM ===
    minumAnim.Update(GetFrameTime());

    minumAnim.Draw({ 166, 326 }, 1.5f);

    // === PELIPER ===
    peliperAnim.Update(GetFrameTime());

    peliperAnim.Draw({ 318, 260 }, 1.75f);

    if(showPoint1)
    {
        DrawTextureEx(POINTTexture_1, { 86, 429 }, 0, 2.0f, WHITE);
        
    }

    if (!gameOver) 
    {
        mPlayer->DrawBall();

        DrawText(TextFormat("Current Score %d", currentScore), 350, 20, 12, WHITE);
        DrawText(TextFormat("Highest Score %d", highestScore), 350, 35, 12, WHITE);
        DrawText(TextFormat("Previous Score %d", previousScore), 350, 50, 12, WHITE);
    }

    else
    {
        mPlayer->canDraw = false;
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, { 0, 0, 0, 220 });
        DrawText("GAMEOVER", SCREEN_WIDTH / 2 - 124, SCREEN_HEIGHT / 2, 40, RED);

        DrawText(TextFormat("Current Score %d", currentScore), SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 70, 20, WHITE);
        DrawText(TextFormat("Highest Score %d", highestScore), SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 100, 20, WHITE);
        DrawText(TextFormat("Previous Score %d", previousScore), SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 130, 20, WHITE);
    }
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

            if (physA == shroomish1) shroomishAnim1.Play("hitted", false);
            else if (physA == shroomish2) shroomishAnim2.Play("hitted", false);
            else if (physA == shroomish3) shroomishAnim3.Play("hitted", false);

            currentScore += physA->itemScore * scoreMultiplier;
        }

        else if (physA->ctype == ColliderType::OBJECT)
        {
            printf("Collide with an object\n");

            if (!changingPokeball)
            {
                printf("Changing pokeball\n");
                ChangeSkin();
            }
        }

        else if (physA->ctype == ColliderType::ITEM)
        {
            printf("Collide with an item\n");
            currentScore += physA->itemScore * scoreMultiplier;
            if (physA->itemScore > 0) plusleAnim.Play("flip", true);
            if (physA->itemScore < 0) minumAnim.Play("flip", true);

            if (highestScore <= currentScore) highestScore = currentScore;
            printf("%d\n", currentScore);
        }

        else if (physA->ctype == ColliderType::PIKACHU)
        {
            if (physA == rightPikachu && pCount >= 1)
            {
                printf("Collide with pikachu\n");
                collidingWithPikachu = true;
                collideWithRightPikachu = true;
            }
            else if (physA == leftPikachu && pCount >= 2)
            {
                printf("Collide with pikachu\n");
                collidingWithPikachu = true;
                collideWithRightPikachu = false;
            }
        }

        else if (physA->ctype == ColliderType::PHITPIKA)
        {
            printf("Collide with the P object\n");
            pCount++;
            if (pCount == 1) rightPikachuAnim.Play("idle");
            else if (pCount == 2) leftPikachuAnim.Play("idle");
        }

        else if (physA->ctype == ColliderType::VOID)
        {
            printf("Collide with void MG\n");
            pCount = 0;
            sumLife1->isActive = true;
            sumLife2->isActive = true;
            sumLife3->isActive = true;

            isLaunching = true;
            showPoint1 = false;
        }

        else if (physA->ctype == ColliderType::WHAILORD && !whailordHitted)
        {
            printf("Collide with whailord\n");
            WhailordAct();
        }

        else if (physA->ctype == ColliderType::LAUNCHING)
        {
            printf("Collide with launching sensor\n");
            isLaunching = false;

            if (!isLaunching && inPelliperSlide) inPelliperSlide = false;
        }

        else if (physA->ctype == ColliderType::PSlIDEBEGIN)
        {
            printf("Collide with pelliper slide 1 sensor\n");
            if (!isLaunching) inPelliperSlide = true;
        }

        else if (physA->ctype == ColliderType::PSLIDEEND)
        {
            printf("Collide with pelliper slide 2 sensor\n");
            inPelliperSlide = false;
        }
        else if (physA->ctype == ColliderType::POINTSTRIANGLE_1 && showPoint1 == false) {
            printf("Collide with 1 Point Triangle\n");
            showPoint1 = true;
            currentScore += 1;
        }
        break;

    default:
        break;
    }
}

void ModuleGame::EndCollision(PhysBody* physA, PhysBody* physB)
{
    switch (physB->ctype)
    {
    case ColliderType::PLAYER:
        if (physA->ctype == ColliderType::PIKACHU)
        {
            if (physA == rightPikachu && pCount >= 1)
            {
                printf("End collide with pikachu\n");
                collidingWithPikachu = false;
            }
            else if (physA == leftPikachu && pCount >= 2)
            {
                printf("End collide with pikachu\n");
                collidingWithPikachu = false;
            }
            pikachuTime = 0;
        }
        
        else if (physA->ctype == ColliderType::OBJECT)
        {
            canChangeSkin = true;
        }

        break;

    default:
        break;
    }
}