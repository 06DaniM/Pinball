#include "Globals.h"
#include "Application.h"
#include "Player.h"
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
    position = { SCREEN_WIDTH - 140, SCREEN_HEIGHT / 2 };

    // Creates the ball
    playerBody = App->physics->CreateCircle(position.x, position.y, radius, false, ColliderType::PLAYER, DYNAMIC);
    playerBody->listener = this;

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
    DrawBall();

    return UPDATE_CONTINUE;
}

// Clean of the ball
bool ModulePlayer::CleanUp()
{
    printf("Unloading Player\n");
    playerBody = nullptr;
    return true;
}

void ModulePlayer::GetPhysics()
{
    int x, y;
    playerBody->GetPosition(x, y);

    position.x = (int)x;
    position.y = (int)y;
}

void ModulePlayer::DrawBall()
{
    // Draw the ball
    DrawCircle(position.x, position.y, radius, RED);
}

// Initial launch
void ModulePlayer::Launch()
{
    if (!playerBody) return;
    playerBody->body->ApplyLinearImpulseToCenter({0,-1.5f}, true);
}

// Reset the ball to a position
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

void ModulePlayer::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
    switch (bodyB->ctype)
    {
    case ColliderType::VOID:
        if (bodyA->ctype == ColliderType::PLAYER)
        {
            if (!needsReset) // Avoid reset more than one time
            {
                printf("Collide with void\n");
                needsReset = true;
                resetDelayTimer = resetDelayDuration;
            }
        }
        break;

    default:
        break;
    }
}