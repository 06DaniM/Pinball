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
    void CreateWalls();
    void CreateTable();

    void HandleInput();

    void DrawWall(PhysBody* wall, Color color);
    void DrawTable();

    void Flippers(PhysBody*& flipper, b2RevoluteJoint*& joint, float posX, float posY, bool isLeft);

public:
    ModulePlayer* ball = nullptr; // Reference to the ball

private:
    PhysBody* physTable;

    b2RevoluteJoint* leftFlipperJoint;
    b2RevoluteJoint* rightFlipperJoint;

    PhysBody* leftFlipper;
    PhysBody* rightFlipper;

    float leftFlipperPositionX = SCREEN_WIDTH / 2 - 100;
    float leftFlipperPositionY = SCREEN_HEIGHT - 100;

    float rightFlipperPositionX = SCREEN_WIDTH / 2 + 100;
    float rightFlipperPositionY = SCREEN_HEIGHT - 100;

    PhysBody* rightWall;
    PhysBody* downVoid;

    PhysBody* startGround;
    PhysBody* startleftWall;

    b2Vec2 leftWallPos, rightWallPos, upWallPos, downVoidPos;
    int wallsSizeW = 50, wallsSizeH = SCREEN_HEIGHT-100;
};
