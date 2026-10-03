# Snake Game (C++ / SDL2)
A classic Snake game built in C++ using SDL2, developed as a portfolio project to practice low-level programming concepts: manual memory/resource management, real-time game loops, and grid-based collision logic.

## Features

- Smooth grid-based movement with keyboard controls (arrow keys)
- Growing snake body implemented with a `std::deque` for efficient front/back operations
- Randomized food spawning using a seeded random number generator
- Collision detection for both self-collision and wall boundaries, ending the game on either
- Input safeguard preventing the snake from instantly reversing into itself
- Timing-based movement decoupled from frame rate, so game speed is consistent regardless of hardware

## Setup

This project requires a C++ compiler (MinGW-w64/MSYS2 recommended on Windows) and SDL2.

1. Install SDL2 for your toolchain. On MSYS2 (UCRT64):
pacman -S mingw-w64-ucrt-x86_64-SDL2

2. Clone the repo:
git clone https://github.com/lalexander16/snake-game.git
cd snake-game

3. Compile:
g++ main.cpp -o snake.exe -IC:/msys64/ucrt64/include/SDL2 -Dmain=SDL_main -LC:/msys64/ucrt64/lib -lmingw32 -mwindows -lSDL2main -lSDL2

4. Run:
./snake.exe


## Controls

- Arrow keys: change direction
- Closing the window ends the game

## Technical Notes

- **Grid-based design**: the snake's position is tracked in grid coordinates (not raw pixels), converting to pixel coordinates only at render time — this keeps movement and collision logic simple (always ±1 per move).
- **Body representation**: the snake's body is a `std::deque<SDL_Point>`, allowing efficient addition at the front (new head) and removal from the back (old tail) on every move.
- **Timing**: movement speed is controlled independently of the render loop's frame rate using `SDL_GetTicks()`, so the game runs at a consistent pace rather than moving as fast as the hardware allows.
- **Collision handling**: both wall and self-collision are checked *before* committing a move, so invalid moves never corrupt game state.

## Planned Features

- On-screen score display (likely via SDL_ttf for text rendering)
- Restart option instead of closing on game over
- Increasing speed as the snake grows
- Edge-case refinement: currently, moving into the tail's *current* position counts as a collision even though that cell will be vacated by the time the head arrives — a minor deviation from strict classic Snake rules