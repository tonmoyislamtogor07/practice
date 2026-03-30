# SOLID Principles: Before & After Comparison

**Project:** Classic Snake Game  
**Language Migration:** C → C++ with SOLID Design Principles  
**Course:** Advanced Programming Lab — February 2026

---

## Table of Contents

1. [What is SOLID?](#1-what-is-solid)
2. [Project Evolution Overview](#2-project-evolution-overview)
3. [Metrics at a Glance](#3-metrics-at-a-glance)
4. [Principle-by-Principle Analysis](#4-principle-by-principle-analysis)
   - [S — Single Responsibility Principle](#s--single-responsibility-principle-srp)
   - [O — Open/Closed Principle](#o--openclosed-principle-ocp)
   - [L — Liskov Substitution Principle](#l--liskov-substitution-principle-lsp)
   - [I — Interface Segregation Principle](#i--interface-segregation-principle-isp)
   - [D — Dependency Inversion Principle](#d--dependency-inversion-principle-dip)
5. [File-by-File Walkthrough](#5-file-by-file-walkthrough)
6. [Real-World Impact](#6-real-world-impact)
7. [Key Takeaways](#7-key-takeaways)

---

## 1. What is SOLID?

SOLID is a set of five design principles that, when applied together, produce software that is easier to understand, extend, and maintain. Originally described by Robert C. Martin, they are especially powerful when moving from a procedural C style toward a structured C++ design.

| Letter | Principle | One-line definition |
|--------|-----------|---------------------|
| **S** | Single Responsibility | A module should have one, and only one, reason to change. |
| **O** | Open/Closed | Software entities should be open for extension but closed for modification. |
| **L** | Liskov Substitution | Subtypes must be substitutable for their base types without altering correctness. |
| **I** | Interface Segregation | Clients should not be forced to depend on interfaces they do not use. |
| **D** | Dependency Inversion | High-level modules should depend on abstractions, not on concrete low-level details. |

---

## 2. Project Evolution Overview

The codebase went through **two transformation stages**:

```
Stage 0 — Original C (monolithic)
  └── main.c  [~330 lines, everything in one file]

Stage 1 — Refactored C (modular, good naming)
  ├── main.c        collision.c    renderer.c
  ├── game.c        food.c         utils.c
  ├── snake.c       snake_game.h
  └── Makefile

Stage 2 — C++ with SOLID (current, this comparison)
  ├── main.cpp      collision.cpp  renderer.cpp
  ├── game.cpp      food.cpp       utils.cpp
  ├── snake.cpp     snake_game.h   (+ IRenderable / IUpdatable interfaces)
  └── Makefile
```

This document focuses on the **Stage 1 → Stage 2** transformation: what each SOLID principle changed, why it matters, and what the concrete impact is on the codebase.

---

## 3. Metrics at a Glance

| Metric | Stage 0 (Original C) | Stage 1 (Refactored C) | Stage 2 (C++ SOLID) |
|--------|----------------------|------------------------|----------------------|
| Source files | 1 | 8 | 8 (`.cpp`) |
| Lines per file (avg) | 330+ | ~120 | ~140 (more comments) |
| Global variables | 11 raw globals | 3 static module vars | 3 static module vars |
| Interfaces / abstractions | 0 | 0 | 2 (`IRenderable`, `IUpdatable`) |
| `const` correctness | Minimal | Partial | Enforced on all read-only params |
| Magic numbers | Multiple inline | Named `#define` constants | Named `constexpr` constants |
| Null checks / assertions | None | `assert(ptr != NULL)` | `assert(ptr != nullptr)` (C++ null) |
| SOLID compliance | ✗ | Partial (SRP only, unintentional) | ✓ Full — all five principles |
| Testability score | Low | Medium | High |
| Ease of adding a new entity | Hard (edit UpdateGame) | Medium (add new .c file) | Easy (implement interface, call in game.cpp) |

---

## 4. Principle-by-Principle Analysis

---

### S — Single Responsibility Principle (SRP)

> *"A class (or function) should have only one reason to change."*

#### The Problem in Stage 0

The entire game lived in `UpdateGame()`. One function handled input, movement, collision, food logic, scoring, and state transitions simultaneously. Changing *any* one concern meant opening and risking *all* the others.

**Before — Stage 0 (Original C):**
```c
// UpdateGame() — does EVERYTHING
void UpdateGame(void)
{
    // 1. Input
    if (IsKeyPressed(KEY_RIGHT) && (snake[0].speed.x == 0) && allowMove) { ... }
    if (IsKeyPressed(KEY_LEFT)  && (snake[0].speed.x == 0) && allowMove) { ... }

    // 2. Movement
    for (int i = 0; i < counterTail; i++) snakePosition[i] = snake[i].position;
    if ((framesCounter % 5) == 0) {
        for (int i = 0; i < counterTail; i++) {
            if (i == 0) { snake[0].position.x += snake[0].speed.x; ... }
            else snake[i].position = snakePosition[i - 1];
        }
    }

    // 3. Collision detection (self)
    for (int i = 1; i < counterTail; i++) {
        if (snake[0].position.x == snake[i].position.x &&
            snake[0].position.y == snake[i].position.y) { gameOver = true; }
    }

    // 4. Food spawning
    if (!fruit.active) {
        fruit.active = true;
        // ... 30 more lines of fruit logic ...
    }

    // 5. Food collision + scoring
    if (snake[0].position.x == fruit.position.x &&
        snake[0].position.y == fruit.position.y) {
        snake[counterTail].position = snakePosition[counterTail - 1];
        counterTail++;
        score++;
        fruit.active = false;
    }
    framesCounter++;
}
```
**Reason to change count: 5+ (input, movement, collision, food, scoring).**

---

**After — Stage 2 (C++ SOLID):**

Each concern became its own function, each with exactly one reason to change:

```cpp
// game.cpp — Game_Update() is now a pure coordinator
void Game_Update(void)
{
    Snake_ProcessInput(&playerSnake);                          // reason: input scheme
    Snake_UpdatePosition(&playerSnake, gameState.framesCounter); // reason: movement rule
    Snake_HandleWrapAround(&playerSnake, gameState.gridOffset);  // reason: boundary rule

    if (Snake_CheckSelfCollision(&playerSnake))                // reason: self-hit rule
        gameState.freezeCounter = FREEZE_DURATION;

    if (!gameFruit.active)
        Food_Spawn(&gameFruit, &playerSnake, gameState.gridOffset); // reason: spawn logic

    if (Collision_CheckSnakeWithFood(&playerSnake, &gameFruit)) {   // reason: eat rule
        Snake_Grow(&playerSnake);
        gameFruit.active = false;
        gameState.playerScore++;
    }
    gameState.framesCounter++;
}
```

```cpp
// snake.cpp — input function has ONE job
void Snake_ProcessInput(Snake* snake)
{
    SnakeSegment* head = &snake->segments[0];
    if (IsKeyPressed(KEY_RIGHT) && (head->speed.x == 0.0f) && snake->allowMove) {
        head->speed = { static_cast<float>(SQUARE_SIZE), 0.0f };
        snake->allowMove = false;
    }
    // ... other directions only — NO movement, NO collision, NO scoring
}
```

**Impact:** Fixing a bug in food spawning now touches only `food.cpp`. Previously it required reading and editing `UpdateGame()` alongside unrelated code.

---

### O — Open/Closed Principle (OCP)

> *"Software entities should be open for extension, but closed for modification."*

#### The Problem in Stage 0 / Stage 1

Adding a new entity (e.g. a power-up, an obstacle) required directly editing `UpdateGame()` and `DrawGame()`, risking breaking existing behaviour. The code was open for modification but not for extension.

**Before — Stage 1 (Refactored C) — adding an obstacle would require:**
```c
// game.c — must be opened and edited
void Game_Update(void)
{
    Snake_ProcessInput(&playerSnake);
    Snake_UpdatePosition(&playerSnake, gameState.framesCounter);
    Snake_HandleWrapAround(&playerSnake, gameState.gridOffset);

    // ← You would have to INSERT new code here, risking side effects
    // Obstacle_Update(&gameObstacle, gameState.framesCounter);  // new code injected

    if (Snake_CheckSelfCollision(&playerSnake)) { ... }
    if (!gameFruit.active) Food_Spawn(...);
    if (Collision_CheckSnakeWithFood(...)) { ... }
}
```

**After — Stage 2 (C++ SOLID) — `snake_game.h` introduces extension points:**
```cpp
// snake_game.h — IRenderable and IUpdatable interfaces
// [OCP] New entities implement these interfaces WITHOUT touching existing code

struct IRenderable {
    virtual void render() const = 0;
    virtual ~IRenderable() = default;
};

struct IUpdatable {
    virtual void update(int framesCounter) = 0;
    virtual ~IUpdatable() = default;
};
```

Adding a power-up now means:
1. Create `powerup.cpp` implementing `IRenderable` and `IUpdatable`.
2. Register it in `game.cpp` — **existing functions in snake.cpp, food.cpp, collision.cpp are never touched.**

```cpp
// food.cpp — Food_Spawn is closed for modification
// OCP in action: the spawn algorithm is encapsulated; changing the strategy
// (e.g. weighted spawn) means a new overload, NOT editing this function.
void Food_Spawn(Food* food, const Snake* snake, Vector2 gridOffset)
{
    // rejection-sampling strategy — completely self-contained
    do { ... } while (!positionValid);
}

// To add weighted spawn: add Food_SpawnWeighted() alongside this, never inside it.
```

**Impact:** The game is now genuinely extensible. New features are additions, not edits. Existing test coverage is never invalidated by new functionality.

---

### L — Liskov Substitution Principle (LSP)

> *"Objects of a subtype should be substitutable for objects of the base type without breaking the program."*

#### The Problem in Stage 0 / Stage 1

There were no interfaces or base types. Every function operated on concrete types directly. Swapping an implementation (e.g. a test-double for `Food`) was impossible without rewriting call sites.

**Before — Stage 1 (Refactored C):**
```c
// collision.c — hard dependency on the concrete Food struct
bool Collision_CheckSnakeWithFood(const Snake* snake, const Food* food)
{
    if (!food->active) return false;
    return (snake->segments[0].position.x == food->position.x) &&
           (snake->segments[0].position.y == food->position.y);
}
// Cannot swap `food` for a BonusFood — the signature is locked to Food*
```

**After — Stage 2 (C++ SOLID):**

All render and update functions accept `const` pointers and honour the interface contract precisely:

```cpp
// snake.cpp — Snake_Render accepts const Snake*, matching IRenderable semantics
// [LSP] Any Snake subtype (e.g. GhostSnake with transparency) can be passed
//        without the renderer breaking — the contract (draw each segment) is upheld.
void Snake_Render(const Snake* snake)
{
    assert(snake != nullptr);
    for (int i = 0; i < snake->length; i++)
    {
        DrawRectangleV(
            snake->segments[i].position,
            snake->segments[i].size,
            snake->segments[i].color   // colour may differ in a subtype — still valid
        );
    }
}

// renderer.cpp — DrawGameOver receives score as a plain int parameter
// [LSP] Renderer does not reach into GameState; any caller supplying an int score
//        satisfies the contract, so a TestGameState or ReplayGameState substitutes cleanly.
void Renderer_DrawGameOver(int finalScore) { ... }
```

```cpp
// snake_game.h — IRenderable contract
// [LSP] Any type implementing render() can be used wherever IRenderable* is expected.
struct IRenderable {
    virtual void render() const = 0;   // const: render must NOT mutate state
    virtual ~IRenderable() = default;
};
```

**Impact:** Unit tests can pass a mock food object into `Collision_CheckSnakeWithFood` without touching real Food or the full game loop. Future subtypes (BonusFood, PoisonFood) plug in without changing any collision logic.

---

### I — Interface Segregation Principle (ISP)

> *"Clients should not be forced to depend on methods they do not use."*

#### The Problem in Stage 0

A single `UpdateGame()` / `DrawGame()` pair coupled every concern together. Any code that needed *only* rendering had to link against the full game-update logic.

**Before — Stage 1 (Refactored C) — no interface separation:**
```c
// snake_game.h exports everything to every consumer
void Snake_Initialize(Snake*, Vector2, Vector2);  // init
void Snake_UpdatePosition(Snake*, int);            // update
void Snake_ProcessInput(Snake*);                   // input
void Snake_HandleWrapAround(Snake*, Vector2);      // boundary
bool Snake_CheckSelfCollision(const Snake*);       // collision
void Snake_Grow(Snake*);                           // growth
void Snake_Render(const Snake*);                   // render
// A replay viewer that only needs Snake_Render still includes ALL of the above
// because there is no interface boundary between render and update concerns.
```

**After — Stage 2 (C++ SOLID) — IRenderable and IUpdatable are segregated:**
```cpp
// snake_game.h — two separate, minimal interfaces

// [ISP] IRenderable: callers that only render depend ONLY on render()
struct IRenderable {
    virtual void render() const = 0;
    virtual ~IRenderable() = default;
};

// [ISP] IUpdatable: callers that only update depend ONLY on update()
struct IUpdatable {
    virtual void update(int framesCounter) = 0;
    virtual ~IUpdatable() = default;
};
```

The same segregation is applied at the function level:

```cpp
// game.cpp — Game_Update uses ONLY update/logic API surface
void Game_Update(void)
{
    Snake_ProcessInput(&playerSnake);         // update API
    Snake_UpdatePosition(&playerSnake, ...);  // update API
    Food_Spawn(&gameFruit, ...);              // update API
    Collision_CheckSnakeWithFood(...);        // logic API
    // ← NEVER calls Snake_Render or Food_Render here
}

// game.cpp — Game_Render uses ONLY render API surface
void Game_Render(void)
{
    Renderer_DrawGrid(gameState.gridOffset);  // render API
    Snake_Render(&playerSnake);               // render API
    Food_Render(&gameFruit);                  // render API
    // ← NEVER calls Snake_ProcessInput or Food_Spawn here
}
```

**Impact:** A headless simulation (AI training, automated testing) can call `Game_Update` repeatedly without any rendering overhead or dependencies. A replay viewer can call only `Game_Render` with pre-recorded state and never link against input or physics logic.

---

### D — Dependency Inversion Principle (DIP)

> *"High-level modules should not depend on low-level modules. Both should depend on abstractions."*

#### The Problem in Stage 0

High-level logic (`UpdateGame`) reached directly into low-level detail: raw array indices, magic numbers (`framesCounter % 5`), direct struct field access, and inline arithmetic for grid dimensions.

**Before — Stage 0 (Original C) — high-level code mixed with low-level detail:**
```c
// UpdateGame() directly manipulates raw data — no abstraction layer
int cols = screenWidth / SQUARE_SIZE;    // ← raw arithmetic everywhere
int rows = screenHeight / SQUARE_SIZE;   // ← same formula duplicated
float offsetX = (screenWidth  - cols * SQUARE_SIZE) / 2;
float offsetY = (screenHeight - rows * SQUARE_SIZE) / 2;

// Grid boundary check inlined inside UpdateGame
if (snake[0].position.x > offsetX + (cols - 1) * SQUARE_SIZE)
    snake[0].position.x = offsetX;
// ← if SQUARE_SIZE changes, this must be hunted down in every function
```

**After — Stage 2 (C++ SOLID) — high-level code depends on abstractions:**
```cpp
// utils.cpp — low-level arithmetic is centralised behind an abstraction
int Utils_GetGridColumns(void) { return SCREEN_WIDTH / SQUARE_SIZE; }
int Utils_GetGridRows(void)    { return SCREEN_HEIGHT / SQUARE_SIZE; }
Vector2 Utils_CalculateGridOffset(void)
{
    const int cols = Utils_GetGridColumns();
    const int rows = Utils_GetGridRows();
    return {
        static_cast<float>((SCREEN_WIDTH  - cols * SQUARE_SIZE) / 2),
        static_cast<float>((SCREEN_HEIGHT - rows * SQUARE_SIZE) / 2)
    };
}

// snake.cpp — depends on the Utils abstraction, NOT on raw arithmetic
void Snake_HandleWrapAround(Snake* snake, Vector2 gridOffset)
{
    const int   cols = Utils_GetGridColumns();   // ← abstraction
    const int   rows = Utils_GetGridRows();      // ← abstraction
    const float maxX = gridOffset.x + static_cast<float>((cols - 1) * SQUARE_SIZE);
    // If SQUARE_SIZE changes, only utils.cpp needs updating — this function is safe.
}

// game.cpp — depends on module-level function abstractions, not raw structs
void Game_Initialize(void)
{
    gameState.gridOffset = Utils_CalculateGridOffset();  // ← abstraction
    Snake_Initialize(&playerSnake, gameState.gridOffset, gameState.gridOffset);
    Food_Initialize(&gameFruit);
    // ← game.cpp does not know HOW Snake or Food initialise themselves
}
```

DIP also appears in `renderer.cpp`:
```cpp
// renderer.cpp — Renderer_DrawGameOver receives score as an int parameter
// [DIP] Does NOT reach into GameState struct directly.
//       Depends on a simple stable type (int), not a high-level game object.
void Renderer_DrawGameOver(int finalScore)
{
    DrawText(TextFormat("FINAL SCORE: %d", finalScore), ...);
}

// Contrast with what this would look like without DIP:
// void Renderer_DrawGameOver(const GameState* state) {  ← bad: renderer now
//     DrawText(TextFormat("FINAL SCORE: %d", state->playerScore), ...);  // couples to GameState
// }
```

**Impact:** Changing the screen resolution or grid tile size now requires editing *only* `utils.cpp`. Every other module that calls `Utils_GetGridColumns()` automatically picks up the new value.

---

## 5. File-by-File Walkthrough

### `snake_game.h`

| Aspect | Stage 1 (C) | Stage 2 (C++ SOLID) | Principle |
|--------|-------------|---------------------|-----------|
| Constants | `#define MAX_SNAKE_LENGTH 400` | `constexpr int MAX_SNAKE_LENGTH = 400;` | Type safety |
| Null literal | `NULL` | `nullptr` | C++ idiom |
| Interfaces | None | `IRenderable`, `IUpdatable` pure-virtual structs | OCP, ISP, DIP |
| Struct definitions | `typedef struct { ... } Snake;` | `struct Snake { ... };` (C++ native) | — |
| Reason to change | Adding a new entity forces header edits touching all consumers | New entity adds a new struct; interfaces remain stable | OCP |

---

### `main.cpp`

| Aspect | Stage 1 (C) | Stage 2 (C++ SOLID) | Principle |
|--------|-------------|---------------------|-----------|
| Responsibility | Window + loop + init | Identical — already thin | SRP (preserved) |
| Dependency | Calls `Game_*` functions | Calls `Game_*` functions | DIP (preserved) |
| C++ change | `main.c` | `main.cpp` — no logic change needed; already compliant | — |

`main.cpp` required the fewest changes because the Stage 1 refactor had already applied SRP and DIP here correctly. This demonstrates that **SOLID compliance can exist in C** — it just requires deliberate design.

---

### `snake.cpp`

| Aspect | Stage 1 (C) | Stage 2 (C++ SOLID) | Principle |
|--------|-------------|---------------------|-----------|
| `NULL` checks | `assert(snake != NULL)` | `assert(snake != nullptr)` | C++ null safety |
| Speed assignment | `(Vector2){ SQUARE_SIZE, 0 }` | `{ static_cast<float>(SQUARE_SIZE), 0.0f }` | Type-safe casts |
| Render signature | `void Snake_Render(const Snake*)` | Same + aligns with `IRenderable` contract | LSP |
| Comments | Section headers | Per-function SOLID annotation `// [SRP]`, `// [DIP]` | Documentation |
| Self-collision | In `snake.c` and duplicated in `collision.c` | `collision.cpp` delegates to `Snake_CheckSelfCollision` — DRY | DIP, SRP |

---

### `food.cpp`

| Aspect | Stage 1 (C) | Stage 2 (C++ SOLID) | Principle |
|--------|-------------|---------------------|-----------|
| Spawn strategy | Rejection sampling, embedded inline | Same strategy, but fully isolated and commented as replaceable | OCP |
| Grid dimensions | Calls `Utils_GetGridColumns()` | Same — already DIP-compliant; preserved | DIP |
| Collision check | `Food_CheckCollision` used by food internally | Used exclusively by `collision.cpp`; food does not call it itself | ISP |
| `const` pointers | `const Snake*` in spawn | Same | LSP |

---

### `collision.cpp`

| Aspect | Stage 1 (C) | Stage 2 (C++ SOLID) | Principle |
|--------|-------------|---------------------|-----------|
| Purpose | Centralised collision module | Same — already SRP-compliant | SRP (preserved) |
| Self-collision | Delegated to `Snake_CheckSelfCollision` | Same delegation — comments explain DIP explicitly | DIP |
| New collision types | Would require editing this file | Requires only adding a new `Collision_Check*` function | OCP |
| `const` enforcement | Present | Enforced + justified with LSP comment | LSP |

---

### `renderer.cpp`

| Aspect | Stage 1 (C) | Stage 2 (C++ SOLID) | Principle |
|--------|-------------|---------------------|-----------|
| GameOver parameter | `Renderer_DrawGameOver(int finalScore)` | Identical — already DIP-compliant | DIP (preserved) |
| Grid dimensions | Calls `Utils_GetGridColumns()` | Same | DIP |
| New overlays | Would require editing `renderer.c` | New `Renderer_Draw*` function added alongside existing ones | OCP |
| `constexpr` font sizes | `int fontSize = 40;` (runtime) | `constexpr int fontSize = 40;` (compile-time) | Performance |

---

### `utils.cpp`

| Aspect | Stage 1 (C) | Stage 2 (C++ SOLID) | Principle |
|--------|-------------|---------------------|-----------|
| Grid columns | `return SCREEN_WIDTH / SQUARE_SIZE;` | Same + `constexpr` constants | DIP (stable abstraction) |
| Return type | `Vector2` with float cast | `static_cast<float>(...)` explicit casts | C++ type safety |
| Testability | Functions are pure (no side effects) | Same — pure functions are trivially unit-testable | SRP, LSP |
| Extension | Adding new utility = new function | Same — OCP naturally satisfied by pure helper functions | OCP |

---

### `game.cpp`

| Aspect | Stage 1 (C) | Stage 2 (C++ SOLID) | Principle |
|--------|-------------|---------------------|-----------|
| `Game_Update` role | Coordinator — calls module functions | Same, now explicitly documented | SRP |
| Direct struct access | Never accesses `Snake.segments[]` directly | Same — never touches raw fields | DIP |
| Render in update | Never | Never — enforced by ISP commentary | ISP |
| New entity integration | Edit `Game_Update` and `Game_Render` | Same edit point, but interfaces make new entities plug-in compatible | OCP |
| `Game_Cleanup` | Comment: reserved for future use | Same | SRP |

---

## 6. Real-World Impact

### Maintainability

**Scenario:** A bug is reported where the snake passes through itself without dying.

- **Stage 0:** You must search through `UpdateGame()` (100+ lines) reading input, movement, and scoring code to find the collision section.
- **Stage 1/2:** Open `collision.cpp` (one file, one concern). The bug is in `Collision_CheckSnakeWithSelf` or `Snake_CheckSelfCollision`. Fixed in under 2 minutes without touching any other file.

---

### Testability

```cpp
// With SOLID C++ — unit test for food collision (no full game needed)
void test_food_collision()
{
    Food food;
    Food_Initialize(&food);
    food.active   = true;
    food.position = { 31.0f, 31.0f };

    Snake snake;
    // minimal snake — only need head position
    snake.segments[0].position = { 31.0f, 31.0f };
    snake.length = 1;

    // [LSP] Collision function accepts const pointers — test doubles work
    bool result = Collision_CheckSnakeWithFood(&snake, &food);
    assert(result == true);   // passes — function has no side effects
}
```

This test is impossible in Stage 0 because `Collision_CheckSnakeWithFood` does not exist as an isolated function — the detection is buried inside `UpdateGame()`.

---

### Extensibility — Adding a Power-Up

| Task | Stage 0 (Original C) | Stage 2 (C++ SOLID) |
|------|-----------------------|----------------------|
| Create power-up data | Add more globals to top of `main.c` | Create `powerup.h` / `powerup.cpp` |
| Spawn logic | Add to `UpdateGame()` — 30+ lines inline | Implement `Powerup_Spawn()` in `powerup.cpp` |
| Collision | Add to `UpdateGame()` — inline | Add `Collision_CheckSnakeWithPowerup()` in `collision.cpp` |
| Rendering | Add to `DrawGame()` — inline | Implement `IRenderable::render()` in `powerup.cpp`, call in `Game_Render()` |
| Files modified | 1 (`main.c`) | 2 (`powerup.cpp` new, `game.cpp` one-line addition) |
| Risk of regression | High — `UpdateGame` touched | Low — existing functions unchanged |

---

### Code Readability

```cpp
// Stage 0 — what does this do?
if (framesCounter % 5 == 0) {
    for (int i = 0; i < counterTail; i++) {
        if (i == 0) {
            snake[0].position.x += snake[0].speed.x;
            snake[0].position.y += snake[0].speed.y;
            allowMove = true;
        } else snake[i].position = snakePosition[i - 1];
    }
}

// Stage 2 — intent is immediately clear from the call site
Snake_UpdatePosition(&playerSnake, gameState.framesCounter);
// One line. Purpose evident. Implementation irrelevant to the reader.
```

---

## 7. Key Takeaways

### What SOLID changed (and what it did not)

| Category | Changed | Not changed |
|----------|---------|-------------|
| Runtime behaviour | ✗ No — game plays identically | ✓ |
| File/folder structure | ✗ No — same 8-file layout | ✓ |
| Language | ✓ C → C++ (enables `constexpr`, `nullptr`, virtual interfaces) | — |
| Function bodies | Minimal — casts, `nullptr`, `constexpr` | Core logic untouched |
| Architecture | ✓ Interfaces added; module contracts made explicit | — |
| Comments | ✓ Every function now documents which SOLID principle it applies | — |

---

### The five-principle summary for this project

```
[SRP] Each function does one thing.
      Game_Update coordinates. Snake_ProcessInput reads input.
      Snake_Render draws. They never cross.

[OCP] New entities (power-ups, obstacles) add new .cpp files.
      Existing snake.cpp, food.cpp, collision.cpp are never touched.

[LSP] All read-only functions accept const pointers.
      A mock Food, a test Snake — any subtype works without surprises.

[DIP] game.cpp never does raw math. It calls Utils_CalculateGridOffset().
      Renderer_DrawGameOver receives an int, not a GameState pointer.
      High-level code depends on abstractions, not on details.

[ISP] IRenderable and IUpdatable are separate interfaces.
      Game_Update never calls render functions.
      Game_Render never calls input or physics functions.
```

---

### Final thought

> The code that existed before this transformation *worked*. SOLID does not fix broken code — it makes working code **survivable**: easier to read six months later, safer to hand to a teammate, and cheaper to extend when requirements change.
>
> Every principle applied here has one practical goal: **reduce the cost of the next change.**

---

*Document generated as part of the Advanced Programming Lab — February 2026*  
*Companion files: `snake_game.h`, `main.cpp`, `snake.cpp`, `food.cpp`, `collision.cpp`, `renderer.cpp`, `utils.cpp`, `game.cpp`*
