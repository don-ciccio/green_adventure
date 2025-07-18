# FPS Weapon Viewer

This is a 3D first-person shooter weapon viewer built with C and the Raylib library. The game features realistic weapon viewmodels with smooth animations for spawning and reloading.

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

-   **3D Weapon Viewmodels:** Realistic AK47 weapon model with proper positioning and scaling
-   **Smooth Animations:** Weapon spawn and reload animations with easing functions
-   **First-Person Camera:** Free-look camera system for immersive experience
-   **Weapon System:** Complete weapon structure with ammo management and shooting mechanics
-   **Raytracing:** Ray-based shooting system with collision detection

## Controls

-   **Mouse:** Look around (first-person camera)
-   **M Key:** Spawn weapon animation
-   **R Key:** Reload weapon animation
-   **WASD:** Move camera (first-person movement)

## Assets

The weapon model (ak47.glb) is included in the assets directory. Additional models can be added by placing them in the `assets/` folder and updating the model loading code.
