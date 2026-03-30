/*
 * collision.c
 * 
 * Collision detection module
 * Centralizes all collision detection logic for game entities
 * 
 * Course: Advanced Programming Lab
 * Date: February 2026
 */

#include "snake_game.h"
#include <assert.h>

// ============================================================================
// COLLISION DETECTION FUNCTIONS
// ============================================================================

/*
 * Check if snake head collides with food
 * 
 * @param snake - Pointer to snake
 * @param food - Pointer to food
 * @return true if collision detected, false otherwise
 */
bool Collision_CheckSnakeWithFood(const Snake* snake, const Food* food)
{
    assert(snake != NULL);
    assert(food != NULL);
    
    if (!food->active)
    {
        return false;
    }
    
    return (snake->segments[0].position.x == food->position.x) &&
           (snake->segments[0].position.y == food->position.y);
}

/*
 * Check if snake head collides with its own body
 * This is a wrapper around Snake_CheckSelfCollision for consistency
 * 
 * @param snake - Pointer to snake
 * @return true if collision detected, false otherwise
 */
bool Collision_CheckSnakeWithSelf(const Snake* snake)
{
    assert(snake != NULL);
    
    return Snake_CheckSelfCollision(snake);
}
