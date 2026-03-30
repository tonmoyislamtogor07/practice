/*
 * renderer.c
 * 
 * Rendering module
 * Handles all drawing operations for UI and game elements
 * 
 * Course: Advanced Programming Lab
 * Date: February 2026
 */

#include "snake_game.h"

// ============================================================================
// GRID RENDERING
// ============================================================================

/*
 * Draw the game grid
 * Renders vertical and horizontal lines to create grid pattern
 * 
 * @param gridOffset - Offset for grid positioning
 */
void Renderer_DrawGrid(Vector2 gridOffset)
{
    int cols = Utils_GetGridColumns();
    int rows = Utils_GetGridRows();

    // Draw vertical lines
    for (int i = 0; i <= cols; i++)
    {
        DrawLineV(
            (Vector2){ gridOffset.x + i * SQUARE_SIZE, gridOffset.y },
            (Vector2){ gridOffset.x + i * SQUARE_SIZE, gridOffset.y + rows * SQUARE_SIZE },
            LIGHTGRAY
        );
    }

    // Draw horizontal lines
    for (int i = 0; i <= rows; i++)
    {
        DrawLineV(
            (Vector2){ gridOffset.x, gridOffset.y + i * SQUARE_SIZE },
            (Vector2){ gridOffset.x + cols * SQUARE_SIZE, gridOffset.y + i * SQUARE_SIZE },
            LIGHTGRAY
        );
    }
}

// ============================================================================
// UI OVERLAY RENDERING
// ============================================================================

/*
 * Draw pause screen overlay
 * Displays when game is paused
 */
void Renderer_DrawPauseScreen(void)
{
    const char* pauseText = "GAME PAUSED";
    int fontSize = 40;
    int textWidth = MeasureText(pauseText, fontSize);
    
    DrawText(
        pauseText,
        SCREEN_WIDTH / 2 - textWidth / 2,
        SCREEN_HEIGHT / 2 - 40,
        fontSize,
        GRAY
    );
}

/*
 * Draw freeze effect overlay
 * Red tint shown when snake crashes before game over
 */
void Renderer_DrawFreezeEffect(void)
{
    DrawRectangle(
        0, 
        0, 
        SCREEN_WIDTH, 
        SCREEN_HEIGHT, 
        Fade(RED, 0.3f)
    );
}

/*
 * Draw game over screen
 * Shows final score and restart instructions
 * 
 * @param finalScore - Player's final score
 */
void Renderer_DrawGameOver(int finalScore)
{
    // Game Over title
    const char* gameOverText = "GAME OVER!";
    int titleFontSize = 40;
    int titleWidth = MeasureText(gameOverText, titleFontSize);
    
    DrawText(
        gameOverText,
        SCREEN_WIDTH / 2 - titleWidth / 2,
        SCREEN_HEIGHT / 2 - 80,
        titleFontSize,
        RED
    );
    
    // Final score
    const char* scoreText = TextFormat("FINAL SCORE: %d", finalScore);
    int scoreFontSize = 30;
    int scoreWidth = MeasureText(scoreText, scoreFontSize);
    
    DrawText(
        scoreText,
        SCREEN_WIDTH / 2 - scoreWidth / 2,
        SCREEN_HEIGHT / 2 - 30,
        scoreFontSize,
        YELLOW
    );
    
    // Restart instruction
    const char* restartText = "PRESS [ENTER] TO PLAY AGAIN";
    int restartFontSize = 20;
    int restartWidth = MeasureText(restartText, restartFontSize);
    
    DrawText(
        restartText,
        SCREEN_WIDTH / 2 - restartWidth / 2,
        SCREEN_HEIGHT / 2 + 20,
        restartFontSize,
        GRAY
    );
}
