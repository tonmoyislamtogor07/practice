/*
 * utils.c
 * 
 * Utility functions module
 * Provides helper functions for calculations and common operations
 * 
 * Course: Advanced Programming Lab
 * Date: February 2026
 */

#include "snake_game.h"

// ============================================================================
// GRID CALCULATIONS
// ============================================================================

/*
 * Calculate number of columns in the game grid
 * 
 * @return Number of columns that fit in screen width
 */
int Utils_GetGridColumns(void)
{
    return SCREEN_WIDTH / SQUARE_SIZE;
}

/*
 * Calculate number of rows in the game grid
 * 
 * @return Number of rows that fit in screen height
 */
int Utils_GetGridRows(void)
{
    return SCREEN_HEIGHT / SQUARE_SIZE;
}

/*
 * Calculate grid offset to center the grid on screen
 * Handles cases where grid doesn't perfectly fit screen dimensions
 * 
 * @return Vector2 containing x and y offsets
 */
Vector2 Utils_CalculateGridOffset(void)
{
    int cols = Utils_GetGridColumns();
    int rows = Utils_GetGridRows();
    
    Vector2 offset;
    offset.x = (float)((SCREEN_WIDTH - cols * SQUARE_SIZE) / 2);
    offset.y = (float)((SCREEN_HEIGHT - rows * SQUARE_SIZE) / 2);
    
    return offset;
}

// ============================================================================
// POSITION VALIDATION
// ============================================================================

/*
 * Check if a position is within valid grid bounds
 * 
 * @param position - Position to validate
 * @param gridOffset - Grid offset for boundary calculation
 * @return true if position is valid, false otherwise
 */
bool Utils_IsPositionValid(Vector2 position, Vector2 gridOffset)
{
    int cols = Utils_GetGridColumns();
    int rows = Utils_GetGridRows();
    
    float minX = gridOffset.x;
    float minY = gridOffset.y;
    float maxX = gridOffset.x + (cols - 1) * SQUARE_SIZE;
    float maxY = gridOffset.y + (rows - 1) * SQUARE_SIZE;
    
    return (position.x >= minX && position.x <= maxX &&
            position.y >= minY && position.y <= maxY);
}
