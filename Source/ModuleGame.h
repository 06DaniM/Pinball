#pragma once

#include "Globals.h"
#include "Module.h"
#include "Player.h"

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
    
private:
    void CreateVoid();
    void CreateTable();
    void CreateObstacles();
    void CreateScoreItems();

    void HandleInput();

    void Draw();

    void Flippers(PhysBody*& flipper, b2RevoluteJoint*& joint, float posX, float posY, bool isLeft);

    void Spring(PhysBody*& base, PhysBody*& plunger, b2PrismaticJoint*& joint, float poxX, float posY);

public:

    // === SCORE === 
    int currentScore = 0;   // Set the current score during gameplay
    int previousScore = 0;  // Set the previous score during gameplay
    int highestScore = 0;   // Set the highest score during gameplay

private:

    ModulePlayer* mPlayer = NULL;

    Texture2D mapTexture;

    // === TABLE (MAP) ===
    PhysBody* physTable = NULL;     // Physbody of the table (map)

    PhysBody* leftTriangle = NULL;
    PhysBody* rightTriangle = NULL;

    PhysBody* leftPlatform = NULL;
    PhysBody* rightPlatform = NULL;

    PhysBody* sumLife1 = NULL;
    PhysBody* sumLife2 = NULL;
    PhysBody* sumLife3 = NULL;

    // === FLIPPERS ===
    PhysBody* leftFlipper = NULL;   // Physbody of the left flipper 
    PhysBody* rightFlipper = NULL;  // Physbody of the right flipper

    b2RevoluteJoint* leftFlipperJoint;  // Revolute joint of the left flipper 
    b2RevoluteJoint* rightFlipperJoint; // Revolute joint of the right flipper

    float leftFlipperPositionX = SCREEN_WIDTH / 2 - 100;  // Set the left flipper x position
    float leftFlipperPositionY = SCREEN_HEIGHT - 100;     // Set the left flipper y position
    
    float rightFlipperPositionX = SCREEN_WIDTH / 2 + 100; // Set the right flipper x position
    float rightFlipperPositionY = SCREEN_HEIGHT - 100;    // Set the right flipper y position

    // === SPRING ===
    PhysBody* base;     // Physbody of the base of the plunger
    PhysBody* plunger;  // Physbody of the plunger

    b2PrismaticJoint* joint; // Prismatic joint of the spring

    float springGroundX = SCREEN_WIDTH - 20;  // Set the x position of the spring
    float springGroundY = SCREEN_HEIGHT - 60; // Set the y position of the spring

    // === VOID ===
    PhysBody* downVoid; // Physbody of the void
    b2Vec2 downVoidPos; // Set the position of the void

    // === COLISIONES TEMPORALES HASTA TENER MAPA ===
    PhysBody* rightWall;
    PhysBody* startGround;
    PhysBody* startleftWall;

    // === VARIABLES TEMPORALES === 
    b2Vec2 leftWallPos, rightWallPos, upWallPos;
    int wallsSizeW = 50, wallsSizeH = SCREEN_HEIGHT - 100;
};
