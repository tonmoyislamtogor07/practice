/*
 * main.c
 * 
 * Main entry point for Snake Game
 * Handles window creation and main game loop
 * 
 * Course: Advanced Programming Lab
 * Date: February 2026
 */

#include "snake_game.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

/*
 * Program main entry point
 */
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Classic Game: Snake - Refactored Edition");

    Game_Initialize();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(Game_UpdateAndDraw, 60, 1);
#else
    SetTargetFPS(TARGET_FPS);
    
    while (!WindowShouldClose())
    {
        Game_UpdateAndDraw();
    }
#endif

    Game_Cleanup();
    CloseWindow();

    return 0;
}
