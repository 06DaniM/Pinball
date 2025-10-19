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
    void HandleInput();
    void DrawTable();

public:
    ModulePlayer* ball = nullptr; // Reference to the ball

private:
    PhysBody* leftWall;
    PhysBody* rightWall;
    PhysBody* downVoid;

    b2Vec2 leftWallPos, rightWallPos, upWallPos, downVoidPos;
    int wallsSizeW = 50, wallsSizeH = SCREEN_HEIGHT-100;
};
