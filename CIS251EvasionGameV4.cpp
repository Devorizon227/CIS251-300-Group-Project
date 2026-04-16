#include "raylib.h"
#include "raymath.h"

// CHANGELOG: V1 introduces only the player and basic WASD movement. - Toby
// CHANGELOG: V2 introduces the enemy and simple chase 'AI', also changes player color. - Toby
// CHANGELOG: V3 introduces new gamestate "gameOver" and a win/lose variable. - Toby

// V1 The game is structured as a simple top-down chase for now. The player can move using WASD to avoid the enemy.
struct Enemy {
    Vector2 position;
    float speed;
};

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Raylib Top-Down - Enemy Chase");
    SetTargetFPS(60);
    
    
    
    // V1 This sets up the player starting position and speed.
    Vector2 playerPos = { 400, 300 };
    float playerSpeed = 250.0f;
    float playerRadius = 20.0f;

    // V1 Enemy Setup
    struct Enemy enemy = { { 100, 100 }, 150.0f }; // Starts at top-left
    float enemyRadius = 15.0f;
    
    //V4 Pause variable 
    bool pause = false;
    
    // V3 New Game State
    bool gameOver = false;

    while (!WindowShouldClose()) {
        
        //V4 Added pause button mechanic         
        if (IsKeyPressed(KEY_SPACE))
        {
            pause = !pause;
        }
        // V3 changes to player movement logic were made so that inputs cannot be used if gamestate is Over
        if (!gameOver && !pause) {
        
            // V1 Main function that determines player movement. Uses WASD keys moving by a vector of 1.
            Vector2 input = { 0, 0 };
            if (IsKeyDown(KEY_W)) input.y -= 1;
            if (IsKeyDown(KEY_S)) input.y += 1;
            if (IsKeyDown(KEY_A)) input.x -= 1;
            if (IsKeyDown(KEY_D)) input.x += 1;
            
            
            // V1 Normalize the input vector to ensure consistent speed in all directions
            if (Vector2Length(input) > 0) {
                input = Vector2Normalize(input);
            }
            
            playerPos.x += input.x * playerSpeed * GetFrameTime();
            playerPos.y += input.y * playerSpeed * GetFrameTime();
            
            //V4 Player "position" can not exceed the dimensions of the window
            
            if (playerPos.x > GetScreenWidth() - playerRadius)
            playerPos.x = GetScreenWidth() - playerRadius;
            if (playerPos.x < playerRadius)
            playerPos.x = playerRadius;
            
            if (playerPos.y > GetScreenHeight() - playerRadius)
            playerPos.y = GetScreenHeight() - playerRadius;
            if (playerPos.y < playerRadius)
            playerPos.y = playerRadius;

            // V2 ENEMY AI, incredibly rudimentary, just moves directly towards the player at a constant speed.
            // V2 Finds vector pointing from enemy to player
            Vector2 enemyDir = Vector2Subtract(playerPos, enemy.position);
           
            // V2 Check so only move if not already on top of the player
            if (Vector2Length(enemyDir) > 0) {
                enemyDir = Vector2Normalize(enemyDir);
            }

            enemy.position.x += enemyDir.x * enemy.speed * GetFrameTime();
            enemy.position.y += enemyDir.y * enemy.speed * GetFrameTime();

            // V3 Collision Detection for enemy and player
            // V3 Check if the distance between circles is less than the sum of their radii
            if (CheckCollisionCircles(playerPos, playerRadius, enemy.position, enemyRadius)) {
                gameOver = true;
            }
        } else {
            // V3 If game is over, check for restart
            if (IsKeyPressed(KEY_R)) {
                playerPos = (Vector2){ 400, 300 };
                enemy.position = (Vector2){ 100, 100 };
                gameOver = false;
            }
        }

        // V1 Drawing code, draws the player and enemy as circles, and some text to explain the game.
        BeginDrawing();
            ClearBackground(RAYWHITE);     
    
            if (!gameOver) {
                // V3 Changes to drawing to check for gameOver state. If game isnt over, draw the enemy and player.
                // V1 Draw Enemy
                DrawCircleV(enemy.position, enemyRadius, RED);
                // V1 Draw Player
                DrawCircleV(playerPos, playerRadius, BLUE);
                DrawText("The Red circle is chasing you!", 10, 10, 20, DARKGRAY);
                DrawText("Use WASD to move", 10, 40, 20, LIGHTGRAY);
                DrawText("Press [Space] to Pause", 10, 60, 20, LIGHTGRAY);
            } 
            else {
                // V3 Draw Game Over Screen upon loss
                DrawText("GAME OVER!", screenWidth/2 - 110, screenHeight/2 - 40, 40, MAROON);
                DrawText("Press [R] to Restart", screenWidth/2 - 100, screenHeight/2 + 20, 20, DARKGRAY);
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}