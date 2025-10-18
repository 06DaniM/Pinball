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
    position = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

    // Crear la bola
    playerBody = App->physics->CreateCircle(position.x, position.y, radius, ColliderType::PLAYER, DYNAMIC);

    // Crear plataforma estática
    App->physics->CreateRectangle(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50, 500, 20, ColliderType::PLATFORM, STATIC);
    initialPosition = position;

    return true;
}

// Update of the ball
update_status ModulePlayer::Update()
{
    if (!playerBody) return UPDATE_CONTINUE;

    // Obtener la posición real de la bola
    int x, y;
    playerBody->GetPosition(x, y);

    // Dibujar la bola
    DrawCircle(x, y, radius, RED);

    return UPDATE_CONTINUE;
}

// Clean of the ball
bool ModulePlayer::CleanUp()
{
    printf("Unloading Player\n");
    playerBody = nullptr;
    return true;
}

// Initial launch
void ModulePlayer::Launch(float initialForce)
{
    if (!playerBody) return;
    printf("Launching\n");
    b2Vec2 force(PIXELS_TO_METERS(0), PIXELS_TO_METERS(initialForce));
    playerBody->body->ApplyLinearImpulseToCenter(force, true);
}

// Reset the ball to a position
void ModulePlayer::Reset()
{
    if (!playerBody) return;

    App->physics->SetBodyPosition(playerBody, initialPosition.x, initialPosition.y, true);
}
