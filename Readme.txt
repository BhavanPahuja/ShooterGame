# 🚀 Space Diamond Collector

A 2D arcade-style space game built with C++ and OpenGL where you pilot a spaceship to collect diamonds while growing larger with each successful collection!

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-5586A4?style=for-the-badge&logo=opengl&logoColor=white)
![GLFW](https://img.shields.io/badge/GLFW-0F1419?style=for-the-badge)

![ShooterGame](https://github.com/BhavanPahuja/ShooterGame/blob/main/project2.png)


project2.png


## 🎮 Gameplay

Control a spaceship in space and collect diamonds scattered across the screen. With each diamond collected, your ship grows larger, making navigation increasingly challenging. Collect all 4 diamonds to win!

![Gameplay Screenshot](screenshot.png)

## ✨ Features

- **Mouse-Controlled Rotation**: Click anywhere to rotate your ship toward that direction
- **Smooth Movement**: Use arrow keys (↑/↓) or W/S to move forward/backward
- **Progressive Difficulty**: Ship grows with each diamond collected (10%, 15%, 20%, 25%, 30%)
- **Collision Detection**: AABB (Axis-Aligned Bounding Box) collision system
- **Score Tracking**: Real-time score display using ImGui
- **Reset Functionality**: Press ESC to reset the game
- **Pixel Art Graphics**: Retro-style ship and diamond sprites
- **Win Condition**: Congratulations message upon collecting all diamonds

## 🛠️ Technology Stack

- **Language**: C++
- **Graphics**: OpenGL 3.3+
- **Window Management**: GLFW
- **Math Library**: GLM (OpenGL Mathematics)
- **Texture Loading**: Custom texture loader
- **UI**: Dear ImGui
- **Extension Loading**: GLEW

## 📋 Prerequisites

- **C++ Compiler** with C++11 support or higher (GCC, Clang, or MSVC)
- **OpenGL** 3.3 or higher
- **CMake** (optional, for building)

### Required Libraries
- GLFW3
- GLEW
- GLM
- ImGui (included in project)

## 🚀 Installation & Setup

### Clone the Repository
```bash
git clone https://github.com/BhavanPahuja/ShooterGame.git
cd ShooterGame
```

### Linux/macOS Setup

#### Install Dependencies (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install libglfw3-dev libglew-dev libglm-dev
```

#### Install Dependencies (macOS with Homebrew)
```bash
brew install glfw glew glm
```

#### Compile and Run
```bash
g++ -std=c++11 main.cpp -o shooter_game \
    -lGL -lGLEW -lglfw -lX11 -lpthread -ldl
./shooter_game
```

### Windows Setup (Visual Studio)

1. **Install vcpkg** (package manager):
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat
```

2. **Install Dependencies**:
```bash
vcpkg install glfw3 glew glm
vcpkg integrate install
```

3. **Open in Visual Studio**:
   - Create a new C++ project
   - Add all source files
   - Link libraries: `opengl32.lib`, `glew32.lib`, `glfw3.lib`
   - Build and run (F5)

### Using CMake (Cross-Platform)

```bash
mkdir build
cd build
cmake ..
make
./shooter_game
```

## 🎯 Controls

| Input | Action |
|-------|--------|
| **Mouse Click** | Rotate ship toward cursor position |
| **↑ / W** | Move ship forward |
| **↓ / S** | Move ship backward |
| **ESC** | Reset game |
| **R** | Reload shaders (debug mode) |

## 🎨 Game Mechanics

### Collision System
The game uses AABB (Axis-Aligned Bounding Box) collision detection:
```cpp
bool checkCollision(GameObject& ship, GameObject& diamond) {
    bool collisionX = ship.position.x + ship.scale.x >= diamond.position.x &&
                      diamond.position.x + diamond.scale.x >= ship.position.x;
    bool collisionY = ship.position.y + ship.scale.y >= diamond.position.y &&
                      diamond.position.y + diamond.scale.y >= ship.position.y;
    return collisionX && collisionY;
}
```

### Rotation System
Ship rotation uses vector mathematics to smoothly rotate toward the mouse cursor:
- Calculates angle between ship's direction and mouse position
- Uses dot product and cross product for accurate rotation
- Smooth interpolation for natural movement


## 📚 Learning Resources

This project demonstrates:
- **OpenGL Rendering Pipeline**: Vertex/Fragment shaders, VAOs, VBOs
- **2D Transformations**: Translation, rotation, scaling matrices
- **Game Loop Architecture**: Event handling, update, render
- **Collision Detection**: AABB collision system
- **Texture Mapping**: Loading and applying textures
- **User Interface**: ImGui integration

## 🎓 Academic Context

This project was developed as part of **CPSC 453** (Computer Graphics) coursework, demonstrating:
- OpenGL fundamentals
- 2D game development techniques
- Real-time rendering
- User interaction handling

## 📝 License

This project is available for educational purposes. Feel free to use and modify for learning.

## 👤 Author

**Bhavan Pahuja**
- GitHub: [@BhavanPahuja](https://github.com/BhavanPahuja)


⭐ **If you enjoyed this project, please give it a star!**

## 🎮 Quick Start

```bash
# Clone and run in 3 commands
git clone https://github.com/BhavanPahuja/ShooterGame.git
cd ShooterGame
# Follow compilation instructions above for your platform
```

**Objective**: Collect all 4 diamonds to win! Good luck, pilot! 🚀✨



