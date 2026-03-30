/*
 * food.c
 * 
 * Food entity management
 * Handles food spawning, collision detection, and rendering
 * 
 * Course: Advanced Programming Lab
 * Date: February 2026
 */

#include "snake_game.h"
#include <assert.h>

// ============================================================================
// FOOD INITIALIZATION
// ============================================================================

/*
 * Initialize food with default properties
 * Food starts inactive and will be spawned during gameplay
 * 
 * @param food - Pointer to food structure to initialize
 */
void Food_Initialize(Food* food)
{
    assert(food != NULL);
    
    food->size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
    food->color = YELLOW;
    food->active = false;
    food->position = (Vector2){ 0.0f, 0.0f };
}

// ============================================================================
// FOOD SPAWNING
// ============================================================================

/*
 * Spawn food at a random grid position
 * Ensures food doesn't spawn on snake's body
 * 
 * @param food - Pointer to food to spawn
 * @param snake - Pointer to snake (to avoid spawning on snake)
 * @param gridOffset - Grid offset for position calculation
 */
void Food_Spawn(Food* food, const Snake* snake, Vector2 gridOffset)
{
    assert(food != NULL);
    assert(snake != NULL);
    
    food->active = true;
    
    int cols = Utils_GetGridColumns();
    int rows = Utils_GetGridRows();
    
    // Check if grid is completely filled with snake
    if (snake->length >= cols * rows)
    {
        food->active = false;
        return;
    }
    
    int randomX, randomY;
    bool positionValid;
    
    /*
     * Find a valid random position that doesn't overlap with snake
     * Uses a loop to retry until valid position is found
     */
    do
    {
        positionValid = true;
        
        // Generate random grid coordinates
        randomX = GetRandomValue(0, cols - 1);
        randomY = GetRandomValue(0, rows - 1);
        
        // Convert to screen coordinates
        food->position = (Vector2){
            gridOffset.x + randomX * SQUARE_SIZE,
            gridOffset.y + randomY * SQUARE_SIZE
        };
        
        // Check if position overlaps with any snake segment
        for (int i = 0; i < snake->length; i++)
        {
            if ((food->position.x == snake->segments[i].position.x) &&
                (food->position.y == snake->segments[i].position.y))
            {
                positionValid = false;
                break;
            }
        }
    }
    while (!positionValid);
}

// ============================================================================
// COLLISION DETECTION
// ============================================================================

/*
 * Check if food collides with a given position
 * 
 * @param food - Pointer to food
 * @param position - Position to check collision against
 * @return true if collision detected, false otherwise
 */
bool Food_CheckCollision(const Food* food, Vector2 position)
{
    assert(food != NULL);
    
    if (!food->active)
    {
        return false;
    }
    
    return (food->position.x == position.x) && 
           (food->position.y == position.y);
}

// ============================================================================
// RENDERING
// ============================================================================

/*
 * Draw food to screen
 * Only renders if food is active
 * 
 * @param food - Pointer to food to render
 */
void Food_Render(const Food* food)
{
    assert(food != NULL);
    
    if (food->active)
    {
        DrawRectangleV(food->position, food->size, food->color);
    }
}
