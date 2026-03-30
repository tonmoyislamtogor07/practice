/*
 * snake_game.h
 * 
 * Main header file for Snake Game
 * Contains type definitions, constants, and function declarations
 * 
 * Course: Advanced Programming Lab
 * Date: February 2026
 */

#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include "raylib.h"
#include <stdbool.h>

// ============================================================================
// GAME CONFIGURATION CONSTANTS
// ============================================================================

#define MAX_SNAKE_LENGTH   400
#define SQUARE_SIZE        31
#define SCREEN_WIDTH       800
#define SCREEN_HEIGHT      450
#define TARGET_FPS         30
#define MOVE_FRAME_DELAY   5
#define FREEZE_DURATION    60  // Frames to freeze before game over

// ============================================================================
// TYPE DEFINITIONS
// ============================================================================

/*
 * Position in 2D space
 */
typedef struct {
    float x;
    float y;
} Position;

/*
 * Represents a single segment of the snake
 */
typedef struct {
    Vector2 position;
    Vector2 size;
    Vector2 speed;
    Color color;
} SnakeSegment;

/*
 * Represents the food/fruit in the game
 */
typedef struct {
    Vector2 position;
    Vector2 size;
    bool active;
    Color color;
} Food;

/*
 * Complete snake entity with all segments
 */
typedef struct {
    SnakeSegment segments[MAX_SNAKE_LENGTH];
    Vector2 segmentPositions[MAX_SNAKE_LENGTH];
    int length;
    bool allowMove;
} Snake;

/*
 * Game state and configuration
 */
typedef struct {
    int framesCounter;
    int playerScore;
    bool isGameOver;
    bool isPaused;
    int freezeCounter;
    Vector2 gridOffset;
} GameState;

// ============================================================================
// CORE GAME FUNCTIONS
// ============================================================================

void Game_Initialize(void);
void Game_Update(void);
void Game_Render(void);
void Game_Cleanup(void);
void Game_UpdateAndDraw(void);

// ============================================================================
// SNAKE MODULE FUNCTIONS
// ============================================================================

void Snake_Initialize(Snake* snake, Vector2 startPosition, Vector2 gridOffset);
void Snake_UpdatePosition(Snake* snake, int framesCounter);
void Snake_ProcessInput(Snake* snake);
void Snake_HandleWrapAround(Snake* snake, Vector2 gridOffset);
bool Snake_CheckSelfCollision(const Snake* snake);
void Snake_Grow(Snake* snake);
void Snake_Render(const Snake* snake);

// ============================================================================
// FOOD MODULE FUNCTIONS
// ============================================================================

void Food_Initialize(Food* food);
void Food_Spawn(Food* food, const Snake* snake, Vector2 gridOffset);
bool Food_CheckCollision(const Food* food, Vector2 position);
void Food_Render(const Food* food);

// ============================================================================
// COLLISION MODULE FUNCTIONS
// ============================================================================

bool Collision_CheckSnakeWithFood(const Snake* snake, const Food* food);
bool Collision_CheckSnakeWithSelf(const Snake* snake);

// ============================================================================
// RENDERING MODULE FUNCTIONS
// ============================================================================

void Renderer_DrawGrid(Vector2 gridOffset);
void Renderer_DrawGameOver(int finalScore);
void Renderer_DrawPauseScreen(void);
void Renderer_DrawFreezeEffect(void);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

int Utils_GetGridColumns(void);
int Utils_GetGridRows(void);
Vector2 Utils_CalculateGridOffset(void);
bool Utils_IsPositionValid(Vector2 position, Vector2 gridOffset);

#endif // SNAKE_GAME_H
