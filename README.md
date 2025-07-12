# Racer

This is a simple 2D racing game built with C and the Raylib library. The game features a top-down view of a car that you can drive around an infinite, procedurally generated world.

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

-   **Infinite World:** The game world is procedurally generated and infinite, so you can drive forever!
-   **Drifting Mechanics:** The car has a simple drifting mechanic that allows you to slide around corners.
-   **Skidmarks:** When you drift, the car leaves skidmarks on the ground. These are implemented with a particle system, so they fade away over time.

## Controls

-   **Up Arrow:** Accelerate
-   **Down Arrow:** Brake/Reverse
-   **Left Arrow:** Steer Left
-   **Right Arrow:** Steer Right

## Assets

The graphical assets used in this game are from the [Free Racing Game Kit](https://craftpix.net/freebies/free-racing-game-kit/) by CraftPix.
