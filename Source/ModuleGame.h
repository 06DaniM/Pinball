#pragma once

#include "Globals.h"
#include "Module.h"
#include "Player.h"
#include "Animation.h"
#include "Coroutine.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>
#include <unordered_map>

class ModuleGame : public Module
{
public:
    ModuleGame(Application* app, bool start_enabled = true);
    ~ModuleGame();

    bool Start();
    update_status Update();
    bool CleanUp();
    void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
    void EndCollision(PhysBody* bodyA, PhysBody* bodyB);
    
private:
    void InitializeTextures();
    void InitializeSFX();

    void ChangeValues();

    void CreateVoid();
    void CreateTable();
    void CreateSlide();
    void CreateObstacles();
    void CreateObjects();
    void CreateScoreItems();

    void ChangeSkin();
    void WhailordAct();
    void Pikachu();
    void MoveEgg();

    void HandleInput();
    void GameOver();

    void Draw();

public:

    // === SCORE === 
    int currentScore = 0;   // Set the current score during gameplay
    int previousScore = 0;  // Set the previous score during gameplay
    int highestScore = 0;   // Set the highest score during gameplay

    bool isLaunching = true;
    bool changingPokeball = false;
    bool gameOver = false;

private:

    ModulePlayer* mPlayer = NULL;

    Texture2D mapTexture;
    Music backgroundMusic;

    // === TABLE (MAP) ===
    PhysBody* physTable = NULL;     // Physbody of the table (map)
    PhysBody* physSlideBegin = NULL;
    PhysBody* physSlideInGame = NULL;
    PhysBody* physSlidepPelliperOut = NULL;
    PhysBody* physSlidepPelliperIn = NULL;
    PhysBody* launchingSensor = NULL;
    PhysBody* pelliperSlideSensor1 = NULL;
    PhysBody* pelliperSlideSensor2 = NULL;

    PhysBody* leftTriangle = NULL;
    PhysBody* rightTriangle = NULL;

    PhysBody* leftPlatform = NULL;
    PhysBody* rightPlatform = NULL;

    PhysBody* physLeftTv = NULL;

    PhysBody* physGroundTerrain = NULL;

    PhysBody* rectSumLife1 = NULL;
    PhysBody* rectSumLife2 = NULL;
    PhysBody* rectSumLife3 = NULL;

    //  === OBJECTS === 
    Texture2D changePokeballTexture;

    Animator changePokeballAnim;

    PhysBody* changePokeBall = NULL;
    bool pokeballChanged = false;

    // Shroomish
    Texture2D shroomish;

    Animator shroomishAnim1;
    Animator shroomishAnim2;
    Animator shroomishAnim3;

    PhysBody* shroomish1 = NULL;
    PhysBody* shroomish2 = NULL;
    PhysBody* shroomish3 = NULL;

    // Whailord
    Texture2D whailLordTexture;

    Animator whailordAnim;

    PhysBody* whailord = NULL;
    bool whailordHitted = false;
    bool inWhailord = false;
    bool readyToShot = false;

    // Pikachu
    Texture2D pikachu;

    Animator rightPikachuAnim;
    Animator leftPikachuAnim;

    PhysBody* pHitPikachu = NULL;
    PhysBody* rightPikachu = NULL;
    PhysBody* leftPikachu = NULL;

    bool pikachuLaunch = false;

    // Plusle
    Texture2D plusle;

    Animator plusleAnim;

    // Minum
    Texture2D minum;

    Animator minumAnim;

    // Peliper
    Texture2D peliperTexture;

    Animator peliperAnim;

    // Egg
    Texture2D eggTexture;

    Animator eggAnim;

    PhysBody* egg;

    bool eggHitted = false;

    // Other objects
    Texture2D sumLifeTexture;

    PhysBody* sumLife1 = NULL;
    PhysBody* sumLife2 = NULL;
    PhysBody* sumLife3 = NULL;

    PhysBody* sumTvScore = NULL;
    PhysBody* minusTvScore = NULL;

    //triangles that iluminate and do cool stuff
    PhysBody* E_EVOHitbox = NULL;
    PhysBody* V_EVOHitbox = NULL;
    PhysBody* O_EVOHitbox = NULL;

    Texture2D E_EVOTexture;
    Texture2D V_EVOTexture;
    Texture2D O_EVOTexture;
    bool showE_EVO = false;
    bool showV_EVO = false;
    bool showO_EVO = false;

    PhysBody* POINTHitbox_1 = NULL;
    PhysBody* POINTHitbox_5 = NULL;
    PhysBody* POINTHitbox_10 = NULL;

    Texture2D POINTTexture_1;
    Texture2D POINTTexture_5;
    Texture2D POINTTexture_10;
    bool showPoint1 = false;
    bool showPoint5 = false;
    bool showPoint10 = false;

    PhysBody* G_GETHitbox = NULL;
    PhysBody* E_GETHitbox = NULL;
    PhysBody* T_GETHitbox = NULL;

    Texture2D G_GETTexture;
    Texture2D E_GETTexture;
    Texture2D T_GETTexture;
    bool showG_GET = false;
    bool showE_GET = false;
    bool showT_GET = false;

    PhysBody* MARTHitbox = NULL;
    PhysBody* CATCHitbox = NULL;

    Texture2D MARTTexture;
    Texture2D CATCHTTexture;
    bool showMART = false;
    bool showCATCH = false;

    bool playingPikachuAnimation = false;
    bool collideWithRightPikachu = false;
    bool collidingWithPikachu = false;
    bool canChangeSkin = true;
    float pikachuTime = 0;
    int pCount = 0;
    float scoreMultiplier = 1;

    bool isRespawning = false;
    bool canContinue = false;
    bool inPelliperSlide = false;
    std::unordered_map<b2Fixture*, float> originalRestitutions;
    bool restitutionToggled = false;

    // === FLIPPERS ===
    Texture2D leftFlipperTexture;
    Texture2D rightFlipperTexture;

    PhysBody* leftFlipper = NULL;   // Physbody of the left flipper 
    PhysBody* rightFlipper = NULL;  // Physbody of the right flipper

    b2RevoluteJoint* leftFlipperJoint;  // Revolute joint of the left flipper 
    b2RevoluteJoint* rightFlipperJoint; // Revolute joint of the right flipper

    float leftFlipperPositionX = SCREEN_WIDTH / 2 - 70;  // Set the left flipper x position
    float leftFlipperPositionY = SCREEN_HEIGHT - 55;     // Set the left flipper y position
    
    float rightFlipperPositionX = SCREEN_WIDTH / 2 + 45; // Set the right flipper x position
    float rightFlipperPositionY = SCREEN_HEIGHT - 55;    // Set the right flipper y position

    // === SPRING ===
    Texture2D spoinkTexture;

    Animator spoinkAnim;

    PhysBody* base;     // Physbody of the base of the plunger
    PhysBody* plunger;  // Physbody of the plunger

    b2PrismaticJoint* joint; // Prismatic joint of the spring

    bool isPressing = false;  // Declare if is pressing the spring for the animations
    const float springGroundX = SCREEN_WIDTH - 20;  // x position of the spring
    const float springGroundY = SCREEN_HEIGHT - 40; // y position of the spring
    const float pullSpeed = 2.0f;    // pulling down speed
    const float launchSpeed = -20.0f; // release (upwards) speed

    // === VOID ===
    PhysBody* downVoid; // Physbody of the void
    b2Vec2 downVoidPos; // Set the position of the void

    CoroutineManager coroutineManager;
};
