/*
 * collision.cpp
 *
 * Collision detection module (C++ Edition)
 * Centralises all collision detection logic for game entities
 *
 * Course: Advanced Programming Lab
 * Date: February 2026
 *
 * ============================================================================
 * SOLID PRINCIPLES APPLIED IN THIS FILE
 * ============================================================================
 *
 * [SRP] Single Responsibility Principle:
 *   - This entire file is dedicated to one domain: collision detection.
 *   - Collision_CheckSnakeWithFood  -> only tests snake-head vs food cell.
 *   - Collision_CheckSnakeWithSelf  -> only tests snake-head vs body segments.
 *   - Neither function modifies game state, scores, or triggers events.
 *     Reaction to a collision is another module's responsibility (game.cpp).
 *
 * [OCP] Open/Closed Principle:
 *   - Adding a new collision type (e.g. snake vs wall, snake vs obstacle)
 *     means adding a new Collision_Check* function — existing functions are
 *     closed for modification.
 *
 * [LSP] Liskov Substitution Principle:
 *   - All functions accept `const` pointers, so any subtype of Snake or Food
 *     can be passed without breaking the expected contract (pure query, no
 *     mutation, deterministic return value).
 *
 * [ISP] Interface Segregation Principle:
 *   - Game logic that only needs food-collision never has to reference
 *     Collision_CheckSnakeWithSelf, and vice-versa.  Functions are split
 *     along natural query boundaries so callers import only what they need.
 *
 * [DIP] Dependency Inversion Principle:
 *   - Collision_CheckSnakeWithSelf delegates the body-check to
 *     Snake_CheckSelfCollision, an abstraction defined in the Snake module.
 *     collision.cpp depends on the Snake module's interface contract, not on
 *     the raw struct-field traversal logic it encapsulates.
 * ============================================================================
 */

#include "snake_game.h"
#include <cassert>

// ============================================================================
// SNAKE vs FOOD COLLISION
// [SRP] Only tests whether the snake head occupies the same cell as active
//       food.  No state is modified; the caller decides what to do next.
// [ISP] Entirely independent of self-collision — callers subscribe to only
//       what they need.
// [DIP] Depends on the Food struct's `active` field and `position` — a
//       stable, low-level data abstraction.
// ============================================================================

/*
 * Check if snake head collides with food.
 *
 * @param snake - Pointer to snake (const: no mutation)
 * @param food  - Pointer to food  (const: no mutation)
 * @return true if food is active and head occupies the same cell
 */
bool Collision_CheckSnakeWithFood(const Snake* snake, const Food* food)
{
    assert(snake != nullptr);
    assert(food  != nullptr);

    // [SRP] Inactive food can never produce a collision — guard here so
    //       callers are not required to remember to check food->active
    if (!food->active)
    {
        return false;
    }

    // Cell-level equality check (positions snap to SQUARE_SIZE grid)
    return (snake->segments[0].position.x == food->position.x) &&
           (snake->segments[0].position.y == food->position.y);
}

// ============================================================================
// SNAKE vs SELF COLLISION
// [SRP] Determines only whether the snake has bitten itself.
// [OCP] If the self-collision algorithm changes (e.g. bounding-box instead of
//       exact position), only Snake_CheckSelfCollision in snake.cpp changes.
//       This wrapper remains untouched.
// [DIP] Delegates to Snake_CheckSelfCollision — an abstraction from the Snake
//       module — rather than re-implementing the loop here.  High-level
//       collision logic depends on the Snake abstraction, not on raw arrays.
// ============================================================================

/*
 * Check if snake head collides with its own body.
 * Wrapper around Snake_CheckSelfCollision for interface consistency.
 *
 * @param snake - Pointer to snake (const: no mutation)
 * @return true if head position matches any body segment's position
 */
bool Collision_CheckSnakeWithSelf(const Snake* snake)
{
    assert(snake != nullptr);

    // [DIP] Depend on the Snake module's abstraction, not on raw field access
    return Snake_CheckSelfCollision(snake);
}
