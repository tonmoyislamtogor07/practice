# SOLID Principles Applied — Snake Game (C++ Edition)

**Project:** Classic Snake Game  
**Language:** C++ (converted from C)  
**Course:** Advanced Programming Lab — February 2026

> This document is a precise, file-by-file and function-by-function record of
> **where** each SOLID principle was applied, **why** that specific spot needed
> it, and **which** principle was chosen — with the actual code as evidence.

---

## Table of Contents

1. [Quick Reference Map](#1-quick-reference-map)
2. [snake_game.h](#2-snake_gameh)
3. [main.cpp](#3-maincpp)
4. [snake.cpp](#4-snakecpp)
5. [food.cpp](#5-foodcpp)
6. [collision.cpp](#6-collisioncpp)
7. [renderer.cpp](#7-renderercpp)
8. [utils.cpp](#8-utilscpp)
9. [game.cpp](#9-gamecpp)
10. [Cross-File Principle Summary](#10-cross-file-principle-summary)

---

## 1. Quick Reference Map

> Every cell shows the principle(s) active in that file/function.
> ✓ = primary application, (✓) = supporting role.

| File | SRP | OCP | LSP | ISP | DIP |
|------|-----|-----|-----|-----|-----|
| `snake_game.h` | ✓ | ✓ | ✓ | ✓ | ✓ |
| `main.cpp` | ✓ | | | | ✓ |
| `snake.cpp` | ✓ | ✓ | ✓ | ✓ | ✓ |
| `food.cpp` | ✓ | ✓ | ✓ | ✓ | ✓ |
| `collision.cpp` | ✓ | ✓ | ✓ | ✓ | ✓ |
| `renderer.cpp` | ✓ | ✓ | ✓ | ✓ | ✓ |
| `utils.cpp` | ✓ | ✓ | ✓ | ✓ | ✓ |
| `game.cpp` | ✓ | ✓ | ✓ | ✓ | ✓ |

---

## 2. `snake_game.h`

This file is the contract layer for the entire project. Every SOLID principle
appears here because this is where abstractions, interfaces, and type
definitions are declared — the foundation everything else depends on.

---

### 2.1 Structs — `Position`, `SnakeSegment`, `Food`, `Snake`, `GameState`

**Which principle:** SRP  
**Where:** The five struct definitions at the top of the header.

**Why:**  
In the original C code, related data was scattered across loose global
variables. Grouping them into structs is SRP applied at the *data* level —
each struct owns exactly one concept and has one reason to change.

```cpp
// [SRP] Position is solely responsible for representing a 2D coordinate.
//       It changes only if the coordinate system changes.
struct Position {
    float x;
    float y;
};

// [SRP] SnakeSegment holds data for exactly one body piece.
//       It changes only if a segment gains or loses a property.
struct SnakeSegment {
    Vector2 position;
    Vector2 size;
    Vector2 speed;
    Color   color;
};

// [SRP] GameState holds only game-level counters and flags.
//       It does NOT know about Snake or Food internals.
struct GameState {
    int     framesCounter;
    int     playerScore;
    bool    isGameOver;
    bool    isPaused;
    int     freezeCounter;
    Vector2 gridOffset;
};
```

**Impact:** When the snake gains a new property (e.g. `isInvincible`), only
`SnakeSegment` changes. `GameState`, `Food`, and `Position` are untouched.

---

### 2.2 `constexpr` Constants

**Which principle:** DIP  
**Where:** All configuration constants at the top of the header.

**Why:**  
Every module that needs grid dimensions or timing values depends on these
named constants — not on raw numbers. This is DIP: high-level modules depend
on a stable abstraction (a named constant) rather than on a concrete literal
scattered everywhere.

```cpp
// [DIP] Named constants are the abstraction layer for configuration.
//       All modules depend on these names, not on the literal values.
//       Changing SQUARE_SIZE here propagates to every module automatically.
constexpr int MAX_SNAKE_LENGTH  = 400;
constexpr int SQUARE_SIZE       = 31;
constexpr int SCREEN_WIDTH      = 800;
constexpr int SCREEN_HEIGHT     = 450;
constexpr int MOVE_FRAME_DELAY  = 5;
constexpr int FREEZE_DURATION   = 60;
```

**Why `constexpr` over `#define`:**  
`constexpr` is type-safe and scoped — it is a C++ abstraction improvement
over the C preprocessor macro. DIP favours stable, typed abstractions.

---

### 2.3 `IRenderable` Interface

**Which principle:** OCP, ISP, LSP, DIP  
**Where:** The `IRenderable` pure-virtual struct.

**Why (OCP):** New renderable entities (bonus food, obstacles, power-ups) can
be added by implementing this interface. Existing rendering code in
`game.cpp` and `renderer.cpp` is never modified.

**Why (ISP):** `IRenderable` carries *only* `render()`. Entities that only
need to draw themselves are not forced to implement update logic.

**Why (LSP):** The `const` on `render()` is a behavioural contract: any
subtype that implements this interface promises not to mutate state during
rendering. That guarantee makes subtypes safely substitutable.

**Why (DIP):** `game.cpp`'s render loop can depend on `IRenderable*` instead
of on concrete `Snake*` or `Food*` — the high-level loop depends on the
abstraction, not the concretion.

```cpp
// [OCP] Open for extension: add new renderable types without touching
//        existing render code.
// [ISP] Segregated from IUpdatable: render-only entities carry no update burden.
// [LSP] `const` contract: any subtype must render without mutating state.
// [DIP] High-level game loop depends on this abstraction, not on Snake/Food.
struct IRenderable {
    virtual void render() const = 0;
    virtual ~IRenderable() = default;
};
```

---

### 2.4 `IUpdatable` Interface

**Which principle:** OCP, ISP, LSP, DIP  
**Where:** The `IUpdatable` pure-virtual struct.

**Why (OCP):** New updatable entities plug in by implementing `update()`.
`Game_Update()` does not need to be opened.

**Why (ISP):** Kept entirely separate from `IRenderable`. A headless
simulation entity implements only `IUpdatable` — it has no rendering
dependency whatsoever.

**Why (LSP):** Any subtype implementing `update(int)` can substitute for
`IUpdatable` in a game loop without breaking the loop's expectations.

**Why (DIP):** `Game_Update()` can iterate over `IUpdatable*` objects — it
depends on the abstraction, not on the concrete `Snake` or `Food` type.

```cpp
// [OCP] New updatable entities extend the game without modifying Game_Update.
// [ISP] Separate from IRenderable: update-only entities carry no render burden.
// [LSP] Any compliant subtype substitutes without breaking the update loop.
// [DIP] Game loop depends on IUpdatable abstraction, not on concrete types.
struct IUpdatable {
    virtual void update(int framesCounter) = 0;
    virtual ~IUpdatable() = default;
};
```

---

## 3. `main.cpp`

`main.cpp` is the thinnest file in the project. It needed the fewest changes
from Stage 1 because the refactored C had already applied the right
principles here instinctively.

---

### 3.1 `main()` function body

**Which principle:** SRP  
**Where:** The entire `main()` body.

**Why:**  
`main()` has exactly one responsibility: manage the window lifetime and drive
the top-level loop. It does not initialise snake positions, spawn food, or
compute grid offsets. Those are other modules' jobs.

```cpp
// [SRP] main() is responsible for window lifecycle and the top-level loop only.
//       All game logic is delegated outward. main() has one reason to change:
//       the platform entry-point contract changes (e.g. WinMain on Windows).
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Classic Game: Snake - C++ SOLID Edition");
    Game_Initialize();

    SetTargetFPS(TARGET_FPS);
    while (!WindowShouldClose())
    {
        Game_UpdateAndDraw();
    }

    Game_Cleanup();
    CloseWindow();
    return 0;
}
```

---

### 3.2 Calls to `Game_*` functions only

**Which principle:** DIP  
**Where:** Every function call inside `main()`.

**Why:**  
`main()` never calls `Snake_Initialize`, `Food_Spawn`, or any low-level
module directly. It calls only the `Game_*` abstraction layer. This is DIP:
the highest-level module (`main`) depends on the next-level abstraction
(`Game_*`), not on concrete subsystems.

```cpp
// [DIP] main() depends on Game_* abstractions — not on Snake, Food, or Renderer.
//       If the entire game engine is swapped, main() is untouched.
Game_Initialize();
Game_UpdateAndDraw();
Game_Cleanup();
```

---

## 4. `snake.cpp`

`snake.cpp` is the richest file for SOLID application because the snake
entity has the most behavioural facets: input, movement, boundary wrapping,
collision, growth, and rendering.

---

### 4.1 `Snake_Initialize()`

**Which principle:** SRP  
**Where:** The entire function.

**Why:**  
This function does one thing: reset a `Snake` struct to a valid initial
state. It does not move the snake, process input, or touch game state. Its
single reason to change is if the snake's initial configuration changes.

```cpp
// [SRP] Solely responsible for setting the snake to a clean starting state.
//       Does not move, draw, or interact with food or game state.
void Snake_Initialize(Snake* snake, Vector2 startPosition, Vector2 gridOffset)
{
    assert(snake != nullptr);
    snake->length    = 1;
    snake->allowMove = false;
    for (int i = 0; i < MAX_SNAKE_LENGTH; i++)
    {
        snake->segments[i].position = { gridOffset.x, gridOffset.y };
        snake->segments[i].size     = { static_cast<float>(SQUARE_SIZE),
                                         static_cast<float>(SQUARE_SIZE) };
        snake->segments[i].speed    = { static_cast<float>(SQUARE_SIZE), 0.0f };
        snake->segments[i].color    = (i == 0) ? BLUE : SKYBLUE;
        snake->segmentPositions[i]  = { 0.0f, 0.0f };
    }
}
```

---

### 4.2 `Snake_ProcessInput()`

**Which principle:** SRP, ISP  
**Where:** The entire function.

**Why (SRP):** This function's only job is to translate keyboard state into a
new head speed vector. It does not move the snake, check collisions, or score
points. One reason to change: the control scheme changes (e.g. WASD, gamepad).

**Why (ISP):** It is a completely separate function from `Snake_UpdatePosition`
and `Snake_Render`. A replay system that feeds pre-recorded inputs never
calls this function — it is not forced to depend on it.

```cpp
// [SRP] One job: map keyboard state to a new speed vector on the head.
// [ISP] Callers that do not need input (replay, AI) never call this function.
void Snake_ProcessInput(Snake* snake)
{
    assert(snake != nullptr);
    SnakeSegment* head = &snake->segments[0];

    if (IsKeyPressed(KEY_RIGHT) && (head->speed.x == 0.0f) && snake->allowMove) {
        head->speed = { static_cast<float>(SQUARE_SIZE), 0.0f };
        snake->allowMove = false;
    }
    if (IsKeyPressed(KEY_LEFT) && (head->speed.x == 0.0f) && snake->allowMove) {
        head->speed = { static_cast<float>(-SQUARE_SIZE), 0.0f };
        snake->allowMove = false;
    }
    if (IsKeyPressed(KEY_UP) && (head->speed.y == 0.0f) && snake->allowMove) {
        head->speed = { 0.0f, static_cast<float>(-SQUARE_SIZE) };
        snake->allowMove = false;
    }
    if (IsKeyPressed(KEY_DOWN) && (head->speed.y == 0.0f) && snake->allowMove) {
        head->speed = { 0.0f, static_cast<float>(SQUARE_SIZE) };
        snake->allowMove = false;
    }
}
```

---

### 4.3 `Snake_UpdatePosition()`

**Which principle:** SRP, DIP  
**Where:** The entire function, specifically the frame-gate condition.

**Why (SRP):** Solely responsible for advancing segment positions by one
tick. Wrap-around is a different concern handled separately.

**Why (DIP):** The frame-gate uses `MOVE_FRAME_DELAY` — a named constant
abstraction — instead of the raw literal `5`. If timing policy changes,
only the constant is updated; this function body is untouched.

```cpp
// [SRP] Solely responsible for advancing position. Wrap-around is elsewhere.
// [DIP] Depends on MOVE_FRAME_DELAY abstraction, not on the raw literal 5.
void Snake_UpdatePosition(Snake* snake, int framesCounter)
{
    assert(snake != nullptr);
    for (int i = 0; i < snake->length; i++)
        snake->segmentPositions[i] = snake->segments[i].position;

    if ((framesCounter % MOVE_FRAME_DELAY) == 0)  // [DIP] named constant
    {
        for (int i = 0; i < snake->length; i++)
        {
            if (i == 0) {
                snake->segments[0].position.x += snake->segments[0].speed.x;
                snake->segments[0].position.y += snake->segments[0].speed.y;
                snake->allowMove = true;
            } else {
                snake->segments[i].position = snake->segmentPositions[i - 1];
            }
        }
    }
}
```

---

### 4.4 `Snake_HandleWrapAround()`

**Which principle:** SRP, DIP  
**Where:** The entire function, specifically the boundary calculations.

**Why (SRP):** One job — teleport the head to the opposite edge when it
exits the grid. Movement arithmetic lives in `Snake_UpdatePosition`.

**Why (DIP):** Grid dimensions come from `Utils_GetGridColumns()` and
`Utils_GetGridRows()` — abstractions — not from repeating
`SCREEN_WIDTH / SQUARE_SIZE` inline. If the grid changes, `utils.cpp` is
the only file to update.

```cpp
// [SRP] One job: boundary wrap. Does not move or draw the snake.
// [DIP] Grid size from Utils abstractions — not raw screen-size arithmetic.
void Snake_HandleWrapAround(Snake* snake, Vector2 gridOffset)
{
    assert(snake != nullptr);
    const int   cols = Utils_GetGridColumns();   // [DIP]
    const int   rows = Utils_GetGridRows();      // [DIP]
    const float minX = gridOffset.x;
    const float maxX = gridOffset.x + static_cast<float>((cols - 1) * SQUARE_SIZE);
    const float minY = gridOffset.y;
    const float maxY = gridOffset.y + static_cast<float>((rows - 1) * SQUARE_SIZE);

    if      (snake->segments[0].position.x > maxX) snake->segments[0].position.x = minX;
    else if (snake->segments[0].position.x < minX) snake->segments[0].position.x = maxX;
    if      (snake->segments[0].position.y > maxY) snake->segments[0].position.y = minY;
    else if (snake->segments[0].position.y < minY) snake->segments[0].position.y = maxY;
}
```

---

### 4.5 `Snake_CheckSelfCollision()`

**Which principle:** SRP, ISP, LSP  
**Where:** The function signature and body.

**Why (SRP):** Answers one question only — does the head share a cell with
any body segment? It does not trigger game over or modify state.

**Why (ISP):** Isolated from input and movement — callers that only need the
collision answer (like `collision.cpp`) depend solely on this function.

**Why (LSP):** Accepts `const Snake*`. The promise is: this is a pure query
that never mutates the snake. Any subtype of snake satisfying that contract
is safely substitutable.

```cpp
// [SRP] Pure query: does head overlap any body segment? No side effects.
// [ISP] Callers needing only collision check depend only on this function.
// [LSP] const Snake* — guaranteed non-mutating; any Snake subtype is safe.
bool Snake_CheckSelfCollision(const Snake* snake)
{
    assert(snake != nullptr);
    for (int i = 1; i < snake->length; i++)
    {
        if ((snake->segments[0].position.x == snake->segments[i].position.x) &&
            (snake->segments[0].position.y == snake->segments[i].position.y))
            return true;
    }
    return false;
}
```

---

### 4.6 `Snake_Grow()`

**Which principle:** SRP, OCP  
**Where:** The entire function.

**Why (SRP):** One job — append one segment to the tail. Score incrementing
and food deactivation are `game.cpp`'s responsibility.

**Why (OCP):** Growing by 1 is the default behaviour. A "grow by N" variant
is a new function (`Snake_GrowBy(Snake*, int n)`) — `Snake_Grow` is not
modified.

```cpp
// [SRP] One job: extend the snake by one segment. Score/food untouched.
// [OCP] "Grow by N" variant = new function, not an edit to this one.
void Snake_Grow(Snake* snake)
{
    assert(snake != nullptr);
    assert(snake->length < MAX_SNAKE_LENGTH);
    if (snake->length < MAX_SNAKE_LENGTH)
    {
        snake->segments[snake->length].position =
            snake->segmentPositions[snake->length - 1];
        snake->length++;
    }
}
```

---

### 4.7 `Snake_Render()`

**Which principle:** SRP, ISP, LSP  
**Where:** The function signature (`const Snake*`) and its isolation from
update logic.

**Why (SRP):** One job — draw every active segment. Does not compute
positions or handle input.

**Why (ISP):** Separated from all update functions. A replay viewer or
spectator mode calls only `Snake_Render` — it never has to link against
`Snake_ProcessInput` or `Snake_UpdatePosition`.

**Why (LSP):** `const Snake*` — the render contract: draw without mutating.
Any Snake subtype that satisfies this contract is substitutable. A
`GhostSnake` with translucent colour would pass here correctly.

```cpp
// [SRP] One job: draw every segment. Does not compute, move, or score.
// [ISP] Callers that only render never depend on input or movement functions.
// [LSP] const Snake* — non-mutating contract; any compliant subtype is safe.
void Snake_Render(const Snake* snake)
{
    assert(snake != nullptr);
    for (int i = 0; i < snake->length; i++)
    {
        DrawRectangleV(
            snake->segments[i].position,
            snake->segments[i].size,
            snake->segments[i].color
        );
    }
}
```

---

## 5. `food.cpp`

---

### 5.1 `Food_Initialize()`

**Which principle:** SRP  
**Where:** The entire function.

**Why:** One job — reset a `Food` struct to a safe inactive state. Does not
spawn, draw, or touch snake or game state. Reason to change: the default
food appearance or initial state changes.

```cpp
// [SRP] One job: reset Food to a safe blank state. No spawn, no draw.
void Food_Initialize(Food* food)
{
    assert(food != nullptr);
    food->size     = { static_cast<float>(SQUARE_SIZE),
                       static_cast<float>(SQUARE_SIZE) };
    food->color    = YELLOW;
    food->active   = false;
    food->position = { 0.0f, 0.0f };
}
```

---

### 5.2 `Food_Spawn()`

**Which principle:** SRP, OCP, DIP  
**Where:** The function body — particularly the rejection-sampling loop and
the grid dimension calls.

**Why (SRP):** Solely responsible for placing food at a valid, unoccupied
cell. Score tracking and food deactivation after eating are `game.cpp`'s job.

**Why (OCP):** The rejection-sampling strategy is fully encapsulated here.
Adding a weighted or deterministic spawn strategy means adding
`Food_SpawnWeighted()` alongside this function — this function is closed for
modification.

**Why (DIP):** Grid dimensions come from `Utils_GetGridColumns()` and
`Utils_GetGridRows()` — abstractions — not from inline division of
`SCREEN_WIDTH`.

```cpp
// [SRP] One job: place food at a valid, snake-free grid cell.
// [OCP] Spawn strategy is encapsulated. New strategy = new function, not an edit.
// [DIP] Grid size from Utils abstractions, not raw screen-size arithmetic.
void Food_Spawn(Food* food, const Snake* snake, Vector2 gridOffset)
{
    assert(food != nullptr);
    assert(snake != nullptr);
    food->active = true;

    const int cols = Utils_GetGridColumns();  // [DIP]
    const int rows = Utils_GetGridRows();     // [DIP]

    if (snake->length >= cols * rows) { food->active = false; return; }

    int  randomX, randomY;
    bool positionValid;
    do {                                      // [OCP] strategy is self-contained
        positionValid = true;
        randomX = GetRandomValue(0, cols - 1);
        randomY = GetRandomValue(0, rows - 1);
        food->position = {
            gridOffset.x + static_cast<float>(randomX * SQUARE_SIZE),
            gridOffset.y + static_cast<float>(randomY * SQUARE_SIZE)
        };
        for (int i = 0; i < snake->length; i++)
        {
            if ((food->position.x == snake->segments[i].position.x) &&
                (food->position.y == snake->segments[i].position.y))
            { positionValid = false; break; }
        }
    } while (!positionValid);
}
```

---

### 5.3 `Food_CheckCollision()`

**Which principle:** SRP, ISP, LSP  
**Where:** The function signature and the `active` guard.

**Why (SRP):** Pure geometric query — does food occupy the given position?
Does not modify food state or trigger any game response.

**Why (ISP):** `collision.cpp` depends only on this function — it never
needs `Food_Spawn` or `Food_Render`.

**Why (LSP):** `const Food*` — guaranteed non-mutating. The inactive guard
(`if (!food->active) return false`) is a correct, predictable behaviour any
subtype must honour.

```cpp
// [SRP] Pure query: does active food occupy this position? No side effects.
// [ISP] collision.cpp depends only on this — never on Spawn or Render.
// [LSP] const Food* — non-mutating contract; inactive guard is predictable.
bool Food_CheckCollision(const Food* food, Vector2 position)
{
    assert(food != nullptr);
    if (!food->active) return false;   // [LSP] subtype must honour this guard
    return (food->position.x == position.x) &&
           (food->position.y == position.y);
}
```

---

### 5.4 `Food_Render()`

**Which principle:** SRP, ISP, LSP  
**Where:** The function signature and its isolation from spawn logic.

**Why (SRP):** Draws food if active. Does not set position, manage activity
state, or interact with snake.

**Why (ISP):** Separated from spawn and collision. A replay viewer calls only
this function — it has no dependency on `Food_Spawn`.

**Why (LSP):** `const Food*` — non-mutating render contract, same reasoning
as `Snake_Render`.

```cpp
// [SRP] One job: draw food if it is active.
// [ISP] Render callers never depend on Spawn or CheckCollision.
// [LSP] const Food* — non-mutating; activity-check behaviour is predictable.
void Food_Render(const Food* food)
{
    assert(food != nullptr);
    if (food->active)
        DrawRectangleV(food->position, food->size, food->color);
}
```

---

## 6. `collision.cpp`

This entire file is an SRP application at the module level — the whole
file exists to own one domain: collision detection.

---

### 6.1 `Collision_CheckSnakeWithFood()`

**Which principle:** SRP, ISP, LSP  
**Where:** The function body and its `const` signatures.

**Why (SRP):** Answers one question — does the snake's head overlap active
food? Does not grow the snake, deactivate food, or update the score. Those
reactions are `game.cpp`'s job.

**Why (ISP):** Food-collision and self-collision are separate functions.
Code that only needs to check food-collision never has to reference
`Collision_CheckSnakeWithSelf`.

**Why (LSP):** Both pointers are `const` — this is a pure query with no side
effects. Any Snake or Food subtype satisfying the same field layout is safely
substitutable.

```cpp
// [SRP] Pure query: does head overlap active food? No mutation, no reaction.
// [ISP] Separate from self-collision — callers subscribe to only what they need.
// [LSP] const pointers — non-mutating; any compliant subtype is substitutable.
bool Collision_CheckSnakeWithFood(const Snake* snake, const Food* food)
{
    assert(snake != nullptr);
    assert(food  != nullptr);
    if (!food->active) return false;
    return (snake->segments[0].position.x == food->position.x) &&
           (snake->segments[0].position.y == food->position.y);
}
```

---

### 6.2 `Collision_CheckSnakeWithSelf()`

**Which principle:** SRP, OCP, DIP  
**Where:** The delegation call to `Snake_CheckSelfCollision`.

**Why (SRP):** One job — answer whether the snake has bitten itself.

**Why (OCP):** If the self-collision algorithm changes (e.g. bounding-box
instead of exact position), only `Snake_CheckSelfCollision` in `snake.cpp`
changes. This wrapper is closed for modification — it just delegates.

**Why (DIP):** Does not re-implement the traversal loop. It depends on
`Snake_CheckSelfCollision` — an abstraction from the Snake module — rather
than reaching into `snake->segments[]` directly.

```cpp
// [SRP] One job: answer self-collision. No other logic.
// [OCP] Algorithm change -> edit Snake_CheckSelfCollision, not this wrapper.
// [DIP] Delegates to Snake module abstraction — no raw field access here.
bool Collision_CheckSnakeWithSelf(const Snake* snake)
{
    assert(snake != nullptr);
    return Snake_CheckSelfCollision(snake);  // [DIP] depend on the abstraction
}
```

---

## 7. `renderer.cpp`

The entire file is an SRP application at the module level — every drawing
concern lives here and only here. No other file calls raylib draw primitives
directly.

---

### 7.1 `Renderer_DrawGrid()`

**Which principle:** SRP, DIP  
**Where:** The function body and the grid-dimension calls.

**Why (SRP):** Draws background grid lines only. Does not draw the snake,
food, or any UI overlay.

**Why (DIP):** Grid dimensions come from `Utils_GetGridColumns()` and
`Utils_GetGridRows()` — not from repeating `SCREEN_WIDTH / SQUARE_SIZE`
inside the renderer.

```cpp
// [SRP] One job: draw the grid lines. Nothing else.
// [DIP] Depends on Utils abstractions for grid size — not raw arithmetic.
void Renderer_DrawGrid(Vector2 gridOffset)
{
    const int cols = Utils_GetGridColumns();  // [DIP]
    const int rows = Utils_GetGridRows();     // [DIP]
    for (int i = 0; i <= cols; i++)
        DrawLineV(
            { gridOffset.x + static_cast<float>(i * SQUARE_SIZE), gridOffset.y },
            { gridOffset.x + static_cast<float>(i * SQUARE_SIZE),
              gridOffset.y + static_cast<float>(rows * SQUARE_SIZE) },
            LIGHTGRAY
        );
    for (int i = 0; i <= rows; i++)
        DrawLineV(
            { gridOffset.x,
              gridOffset.y + static_cast<float>(i * SQUARE_SIZE) },
            { gridOffset.x + static_cast<float>(cols * SQUARE_SIZE),
              gridOffset.y + static_cast<float>(i * SQUARE_SIZE) },
            LIGHTGRAY
        );
}
```

---

### 7.2 `Renderer_DrawPauseScreen()`

**Which principle:** SRP, OCP  
**Where:** The entire function.

**Why (SRP):** Draws only the pause banner. It does not check whether the
game is paused — that decision belongs to `game.cpp`. This function is always
called with intent; it just draws.

**Why (OCP):** Changing pause screen appearance (font, colour, animation)
touches only this function — all other overlays are untouched.

```cpp
// [SRP] One job: draw the pause banner. The decision to show it is elsewhere.
// [OCP] Appearance changes here without touching any other renderer function.
void Renderer_DrawPauseScreen(void)
{
    const char* pauseText  = "GAME PAUSED";
    constexpr int fontSize = 40;
    const int textWidth    = MeasureText(pauseText, fontSize);
    DrawText(pauseText,
             SCREEN_WIDTH / 2 - textWidth / 2,
             SCREEN_HEIGHT / 2 - 40,
             fontSize, GRAY);
}
```

---

### 7.3 `Renderer_DrawFreezeEffect()`

**Which principle:** SRP, OCP  
**Where:** The entire function.

**Why (SRP):** Draws only the translucent red crash-flash overlay.

**Why (OCP):** Changing the colour or opacity of the crash effect edits only
this function — no other renderer function is opened.

```cpp
// [SRP] One job: draw the red crash-flash. Nothing else.
// [OCP] Colour/opacity change touches only this function.
void Renderer_DrawFreezeEffect(void)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(RED, 0.3f));
}
```

---

### 7.4 `Renderer_DrawGameOver()`

**Which principle:** SRP, DIP  
**Where:** The function parameter (`int finalScore`) and the function body.

**Why (SRP):** Draws the full game-over UI — title, score, restart hint. It
does not calculate the score, check for high scores, or modify any state.

**Why (DIP):** Receives `int finalScore` — a primitive, stable type — rather
than `const GameState*`. The renderer depends on the smallest possible
abstraction. If `GameState` gains ten new fields, this function is completely
unaffected.

```cpp
// [SRP] One job: draw the game-over screen. Score is received, not computed.
// [DIP] Depends on int (stable primitive), NOT on GameState (high-level struct).
void Renderer_DrawGameOver(int finalScore)
{
    const char* gameOverText    = "GAME OVER!";
    constexpr int titleFontSize = 40;
    const int     titleWidth    = MeasureText(gameOverText, titleFontSize);
    DrawText(gameOverText,
             SCREEN_WIDTH / 2 - titleWidth / 2,
             SCREEN_HEIGHT / 2 - 80,
             titleFontSize, RED);

    const char* scoreText      = TextFormat("FINAL SCORE: %d", finalScore);
    constexpr int scoreFontSize = 30;
    const int     scoreWidth    = MeasureText(scoreText, scoreFontSize);
    DrawText(scoreText,
             SCREEN_WIDTH / 2 - scoreWidth / 2,
             SCREEN_HEIGHT / 2 - 30,
             scoreFontSize, YELLOW);

    const char* restartText      = "PRESS [ENTER] TO PLAY AGAIN";
    constexpr int restartFontSize = 20;
    const int     restartWidth    = MeasureText(restartText, restartFontSize);
    DrawText(restartText,
             SCREEN_WIDTH / 2 - restartWidth / 2,
             SCREEN_HEIGHT / 2 + 20,
             restartFontSize, GRAY);
}
```

---

## 8. `utils.cpp`

The entire file is a DIP application at the module level — it is the single
source of truth for grid arithmetic. Every other module depends on this
abstraction layer rather than on raw calculations.

---

### 8.1 `Utils_GetGridColumns()` and `Utils_GetGridRows()`

**Which principle:** SRP, DIP  
**Where:** Both getter functions.

**Why (SRP):** Each function owns exactly one calculation. Columns and rows
are separate functions — not combined into one "get grid size" function —
because changing the column formula does not require changing the row formula.

**Why (DIP):** All other modules call these functions instead of writing
`SCREEN_WIDTH / SQUARE_SIZE` inline. If the layout algorithm changes
(e.g. supporting multiple resolutions), only these two functions are updated.
Every caller picks up the change automatically.

```cpp
// [SRP] One calculation each. Columns and rows are independent concerns.
// [DIP] The authoritative abstraction for grid dimensions. All modules depend here.
int Utils_GetGridColumns(void) { return SCREEN_WIDTH / SQUARE_SIZE; }
int Utils_GetGridRows(void)    { return SCREEN_HEIGHT / SQUARE_SIZE; }
```

---

### 8.2 `Utils_CalculateGridOffset()`

**Which principle:** SRP, DIP  
**Where:** The function body.

**Why (SRP):** One job — compute the pixel offset that centres the grid on
screen. Does not draw anything or modify any struct.

**Why (DIP):** Calls `Utils_GetGridColumns()` and `Utils_GetGridRows()`
rather than repeating the division — even inside `utils.cpp` itself, one
abstraction depends on another rather than on the raw formula.

```cpp
// [SRP] One job: compute the centering pixel offset. No drawing, no mutation.
// [DIP] Calls own abstractions (GetGridColumns/Rows) rather than raw division.
Vector2 Utils_CalculateGridOffset(void)
{
    const int cols = Utils_GetGridColumns();   // [DIP] use own abstraction
    const int rows = Utils_GetGridRows();      // [DIP] use own abstraction
    return {
        static_cast<float>((SCREEN_WIDTH  - cols * SQUARE_SIZE) / 2),
        static_cast<float>((SCREEN_HEIGHT - rows * SQUARE_SIZE) / 2)
    };
}
```

---

### 8.3 `Utils_IsPositionValid()`

**Which principle:** SRP, ISP, DIP  
**Where:** The function body.

**Why (SRP):** Pure validation — answers only "is this position inside the
grid?" No repositioning, no drawing.

**Why (ISP):** Callers that need only bounds validation depend on this one
function — they are not forced to import spawn or movement utilities.

**Why (DIP):** Derives boundary limits from `Utils_GetGridColumns()` and
`Utils_GetGridRows()` — not from raw constant arithmetic.

```cpp
// [SRP] Pure query: is this position within legal grid bounds?
// [ISP] Bounds-check callers depend only on this function.
// [DIP] Boundary limits from Utils abstractions, not raw arithmetic.
bool Utils_IsPositionValid(Vector2 position, Vector2 gridOffset)
{
    const int   cols = Utils_GetGridColumns();   // [DIP]
    const int   rows = Utils_GetGridRows();      // [DIP]
    const float minX = gridOffset.x;
    const float minY = gridOffset.y;
    const float maxX = gridOffset.x + static_cast<float>((cols - 1) * SQUARE_SIZE);
    const float maxY = gridOffset.y + static_cast<float>((rows - 1) * SQUARE_SIZE);
    return (position.x >= minX && position.x <= maxX &&
            position.y >= minY && position.y <= maxY);
}
```

---

## 9. `game.cpp`

`game.cpp` is where the highest concentration of all five principles comes
together, because this is the top-level coordinator — it sits above every
other module.

---

### 9.1 Static module-level variables

**Which principle:** SRP  
**Where:** The three static variables at the top of `game.cpp`.

**Why:** State ownership is confined to one translation unit. No other module
can mutate `gameState`, `playerSnake`, or `gameFruit` directly. Each
variable owns one entity's data.

```cpp
// [SRP] Each variable owns exactly one entity. game.cpp is the only owner.
static GameState gameState   = { 0 };
static Snake     playerSnake = { 0 };
static Food      gameFruit   = { 0 };
```

---

### 9.2 `Game_Initialize()`

**Which principle:** SRP, DIP  
**Where:** The entire function.

**Why (SRP):** One job — reset all subsystems to a clean starting state. Does
not update positions or draw anything.

**Why (DIP):** Delegates entity setup to `Snake_Initialize` and
`Food_Initialize` — never writes raw struct fields of Snake or Food directly.
Grid offset comes from `Utils_CalculateGridOffset()`.

```cpp
// [SRP] One job: reset all subsystems. No movement, no drawing.
// [DIP] Delegates to module abstractions — never writes Snake/Food fields raw.
void Game_Initialize(void)
{
    gameState.framesCounter = 0;
    gameState.playerScore   = 0;
    gameState.isGameOver    = false;
    gameState.isPaused      = false;
    gameState.freezeCounter = 0;
    gameState.gridOffset    = Utils_CalculateGridOffset();   // [DIP]
    Snake_Initialize(&playerSnake, gameState.gridOffset, gameState.gridOffset); // [DIP]
    Food_Initialize(&gameFruit);                             // [DIP]
}
```

---

### 9.3 `Game_Update()`

**Which principle:** SRP, OCP, ISP, DIP  
**Where:** The entire function body.

**Why (SRP):** Advances game logic by one tick. Never draws anything — that
is `Game_Render`'s job. It coordinates; it does not implement any subsystem's
logic itself.

**Why (OCP):** Adding a new entity (obstacle, power-up) means adding a new
call here — `Snake_UpdatePosition` and `Food_Spawn` are never opened.

**Why (ISP):** Calls only update/logic API surface — never a render function.

**Why (DIP):** Every call goes through a module-level function abstraction.
`game.cpp` never accesses `playerSnake.segments[0].position` directly.

```cpp
// [SRP] Advances logic only — never draws.
// [OCP] New entity = new call here; existing module functions untouched.
// [ISP] Calls only update/logic API. No render functions ever called here.
// [DIP] All interaction is through module-level abstractions, not raw fields.
void Game_Update(void)
{
    if (!gameState.isGameOver)
    {
        if (IsKeyPressed('P')) gameState.isPaused = !gameState.isPaused;

        if (!gameState.isPaused)
        {
            if (gameState.freezeCounter > 0) {       // freeze countdown
                gameState.freezeCounter--;
                if (gameState.freezeCounter == 0) gameState.isGameOver = true;
                return;
            }

            Snake_ProcessInput(&playerSnake);                          // [DIP]
            Snake_UpdatePosition(&playerSnake, gameState.framesCounter); // [DIP]
            Snake_HandleWrapAround(&playerSnake, gameState.gridOffset);  // [DIP]

            if (Snake_CheckSelfCollision(&playerSnake))                // [DIP]
                gameState.freezeCounter = FREEZE_DURATION;

            if (!gameFruit.active)
                Food_Spawn(&gameFruit, &playerSnake, gameState.gridOffset); // [DIP]

            if (Collision_CheckSnakeWithFood(&playerSnake, &gameFruit)) { // [DIP]
                Snake_Grow(&playerSnake);
                gameFruit.active = false;
                gameState.playerScore++;
            }
            gameState.framesCounter++;
        }
    }
    else {
        if (IsKeyPressed(KEY_ENTER)) Game_Initialize();
    }
}
```

---

### 9.4 `Game_Render()`

**Which principle:** SRP, ISP, DIP  
**Where:** The entire function body.

**Why (SRP):** Draws the current frame only. Never modifies positions,
processes input, or triggers state transitions.

**Why (ISP):** Calls only render API — `Snake_Render`, `Food_Render`,
`Renderer_Draw*`. Never calls `Snake_ProcessInput` or `Food_Spawn`.

**Why (DIP):** Passes `gameState.playerScore` (a primitive `int`) to
`Renderer_DrawGameOver` — not the full `GameState` struct. The renderer
depends on the minimal abstraction it needs.

```cpp
// [SRP] Draws the current frame only. No logic, no state changes.
// [ISP] Calls only render API. Update functions are never called here.
// [DIP] Passes int score to renderer — not the full GameState struct.
void Game_Render(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    if (!gameState.isGameOver)
    {
        Renderer_DrawGrid(gameState.gridOffset);    // [ISP] render API only
        Snake_Render(&playerSnake);                 // [ISP] render API only
        Food_Render(&gameFruit);                    // [ISP] render API only
        if (gameState.isPaused)    Renderer_DrawPauseScreen();
        if (gameState.freezeCounter > 0) Renderer_DrawFreezeEffect();
    }
    else
    {
        Renderer_DrawGameOver(gameState.playerScore); // [DIP] int, not GameState*
    }

    EndDrawing();
}
```

---

### 9.5 `Game_Cleanup()`

**Which principle:** SRP  
**Where:** The function's defined scope and placeholder comment.

**Why:** One job — release resources. The body is currently empty because
there is no dynamic memory, but the function exists as the *designated* place
for future resource cleanup. When textures or audio are added, they come
here — not scattered across `Game_Update` or `main`.

```cpp
// [SRP] Designated owner of resource release. Nothing else lives here.
void Game_Cleanup(void)
{
    // Reserved: future texture/audio/heap cleanup belongs here only.
}
```

---

### 9.6 `Game_UpdateAndDraw()`

**Which principle:** SRP, DIP  
**Where:** The two-line function body.

**Why (SRP):** This thin wrapper's only job is to sequence update then render
for platforms (desktop and web/emscripten) that need a single callback
function. Logic lives in `Game_Update`; drawing lives in `Game_Render`.

**Why (DIP):** Depends only on `Game_Update` and `Game_Render` abstractions.
The web platform's emscripten loop and the desktop loop both call this without
knowing anything about snake movement or rendering primitives.

```cpp
// [SRP] Sequences update then render. All logic is in the two delegates.
// [DIP] Depends on Game_Update and Game_Render abstractions only.
void Game_UpdateAndDraw(void)
{
    Game_Update();
    Game_Render();
}
```

---

## 10. Cross-File Principle Summary

### Where each principle does the most work

| Principle | Primary home | What it prevents |
|-----------|-------------|------------------|
| **SRP** | Every function across all files | Bugs hiding behind unrelated code; wide blast radius when changing one feature |
| **OCP** | `snake_game.h` interfaces; `food.cpp` spawn strategy; `renderer.cpp` overlays | Regressions caused by editing already-tested functions to add new features |
| **LSP** | Every `const` pointer signature across all files | Subtypes that silently break callers; unsafe mock/test-double substitution |
| **ISP** | `IRenderable` vs `IUpdatable`; `Game_Update` vs `Game_Render` | A headless simulation linking against drawing code; a renderer linking against physics |
| **DIP** | `utils.cpp` grid abstractions; `Renderer_DrawGameOver(int)` parameter; `game.cpp` delegation pattern | Grid formula duplicated in five files; renderer breaking when GameState gains a new field |

### The dependency chain the principles create

```
main.cpp
  └── depends on → Game_* abstractions (DIP)
        └── game.cpp coordinates via → Snake_*, Food_*, Collision_*, Renderer_* (DIP, ISP)
              ├── snake.cpp  — one concern per function (SRP), const contracts (LSP)
              ├── food.cpp   — one concern per function (SRP), const contracts (LSP)
              ├── collision.cpp — delegates to Snake module (DIP), separated queries (ISP)
              ├── renderer.cpp  — receives primitives not structs (DIP), one overlay per fn (SRP, OCP)
              └── utils.cpp     — single source of grid arithmetic (DIP, SRP)
```

Every arrow in that chain points from high-level to abstraction, never from
high-level to raw low-level detail. That is the entire goal of SOLID.

---

*Document generated as part of the Advanced Programming Lab — February 2026*
*Companion files: `snake_game.h`, `main.cpp`, `snake.cpp`, `food.cpp`,*
*`collision.cpp`, `renderer.cpp`, `utils.cpp`, `game.cpp`*
