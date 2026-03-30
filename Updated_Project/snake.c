/*
 * snake.c
 * 
 * Snake entity management
 * Handles snake movement, input processing, collision detection, and rendering
 * 
 * Course: Advanced Programming Lab
 * Date: February 2026
 */

#include "snake_game.h"
#include <assert.h>

// ============================================================================
// SNAKE INITIALIZATION
// ============================================================================

/*
 * Initialize snake with starting position and configuration
 * 
 * @param snake - Pointer to snake structure to initialize
 * @param startPosition - Initial position for snake head
 * @param gridOffset - Grid offset for proper positioning
 */
void Snake_Initialize(Snake* snake, Vector2 startPosition, Vector2 gridOffset)
{
    assert(snake != NULL);
    
    snake->length = 1;
    snake->allowMove = false;

    // Initialize all segments
    for (int i = 0; i < MAX_SNAKE_LENGTH; i++)
    {
        snake->segments[i].position = (Vector2){ gridOffset.x, gridOffset.y };
        snake->segments[i].size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
        snake->segments[i].speed = (Vector2){ SQUARE_SIZE, 0 };
        
        // Head is blue, body segments are sky blue
        snake->segments[i].color = (i == 0) ? BLUE : SKYBLUE;
        
        snake->segmentPositions[i] = (Vector2){ 0.0f, 0.0f };
    }
}

// ============================================================================
// INPUT PROCESSING
// ============================================================================

/*
 * Process keyboard input for snake direction
 * Prevents 180-degree turns (moving directly backwards)
 * 
 * @param snake - Pointer to snake to control
 */
void Snake_ProcessInput(Snake* snake)
{
    assert(snake != NULL);
    
    SnakeSegment* head = &snake->segments[0];

    if (IsKeyPressed(KEY_RIGHT) && (head->speed.x == 0) && snake->allowMove)
    {
        head->speed = (Vector2){ SQUARE_SIZE, 0 };
        snake->allowMove = false;
    }
    
    if (IsKeyPressed(KEY_LEFT) && (head->speed.x == 0) && snake->allowMove)
    {
        head->speed = (Vector2){ -SQUARE_SIZE, 0 };
        snake->allowMove = false;
    }
    
    if (IsKeyPressed(KEY_UP) && (head->speed.y == 0) && snake->allowMove)
    {
        head->speed = (Vector2){ 0, -SQUARE_SIZE };
        snake->allowMove = false;
    }
    
    if (IsKeyPressed(KEY_DOWN) && (head->speed.y == 0) && snake->allowMove)
    {
        head->speed = (Vector2){ 0, SQUARE_SIZE };
        snake->allowMove = false;
    }
}

// ============================================================================
// MOVEMENT AND POSITION UPDATE
// ============================================================================

/*
 * Update snake position based on current speed
 * Moves snake forward and updates all body segments
 * 
 * @param snake - Pointer to snake to update
 * @param framesCounter - Current frame count for timing
 */
void Snake_UpdatePosition(Snake* snake, int framesCounter)
{
    assert(snake != NULL);
    
    // Store previous positions
    for (int i = 0; i < snake->length; i++)
    {
        snake->segmentPositions[i] = snake->segments[i].position;
    }

    // Move snake every MOVE_FRAME_DELAY frames
    if ((framesCounter % MOVE_FRAME_DELAY) == 0)
    {
        for (int i = 0; i < snake->length; i++)
        {
            if (i == 0)
            {
                // Move head
                snake->segments[0].position.x += snake->segments[0].speed.x;
                snake->segments[0].position.y += snake->segments[0].speed.y;
                snake->allowMove = true;
            }
            else
            {
                // Body segments follow the segment in front
                snake->segments[i].position = snake->segmentPositions[i - 1];
            }
        }
    }
}

// ============================================================================
// SCREEN WRAP-AROUND
// ============================================================================

/*
 * Handle screen wrap-around (snake teleports to opposite side)
 * 
 * @param snake - Pointer to snake to check
 * @param gridOffset - Grid offset for boundary calculation
 */
void Snake_HandleWrapAround(Snake* snake, Vector2 gridOffset)
{
    assert(snake != NULL);
    
    int cols = Utils_GetGridColumns();
    int rows = Utils_GetGridRows();
    
    float minX = gridOffset.x;
    float minY = gridOffset.y;
    float maxX = gridOffset.x + (cols - 1) * SQUARE_SIZE;
    float maxY = gridOffset.y + (rows - 1) * SQUARE_SIZE;

    // Horizontal wrap-around
    if (snake->segments[0].position.x > maxX)
    {
        snake->segments[0].position.x = minX;
    }
    else if (snake->segments[0].position.x < minX)
    {
        snake->segments[0].position.x = maxX;
    }

    // Vertical wrap-around
    if (snake->segments[0].position.y > maxY)
    {
        snake->segments[0].position.y = minY;
    }
    else if (snake->segments[0].position.y < minY)
    {
        snake->segments[0].position.y = maxY;
    }
}

// ============================================================================
// COLLISION DETECTION
// ============================================================================

/*
 * Check if snake head collides with its own body
 * 
 * @param snake - Pointer to snake to check
 * @return true if collision detected, false otherwise
 */
bool Snake_CheckSelfCollision(const Snake* snake)
{
    assert(snake != NULL);
    
    for (int i = 1; i < snake->length; i++)
    {
        if ((snake->segments[0].position.x == snake->segments[i].position.x) &&
            (snake->segments[0].position.y == snake->segments[i].position.y))
        {
            return true;
        }
    }
    
    return false;
}

// ============================================================================
// GROWTH
// ============================================================================

/*
 * Increase snake length by one segment
 * Called when snake eats food
 * 
 * @param snake - Pointer to snake to grow
 */
void Snake_Grow(Snake* snake)
{
    assert(snake != NULL);
    assert(snake->length < MAX_SNAKE_LENGTH);
    
    if (snake->length < MAX_SNAKE_LENGTH)
    {
        snake->segments[snake->length].position = snake->segmentPositions[snake->length - 1];
        snake->length++;
    }
}

// ============================================================================
// RENDERING
// ============================================================================

/*
 * Draw snake to screen
 * 
 * @param snake - Pointer to snake to render
 */
void Snake_Render(const Snake* snake)
{
    assert(snake != NULL);
    
    for (int i = 0; i < snake->length; i++)
    {
        DrawRectangleV(
            snake->segments[i].position,
            snake->segments[i].size,
            snake->segments[i].color
        );
    }
}
