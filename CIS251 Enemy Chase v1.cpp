#include "raylib.h"
#include "raymath.h"
// The game is structured as a simple top-down chase for now. The player can move using WASD to avoid the enemy.
struct Enemy {
    Vector2 position;
    float speed;
};

int main() {
    InitWindow(800, 600, "Raylib Top-Down - Enemy Chase");
    SetTargetFPS(60);

    // This sets up the player starting position and speed.
    Vector2 playerPos = { 400, 300 };
    float playerSpeed = 250.0f;

    // Enemy Setup
    Enemy enemy = { { 100, 100 }, 150.0f }; // Starts at top-left, slower than player

    while (!WindowShouldClose()) {
        // Main function that determines player movement. Uses WASD keys moving by a vector of 1.
        Vector2 input = { 0, 0 };
        if (IsKeyDown(KEY_W)) input.y -= 1;
        if (IsKeyDown(KEY_S)) input.y += 1;
        if (IsKeyDown(KEY_A)) input.x -= 1;
        if (IsKeyDown(KEY_D)) input.x += 1;
        // Normalize the input vector to ensure consistent speed in all directions
        if (Vector2Length(input) > 0) {
            input = Vector2Normalize(input);
        }
        playerPos.x += input.x * playerSpeed * GetFrameTime();
        playerPos.y += input.y * playerSpeed * GetFrameTime();

        // ENEMY AI, incredibly rudimentary, just moves directly towards the player at a constant speed.
        // Finds vector pointing from enemy to player
        Vector2 enemyDir = Vector2Subtract(playerPos, enemy.position);
        
        // Check so only move if not already on top of the player
        if (Vector2Length(enemyDir) > 0) {
            enemyDir = Vector2Normalize(enemyDir);
        }

        enemy.position.x += enemyDir.x * enemy.speed * GetFrameTime();
        enemy.position.y += enemyDir.y * enemy.speed * GetFrameTime();

        // Drawing code, draws the player and enemy as circles, and some text to explain the game.
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // Draw Enemy
            DrawCircleV(enemy.position, 15, RED);
            
            // Draw Player
            DrawCircleV(playerPos, 20, MAROON);

            DrawText("The Red circle is chasing you!", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}