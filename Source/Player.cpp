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

    lifesTexture = LoadTexture("Assets/Pokeball.png");
    pokeBallTexture = LoadTexture("Assets/PokeballAnim.png");       // Score x1
    superBallTexture = LoadTexture("Assets/SuperballAnim.png");     // Score x1.25
    ultraBallTexture = LoadTexture("Assets/UltraBallAnim.png");     // Score x1.5
    masterBallTexture = LoadTexture("Assets/MasterballAnim.png");   // Score x2

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
                canDraw = true;
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
    if (!canDraw) return;

    float scale = 1.5f;
    int frameCount = 16;
    int frameWidth = 15;
    int frameHeight = 15;

    // Obtén la rotación real desde Box2D
    float rotation = playerBody->body->GetAngle() * RAD2DEG;

    int currentFrame = frameCount - 1 - ((int)(fabs(rotation) / 360.0f * frameCount) % frameCount);

    Rectangle source = { (float)(currentFrame * frameWidth), 0.0f, (float)frameWidth, (float)frameHeight };

    Rectangle dest = { position.x, position.y, frameWidth * scale, frameHeight * scale };
    Vector2 origin = { (frameWidth * scale) / 2, (frameHeight * scale) / 2 };

    Texture2D* tex = nullptr;
    switch (currentPokeball)
    {
    case 0: tex = &pokeBallTexture; break;
    case 1: tex = &superBallTexture; break;
    case 2: tex = &ultraBallTexture; break;
    default: tex = &masterBallTexture; break;
    }

    // Dibuja el frame correspondiente (sin rotación real)
    DrawTexturePro(*tex, source, dest, origin, 0.0f, WHITE);
}

void ModulePlayer::TeleportBallDebug()
{
    App->physics->SetBodyPosition(playerBody, GetMousePosition().x, GetMousePosition().y, false);
}

bool ModulePlayer::CleanUp()
{
    printf("Unloading Player\n");

    playerBody = nullptr;

    UnloadTexture(lifesTexture);
    UnloadTexture(pokeBallTexture);
    UnloadTexture(superBallTexture);
    UnloadTexture(ultraBallTexture);
    UnloadTexture(masterBallTexture);

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