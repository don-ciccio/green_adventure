# 3D Third-Person Adventure Game

This is a 3D third-person adventure game built with C and the Raylib library. The game features a character with animated accessories, dynamic lighting, and a procedurally generated forest environment.

## How to Build and Run

### Prerequisites

-   A C compiler (like GCC or Clang)
-   The Raylib library installed on your system

### Building

To build the game, simply run the `make` command in the root directory of the project:

```bash
make build_osx
```

This will compile the game and create an executable file in the `bin` directory.

### Running

Once the game is built, you can run it from the root directory with the following command:

```bash
./bin/build_osx
```

## Features

### Player System

-   **3D Character Model:** Greenman character with realistic animations
-   **Equipment System:** Hat, sword, and shield accessories that attach to character bones
-   **Smooth Animations:** Idle, running, and attack animations with seamless transitions
-   **Collision Detection:** Player-enemy collision system with visual feedback

### Camera System

-   **Third-Person Camera:** Smooth camera that follows the player
-   **Mouse Look:** Free-look camera system with mouse sensitivity
-   **Zoom Control:** Mouse wheel zooming with distance limits
-   **Dynamic Height:** Camera automatically adjusts height when zoomed

### Lighting System

-   **Golden Hour Lighting:** Warm, atmospheric lighting with golden tones
-   **Multiple Light Sources:** Ambient, directional, and point lights
-   **Dynamic Lighting:** Real-time lighting calculations with custom shaders
-   **Light Controls:** Toggle individual lights during gameplay

### Environment

-   **Procedural Forest:** Randomly generated tree patches with variety
-   **Custom Textures:** Cold-themed tree textures for atmospheric effect
-   **Scene Management:** Efficient 3D scene rendering system

### Enemy System

-   **AI Enemies:** Multiple enemies with health and collision detection
-   **Combat Feedback:** Visual collision responses and damage system

## Controls

### Movement

-   **WASD:** Move character (camera-relative movement)
-   **Mouse:** Look around (camera control)
-   **Mouse Wheel:** Zoom in/out

### Actions

-   **Spacebar:** Attack animation
-   **C:** Cycle through animations (debug)

### Equipment

-   **1:** Toggle hat visibility
-   **2:** Toggle sword visibility
-   **3:** Toggle shield visibility

### Lighting

-   **Y:** Toggle first light
-   **R:** Toggle second light
-   **G:** Toggle third light

### System

-   **ESC:** Pause/unpause game

## Assets

The game includes several 3D models and textures:

-   `greenman.glb` - Main character model with animations
-   `greenman_hat.glb` - Hat accessory
-   `greenman_sword.glb` - Sword accessory
-   `greenman_shield.glb` - Shield accessory
-   `LowPolyTreePack.glb` - Forest environment models
-   `Colorsheet Tree Cold.png` - Tree texture
-   Custom lighting shaders in `assets/shaders/`

## Technical Features

-   **Bone Socket System:** Advanced character rigging for equipment attachment
-   **Custom Shaders:** GLSL shaders for realistic lighting effects
-   **Scene Graph:** Hierarchical scene management for complex environments
-   **Animation Blending:** Smooth transitions between character animations
-   **Bounding Box Collision:** Efficient 3D collision detection system
