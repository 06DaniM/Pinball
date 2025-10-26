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
    ballTexture = LoadTexture("Assets/Pokeball.png");
    position = { SCREEN_WIDTH - 20, SCREEN_HEIGHT - 200 };

    // Creates the ball
    playerBody = App->physics->CreateCircle(position.x, position.y, radius, false, ColliderType::PLAYER, DYNAMIC);
    playerBody->listener = this;

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

    GetPhysics();
    Reset();

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

    float deltaTime = GetFrameTime();
    resetDelayTimer -= deltaTime;

    // Teleport when the time passed
    if (resetDelayTimer <= 0.0f)
    {
        App->physics->SetBodyPosition(playerBody, initialPosition.x, initialPosition.y, true);
        playerBody->body->SetLinearVelocity(b2Vec2(0, 0));
        needsReset = false;
        resetDelayTimer = resetDelayDuration;
        printf("Teleported back to start\n");
    }
  
}

void ModulePlayer::DrawBall()
{
    // Draw the ball
    DrawTexture(ballTexture, position.x - radius, position.y - radius, WHITE);
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
        if (physA->ctype == ColliderType::PLAYER)
        {
            if (!needsReset) // Avoid reset more than one time (just in case)
            {
                printf("Collide with void\n");
                needsReset = true;
                resetDelayTimer = resetDelayDuration;
                life--;
            }
        }
        break;

    case ColliderType::ITEM:
        if (physA->ctype == ColliderType::PLAYER)
        {
            printf("Collide with an item\n");
            mGame->currentScore += physB->itemScore;

            if (mGame->highestScore <= mGame->currentScore) mGame->highestScore = mGame->currentScore;
        }
        break;

    default:
        break;
    }
}