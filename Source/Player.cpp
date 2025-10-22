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
    playerBody = App->physics->CreateCircle(position.x, position.y, radius, ColliderType::PLAYER, DYNAMIC);
    playerBody->listener = this;

    initialPosition = position;

    return true;
}

// Update of the ball
update_status ModulePlayer::Update()
{
    if (!playerBody) return UPDATE_CONTINUE;

    if (needsReset)
    {
        float deltaTime = GetFrameTime(); // Raylib: tiempo entre frames (en segundos)
        resetDelay -= deltaTime;

        // Cuando pasa el tiempo, teletransportamos
        if (resetDelay <= 0.0f)
        {
            App->physics->SetBodyPosition(playerBody, initialPosition.x, initialPosition.y, true);
            playerBody->body->SetLinearVelocity(b2Vec2(0, 0)); // Detener movimiento
            needsReset = false;
            printf("Teleported back to start\n");
        }
    }

    GetPhysics();
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
    if (!playerBody) return;

    App->physics->SetBodyPosition(playerBody, initialPosition.x, initialPosition.y, true);
}

void ModulePlayer::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
    switch (bodyB->ctype)
    {
    case ColliderType::VOID:
        if (bodyA->ctype == ColliderType::PLAYER)
        {
            if (!needsReset) // Evitar reiniciar varias veces seguidas
            {
                printf("Collide with void\n");
                needsReset = true;
                resetDelay = resetDelayDuration; // Comienza el temporizador
            }
        }
        break;

    default:
        break;
    }
}