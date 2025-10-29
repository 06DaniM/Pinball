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

    pokeBallTexture = LoadTexture("Assets/Pokeball.png");
    superBallTexture = LoadTexture("Assets/Superball.png");
    ultraBallTexture = LoadTexture("Assets/UltraBall.png");
    masterBallTexture = LoadTexture("Assets/Masterball.png");

    position = { SCREEN_WIDTH - 20, SCREEN_HEIGHT - 250 };

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
    SetTargetFPS(240);
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
    if (canDraw)
    {
        if (currentPokeball == 0) DrawTexture(pokeBallTexture, position.x - radius, position.y - radius, WHITE);
        else if (currentPokeball == 1) DrawTexture(superBallTexture, position.x - radius, position.y - radius, WHITE);
        else if (currentPokeball == 2) DrawTexture(ultraBallTexture, position.x - radius, position.y - radius, WHITE);
        else DrawTexture(masterBallTexture, position.x - radius, position.y - radius, WHITE);
    }
}

void ModulePlayer::TeleportBallDebug()
{
    App->physics->SetBodyPosition(playerBody, GetMousePosition().x, GetMousePosition().y, false);
}

void ModulePlayer::ChangeSkin()
{
    currentPokeball++;
    changingPokeball = true;
    playerBody->btype = KINEMATIC;
    playerBody->body->SetLinearVelocity(b2Vec2_zero);

    // Añadir un timer para luego lanzar la pelota
    // Timer(float time);
    /*playerBody->body->SetLinearVelocity(b2Vec2_zero);
    changingPokeball = false;
    playerBody->btype = DYNAMIC;
    canDraw = true;*/
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

    case ColliderType::OBJECT:
        if (physA->ctype == ColliderType::PLAYER)
        {
            printf("Collide with an object\n");
            
            if (!changingPokeball)
            {
                printf("Changing pokeball\n");
                ChangeSkin();
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