#include "Globals.h"
#include "Application.h"
#include "Player.h"
#include "ModuleGame.h"
#include "ModulePhysics.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{
}

// Load of the ball
bool ModulePlayer::Start()
{
    printf("Loading Player\n");

    pokeBallTexture = LoadTexture("Assets/Pokeball.png");       // Score x1
    superBallTexture = LoadTexture("Assets/Superball.png");     // Score x1.25
    ultraBallTexture = LoadTexture("Assets/UltraBall.png");     // Score x1.5
    masterBallTexture = LoadTexture("Assets/Masterball.png");   // Score x2

    position = { SCREEN_WIDTH - 20, SCREEN_HEIGHT - 250 };

    // Creates the ball
    playerBody = App->physics->CreateCircle(position.x, position.y, radius, false, this, ColliderType::PLAYER, DYNAMIC);

    life = maxLife;

    mGame = App->game;
    
    initialPosition = position;
    resetDelayTimer = resetDelayDuration;

    return true;
}

// Update of the ball
update_status ModulePlayer::Update()
{
    if (!playerBody) return UPDATE_CONTINUE;

    float deltaTime = GetFrameTime();

    GetPhysics();
    Reset();

    coroutineManager.Update(deltaTime);

    if (IsKeyPressed(KEY_A)) TeleportBallDebug();

    return UPDATE_CONTINUE;
}

void ModulePlayer::GetPhysics()
{
    int x, y;
    playerBody->GetPosition(x, y);

    position.x = (int)x;
    position.y = (int)y;
}

void ModulePlayer::Reset()
{
    if (!needsReset) return;

    if (!teleporting)
    {
        teleporting = true;

        coroutineManager.StartCoroutine(resetDelayDuration, [this]() 
            {
                App->physics->SetBodyPosition(playerBody, initialPosition.x, initialPosition.y, true);
                playerBody->body->SetLinearVelocity(b2Vec2(0, 0));
                needsReset = false;
                teleporting = false;
                printf("Teleported back to start\n");
            });
    }
}

void ModulePlayer::DrawBall()
{
    // Draw the ball
    if (canDraw)
    {
        float scale = 1.5f;
        if (currentPokeball == 0) DrawTextureEx(pokeBallTexture, { position.x - radius, position.y - radius }, 0, scale, WHITE);
        else if (currentPokeball == 1) DrawTextureEx(superBallTexture, { position.x - radius, position.y - radius }, 0, scale, WHITE);
        else if (currentPokeball == 2) DrawTextureEx(ultraBallTexture, { position.x - radius, position.y - radius }, 0, scale, WHITE);
        else DrawTextureEx(masterBallTexture, { position.x - radius, position.y - radius }, 0, scale, WHITE);
    }
}

void ModulePlayer::TeleportBallDebug()
{
    App->physics->SetBodyPosition(playerBody, GetMousePosition().x, GetMousePosition().y, false);
}

bool ModulePlayer::CleanUp()
{
    printf("Unloading Player\n");
    playerBody = nullptr;
    return true;
}

void ModulePlayer::OnCollision(PhysBody* physA, PhysBody* physB)
{
    switch (physB->ctype)
    {
    case ColliderType::VOID:
        if (physA->ctype == ColliderType::PLAYER && !mGame->changingPokeball)
        {
            if (!needsReset) // Avoid reset more than one time (just in case)
            {
                printf("Collide with void\n");
                needsReset = true;
                resetDelayTimer = resetDelayDuration;
                life--;
                if (life <= 0) mGame->gameOver = true;
            }
        }
        break;

    case ColliderType::SUMLIFE:
        if (physA->ctype == ColliderType::PLAYER)
        {
            printf("Collide with a sum life\n");
            if (physB->isActive)
            {
                physB->isActive = false;
                currentSumLife++;
                if (currentSumLife == 3) life++;
            }
        }
        break;

    default:
        break;
    }
}