#pragma once

#include "Globals.h"
#include "Module.h"
#include "Player.h"
#include "Animation.h"
#include "Coroutine.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>

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

    void CreateVoid();
    void CreateTable();
    void CreateSlide();
    void CreateObstacles();
    void CreateObjects();
    void CreateScoreItems();

    void ChangeSkin();
    void WhailordAct();
    void Pikachu();

    void HandleInput();

    void Draw();

    void Flippers(PhysBody*& flipper, b2RevoluteJoint*& joint, float posX, float posY, bool isLeft);

    void Spring(PhysBody*& base, PhysBody*& plunger, b2PrismaticJoint*& joint, float poxX, float posY);

public:

    // === SCORE === 
    int currentScore = 0;   // Set the current score during gameplay
    int previousScore = 0;  // Set the previous score during gameplay
    int highestScore = 0;   // Set the highest score during gameplay

    bool isLaunching = false;
    bool changingPokeball = false;

private:

    ModulePlayer* mPlayer = NULL;

    Texture2D mapTexture;

    // === TABLE (MAP) ===
    PhysBody* physTable = NULL;     // Physbody of the table (map)
    PhysBody* physSlide = NULL;

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

    // Other objects
    PhysBody* sumLife1 = NULL;
    PhysBody* sumLife2 = NULL;
    PhysBody* sumLife3 = NULL;

    PhysBody* sumTvScore = NULL;
    PhysBody* minusTvScore = NULL;

    Texture2D pikachu;

    Animator rightPikachuAnim;
    Animator leftPikachuAnim;

    PhysBody* pHitPikachu = NULL;
    PhysBody* rightPikachu = NULL;
    PhysBody* leftPikachu = NULL;

    bool playingPikachuAnimation = false;
    bool collideWithRightPikachu = false;
    bool collidingWithPikachu = NULL;
    float pikachuTime = 0;
    int pCount = 0;

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
