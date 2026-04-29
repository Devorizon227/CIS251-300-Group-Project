#include "raylib.h"
#include "raymath.h"

// CHANGELOG: V1 introduces only the player and basic WASD movement. 
// CHANGELOG: V2 introduces the enemy and simple chase 'AI', also changes player color. 
// CHANGELOG: V3 introduces new gamestate "gameOver" and a win/lose variable. 
// CHANGELOG: V4 introduces pause and window border barrier. 
// CHANGELOG: V5 introduces a starting main menu and boolean enemy state for collsion. 
// CHANGELOG: V6 introduces array-based wave system. 
// CHANGELOG: V7 introduces a hit-text timer for the "WHACK!" message. 
// CHANGELOG: V8 introduces a motion trail effect for the weapon. 

// V6 Added a limit for the maximum amount of enemies allowed in the array
#define MAX_ENEMIES 100
// V8 Define the number of frames to store for the weapon trail
#define MAX_TRAIL_SAMPLES 10

// V1 The game is structured as a simple top-down chase for now. The player can move using WASD to avoid the enemy.
struct Enemy {
    Vector2 position;
    float speed;
    bool active;
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
    // V6 Replaced the single 'enemy' struct with an array of enemies and wave counters
    struct Enemy enemies[MAX_ENEMIES];
    int enemiesInWave = 0;
    int currentWave = 0;
    float enemyRadius = 15.0f;
    
    // V7 Timer variable to keep the "WHACK!" text visible
    float whackTimer = 0.0f;

    // V8 Array to store the history of weapon angles for the trail
    float trailAngles[MAX_TRAIL_SAMPLES] = { 0 };
    
    // V5 Main Menu variable
    bool MainMenu = true;
    Rectangle MenuPlay = { 300, 220, 200, 60 };
    Rectangle MenuExit = { 300, 320, 200, 60 };
    // V4 Pause variable 
    bool pause = false;
    // V3 New Game State
    bool gameOver = false;

    while (!WindowShouldClose()) {
        
        // Update mouse position every frame
        Vector2 mousePos = GetMousePosition();
        
        // V5 Collision check to make sure mouse position is on rectangle and while colliding, mouse is clicked      
        if (MainMenu)
        {
            if (CheckCollisionPointRec(mousePos, MenuPlay) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                MainMenu = false;
            }

            if (CheckCollisionPointRec(mousePos, MenuExit) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                CloseWindow();
            }
        }
        //V4 Added pause button mechanic          
        if (IsKeyPressed(KEY_SPACE))
        {
            pause = !pause;
        }
        // V3 changes to player movement logic were made so that inputs cannot be used if gamestate is Over
        else if (!gameOver && !pause) {
        
            // V6 WAVE GENERATION LOGIC: Check if all enemies in the current wave are dead
            bool waveCleared = true;
            for (int i = 0; i < enemiesInWave; i++) {
                if (enemies[i].active) waveCleared = false;
            }

            // V6 If no enemies are left active, start the next wave
            if (waveCleared) {
                currentWave++;
                enemiesInWave = currentWave + 1; // Increase enemy count per wave
                if (enemiesInWave > MAX_ENEMIES) enemiesInWave = MAX_ENEMIES;

                for (int i = 0; i < enemiesInWave; i++) {
                    enemies[i].position = (Vector2){ (float)GetRandomValue(0, screenWidth), (float)GetRandomValue(-100, 0) };
                    enemies[i].speed = 120.0f + (currentWave * 10.0f);
                    enemies[i].active = true;
                }
            }

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
            if (playerPos.x > GetScreenWidth() - playerRadius) playerPos.x = GetScreenWidth() - playerRadius;
            if (playerPos.x < playerRadius) playerPos.x = playerRadius;
            if (playerPos.y > GetScreenHeight() - playerRadius) playerPos.y = GetScreenHeight() - playerRadius;
            if (playerPos.y < playerRadius) playerPos.y = playerRadius;

            // V6 WAVE UPDATE LOOP: Handle all active enemies
            for (int i = 0; i < enemiesInWave; i++) {
                if (!enemies[i].active) continue;

                // V2 ENEMY AI, incredibly rudimentary, just moves directly towards the player at a constant speed.
                Vector2 enemyDir = Vector2Subtract(playerPos, enemies[i].position);
               
                // V2 Check so only move if not already on top of the player
                if (Vector2Length(enemyDir) > 0) {
                    enemyDir = Vector2Normalize(enemyDir);
                }

                enemies[i].position.x += enemyDir.x * enemies[i].speed * GetFrameTime();
                enemies[i].position.y += enemyDir.y * enemies[i].speed * GetFrameTime();
                           
                // V3 Collision Detection for enemy and player
                if (CheckCollisionCircles(playerPos, playerRadius, enemies[i].position, enemyRadius)) {
                    WaitTime(0.1f);
                    gameOver = true;
                }
            }
        } else {
            // V3 If game is over, check for restart
            if (IsKeyPressed(KEY_R)) {
                playerPos = (Vector2){ 400, 300 };
                // V6 Reset wave counters for restart
                currentWave = 0;
                enemiesInWave = 0;
                gameOver = false;
                // V7 Reset whack timer on restart
                whackTimer = 0.0f;
            }
        }

        // V1 Drawing code, draws the player and enemy as circles, and some text to explain the game.
        BeginDrawing();
            ClearBackground(BLACK);     
            
            // V5 Drawing the main menu buttons     
            if (MainMenu)
            {
                DrawText("MAIN MENU", 280, 120, 40, WHITE);
                DrawRectangleRec(MenuPlay, BLUE);
                DrawText("PLAY", 360, 240, 30, WHITE);
                DrawRectangleRec(MenuExit, MAROON);
                DrawText("EXIT", 360, 340, 30, WHITE);
            }
            else if (!gameOver) {
                // V3 Changes to drawing to check for gameOver state.
                // V6 Loop to draw every active enemy in the current wave
                for (int i = 0; i < enemiesInWave; i++) {
                    if (enemies[i].active) DrawCircleV(enemies[i].position, enemyRadius, RED);
                }

                // V1 Draw Player
                DrawCircleV(playerPos, playerRadius, BLUE);
                
                // Draw the Weapon logic
                Vector2 directionToMouse = Vector2Subtract(mousePos, playerPos);
                float angleToMouse = atan2f(directionToMouse.y, directionToMouse.x) * RAD2DEG;
                Rectangle weapon = {playerPos.x, playerPos.y, 80.0f, 8.0f};
                Vector2 weaponOrigin = {0.0f, 4.0f};

                // V8 UPDATE TRAIL: Shift all previous angles and add current angle to front
                if (!pause) {
                    for (int i = MAX_TRAIL_SAMPLES - 1; i > 0; i--) {
                        trailAngles[i] = trailAngles[i - 1];
                    }
                    trailAngles[0] = angleToMouse;
                }

                // V8 DRAW TRAIL: Iterate through history and draw fading rectangles
                for (int i = MAX_TRAIL_SAMPLES - 1; i > 0; i--) {
                    float alpha = 0.4f - ((float)i / MAX_TRAIL_SAMPLES) * 0.4f;
                    DrawRectanglePro(weapon, weaponOrigin, trailAngles[i], Fade(GREEN, alpha));
                }

                // Draw the main weapon on top of the trail
                DrawRectanglePro(weapon, weaponOrigin, angleToMouse, GREEN);
                Vector2 weaponDirection = Vector2Normalize(directionToMouse);
                Vector2 weaponEnd = Vector2Add(playerPos, Vector2Scale(weaponDirection, 80.0f));
                
                // V6 WEAPON COLLISION LOOP
                for (int i = 0; i < enemiesInWave; i++) {
                    if (enemies[i].active && CheckCollisionCircleLine(enemies[i].position, enemyRadius, playerPos, weaponEnd))
                    {
                        enemies[i].active = false;
                        // V7 Set the timer to 0.7 seconds when a hit occurs
                        whackTimer = 0.7f;
                    }
                }

                // V7 Draw the "WHACK!" text if the timer is currently active
                if (whackTimer > 0)
                {
                    DrawText("WHACK!", 10, 100, 20, DARKGREEN);
                    whackTimer -= GetFrameTime();
                }

                DrawText("EN GARDE!", 10, 10, 30, DARKGRAY);
                DrawText("Use WASD to move", 10, 40, 20, LIGHTGRAY);
                DrawText("Press [Space] to Pause", 10, 60, 20, LIGHTGRAY);
                // V6 Draw Wave Counter UI
                DrawText(TextFormat("Wave: %d", currentWave), 10, 80, 20, RAYWHITE);
            } 
            else {
                // V3 Draw Game Over Screen upon loss
                DrawText("GAME OVER!", screenWidth/2 - 110, screenHeight/2 - 40, 40, MAROON);
                DrawText("Press [R] to Restart", screenWidth/2 - 100, screenHeight/2 + 20, 20, DARKGRAY);
                DrawText("Press [ESC] to Leave", screenWidth/2 - 100, screenHeight/2 + 95, 20,DARKGRAY);
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}