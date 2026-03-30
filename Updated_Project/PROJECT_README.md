# 🐍 Snake Game - Refactored Edition

**Course:** Advanced Programming Lab  
**Language:** C  
**Graphics Library:** Raylib  
**Purpose:** Demonstrate professional code refactoring and software engineering standards

---

## 📋 Project Overview

This is a fully refactored version of the classic Snake game, implementing modern software engineering practices including:

- ✅ **Modular Architecture** - Separation of concerns across multiple modules
- ✅ **Professional Naming Conventions** - Clear, descriptive identifiers
- ✅ **Comprehensive Documentation** - Detailed comments and documentation
- ✅ **Clean Code Structure** - Well-organized folder hierarchy
- ✅ **Error Handling** - Assertions and safe programming practices
- ✅ **Scalable Design** - Easy to extend with new features

---

## 🗂️ Project Structure

```
snake-game-refactored/
├── main.c              # Main entry point
├── game.c              # Core game logic and state management
├── snake.c             # Snake entity management
├── food.c              # Food spawning and management
├── collision.c         # Collision detection module
├── renderer.c          # Rendering and UI display
├── utils.c             # Utility functions
├── snake_game.h        # Main header file with all declarations
├── Makefile            # Build configuration
└── README.md           # This file
```

---

## 🎮 Game Features

- Classic snake gameplay with wrap-around screen edges
- Smooth keyboard controls (Arrow keys)
- Score tracking
- Pause functionality (Press 'P')
- Game over screen with restart option (Press ENTER)
- Visual freeze effect on collision before game over

---

## 🔧 Building the Game

### Prerequisites

- GCC compiler (or compatible C compiler)
- Raylib library installed
- Make (optional, for using Makefile)

### Installation Steps

#### Linux/macOS

1. Install Raylib:
```bash
# Ubuntu/Debian
sudo apt install libraylib-dev

# macOS (using Homebrew)
brew install raylib
```

2. Build the game:
```bash
make
```

3. Run the game:
```bash
make run
```

#### Windows

1. Install Raylib (follow official Raylib installation guide)
2. Build using MinGW:
```bash
make
```

### Manual Compilation

If you prefer not to use the Makefile:

```bash
gcc main.c game.c snake.c food.c collision.c renderer.c utils.c -o snake_game -lraylib -lm -lpthread -ldl
```

---

## 🎯 How to Play

- **Arrow Keys** - Control snake direction
- **P** - Pause/Unpause game
- **ENTER** - Restart game after game over

**Objective:** Eat the yellow food to grow your snake and increase your score. Avoid running into yourself!

---

## 📚 Code Architecture

### Module Responsibilities

#### **game.c** - Core Game Logic
- Game state management
- Main game loop coordination
- Event orchestration between modules

#### **snake.c** - Snake Entity
- Snake movement and positioning
- Input processing
- Self-collision detection
- Growth mechanics

#### **food.c** - Food Management
- Random food spawning
- Collision with snake
- Ensures food doesn't spawn on snake

#### **collision.c** - Collision Detection
- Centralized collision logic
- Snake-food collision
- Snake-self collision

#### **renderer.c** - Visual Output
- Grid rendering
- Entity rendering coordination
- UI overlays (pause, game over)

#### **utils.c** - Helper Functions
- Grid calculations
- Position validation
- Common mathematical operations

---

## 🔍 Refactoring Highlights

### Before Refactoring Issues:
- ❌ Single file with 300+ lines
- ❌ Global variables scattered throughout
- ❌ Cryptic variable names (x, y, scr, snkLen)
- ❌ Mixed responsibilities in functions
- ❌ Difficult to test or extend

### After Refactoring Improvements:
- ✅ Modular structure (7 files + 1 header)
- ✅ Encapsulated state in structures
- ✅ Descriptive names (playerScore, snakeLength)
- ✅ Single Responsibility Principle
- ✅ Easy to test and extend

---

## 🚀 Future Enhancements

Potential features to add:

- [ ] Multiple difficulty levels
- [ ] Obstacles and power-ups
- [ ] High score persistence (save/load)
- [ ] Sound effects
- [ ] Multiple game modes
- [ ] AI-controlled snake opponent
- [ ] Unit testing framework
- [ ] Configuration file support

---

## 📖 Learning Outcomes

This refactored code demonstrates:

1. **Separation of Concerns** - Each module has a clear, focused purpose
2. **Naming Conventions** - Consistent, descriptive naming throughout
3. **Documentation** - Every function includes purpose and parameter documentation
4. **Error Handling** - Assertions validate assumptions and prevent bugs
5. **Maintainability** - Code is easy to read, understand, and modify
6. **Scalability** - New features can be added without major restructuring

---

## 📝 Documentation Standards

All functions follow this documentation pattern:

```c
/*
 * Brief description of what the function does
 * 
 * @param paramName - Description of parameter
 * @return Description of return value
 */
```

---

## 🛠️ Development Guidelines

When extending this code:

1. **Follow naming conventions**: Use descriptive names with module prefixes
2. **Maintain separation**: Keep modules focused on single responsibilities
3. **Document thoroughly**: Add comments explaining the "why", not just the "what"
4. **Use assertions**: Validate assumptions in debug builds
5. **Test incrementally**: Verify functionality after each change

---

## 📄 License

Educational project for Advanced Programming Lab course.

---

## 👨‍💻 Author

Refactored by: [Your Name]  
Original Game: Classic Snake  
Course: Advanced Programming Lab  
Date: February 2026

---

## 🙏 Acknowledgments

- **Raylib** - For the excellent graphics library
- **Original Snake** - For the timeless gameplay concept
- **Course Instructors** - For teaching software engineering principles

---

**Happy Coding! 🎮🐍**
