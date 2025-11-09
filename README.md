# Ascent

**A Descent-inspired Game**

> Maurits Lam  
> 2025

A 3D first-person shooter with six degrees of freedom based on the Descent game series by Parallax Software. Built using the OGRE 3D rendering engine in modern C++.

## Features

- **Six degrees of freedom movement**  
  Navigate freely through 3D space in all directions.

- **Multiple weapon systems**  
  Switch between different cannons with unique firing patterns.

- **Enemy AI**  
  Face off against opponents in multi-level environments.

- **Level progression**  
  Explore multiple distinct levels.

## Controls

| Action         | Key         |
|----------------|-------------|
| Move Forward   | W           |
| Move Backward  | S           |
| Move Left      | A           |
| Move Right     | D           |
| Shoot          | Left Mouse  |
| Switch Cannons | Right Mouse |
| Toggle Map     | Tab         |
| Quick Save     | F5          |
| Quick Load     | F8          |
| Exit           | Esc         |

## User Guide

1. **Build the project:** Use the CMake presets to configure and build:
   - Debug: `Build Debug` task
   - Release: `Build Release` task

2. **Run the application:** Execute the generated binary from `bin/debug/` or `bin/release/`.

3. **Configuration:** OGRE configuration is managed through the configuration files in the bin directory.

## Implementation Overview

- **OGRE Integration:** Leverages OGRE for 3D rendering, scene management, and resource handling.

- **Entity System:** Game entities with modular components for enemies, player, and projectiles.

- **Map System:** Level-based world design with `GameWorld`, `LevelOne`, and `LevelTwo`.

- **Game State Management:** `GameState` and `Game` classes manage application flow and game logic.

- **UI Components:** Text rendering, notifications, and loading screens.

## Build Requirements

- CMake 3.15+
- C++17 compatible compiler (MSVC, GCC, or Clang)
- vcpkg
- OGRE 3D Engine (managed via vcpkg)

## Building

### Configure and Build (Debug)

```bash
cmake --preset msvc-x64-debug
cmake --build build/debug --config Debug
```

### Configure and Build (Release)

```bash
cmake --preset msvc-x64-release
cmake --build build/release --config Release
```

## License

This software is licensed under the [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.html). See [COPYING](COPYING) for details.
