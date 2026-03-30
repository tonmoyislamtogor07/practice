/*
 * game.c
 * 
 * Core game logic and state management
 * Handles game initialization, update loop, and coordination between modules
 * 
 * Course: Advanced Programming Lab
 * Date: February 2026
 */

#include "snake_game.h"
#include <assert.h>

// ============================================================================
// GLOBAL GAME STATE
// ============================================================================

static GameState gameState = { 0 };
static Snake playerSnake = { 0 };
static Food gameFruit = { 0 };

// ============================================================================
// GAME INITIALIZATION
// ============================================================================

/*
 * Initialize all game systems and reset game state
 * Called at game start and when restarting after game over
 */
void Game_Initialize(void)
{
    // Reset game state
    gameState.framesCounter = 0;
    gameState.playerScore = 0;
    gameState.isGameOver = false;
    gameState.isPaused = false;
    gameState.freezeCounter = 0;
    
    // Calculate grid offset for centering
    gameState.gridOffset = Utils_CalculateGridOffset();
    
    // Initialize game entities
    Snake_Initialize(&playerSnake, gameState.gridOffset, gameState.gridOffset);
    Food_Initialize(&gameFruit);
}

// ============================================================================
// GAME UPDATE LOGIC
// ============================================================================

/*
 * Main game update function
 * Called once per frame to update game state
 */
void Game_Update(void)
{
    if (!gameState.isGameOver)
    {
        // Handle pause toggle
        if (IsKeyPressed('P'))
        {
            gameState.isPaused = !gameState.isPaused;
        }

        if (!gameState.isPaused)
        {
            // Handle freeze countdown (delay before game over)
            if (gameState.freezeCounter > 0)
            {
                gameState.freezeCounter--;
                if (gameState.freezeCounter == 0)
                {
                    gameState.isGameOver = true;
                }
                return;  // Skip game logic during freeze
            }

            // Process player input
            Snake_ProcessInput(&playerSnake);

            // Update snake position
            Snake_UpdatePosition(&playerSnake, gameState.framesCounter);

            // Handle screen wrap-around
            Snake_HandleWrapAround(&playerSnake, gameState.gridOffset);

            // Check for self-collision
            if (Snake_CheckSelfCollision(&playerSnake))
            {
                gameState.freezeCounter = FREEZE_DURATION;
            }

            // Spawn food if not active
            if (!gameFruit.active)
            {
                Food_Spawn(&gameFruit, &playerSnake, gameState.gridOffset);
            }

            // Check collision with food
            if (Collision_CheckSnakeWithFood(&playerSnake, &gameFruit))
            {
                Snake_Grow(&playerSnake);
                gameFruit.active = false;
                gameState.playerScore++;
            }

            gameState.framesCounter++;
        }
    }
    else
    {
        // Game over - wait for restart
        if (IsKeyPressed(KEY_ENTER))
        {
            Game_Initialize();
        }
    }
}

// ============================================================================
// GAME RENDERING
// ============================================================================

/*
 * Main rendering function
 * Draws all game elements to screen
 */
void Game_Render(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    if (!gameState.isGameOver)
    {
        // Draw game grid
        Renderer_DrawGrid(gameState.gridOffset);

        // Draw game entities
        Snake_Render(&playerSnake);
        Food_Render(&gameFruit);

        // Draw UI overlays
        if (gameState.isPaused)
        {
            Renderer_DrawPauseScreen();
        }

        if (gameState.freezeCounter > 0)
        {
            Renderer_DrawFreezeEffect();
        }
    }
    else
    {
        // Draw game over screen
        Renderer_DrawGameOver(gameState.playerScore);
    }

    EndDrawing();
}

// ============================================================================
// GAME CLEANUP
// ============================================================================

/*
 * Cleanup game resources
 * Called before program exit
 */
void Game_Cleanup(void)
{
    // No dynamic memory to free in current implementation
    // Reserved for future resource cleanup
}

// ============================================================================
// COMBINED UPDATE AND DRAW
// ============================================================================

/*
 * Combined update and draw function
 * Used for main game loop and web platform
 */
void Game_UpdateAndDraw(void)
{
    Game_Update();
    Game_Render();
}
