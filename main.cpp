
#include <SDL2/SDL.h>
#include <deque>
#include <cstdlib>
#include <ctime>

int main(int argc, char* argv[]) {

    const int CELL_SIZE = 20; // All grid math (movement, position, collision) is based on this unit, not raw pixels

    // SDL must be initialized before any other SDL function call will work
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Window size is 800x600 -> with CELL_SIZE 20, this gives a 40x30 grid
    SDL_Window* window = SDL_CreateWindow("Snake Emulator",
                                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                         800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Could not create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // -1 lets SDL pick the first available rendering driver automatically
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        SDL_Log("Could not create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Snake's position is tracked in grid coordinates, not pixels, so movement
    // is just +/-1 per tick; pixel values are only computed when drawing
    std::deque<SDL_Point> snakeBody;
    snakeBody.push_back({5, 5}); //starting head position

    // Direction is a delta applied to gridX/gridY each tick; (1,0) = moving right
    int dirX = 1;
    int dirY = 0;

    bool running = true;
    SDL_Event event;
   
    // Tracks when the snake last moved, so movement speed is decoupled from
    // the render loop's frame rate (otherwise the snake would move hundreds of times per second)
    Uint32 lastMoveTime = SDL_GetTicks();
    const int MOVE_DELAY = 200; /// ms between moves; lower = faster snake

    SDL_Point food = {15, 10}; // pick any starting grid position

    srand(time(nullptr)); // Initialize random seed

    while (running) {
        // Drain all pending events this frame; multiple can queue up between frames
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            // Arrow keys only change direction; actual movement is handled
            // separately below, gated by MOVE_DELAY
            if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        dirX = 0; dirY = -1;
                        break;
                    case SDLK_DOWN:
                        dirX = 0; dirY = 1;
                        break;
                    case SDLK_LEFT:
                        dirX = -1; dirY = 0;
                        break;
                    case SDLK_RIGHT:
                        dirX = 1; dirY = 0;
                        break;
                }
            }
        }

        // Runs every frame regardless of events, so the snake keeps moving
        // on its own even when no key is being pressed
        if(SDL_GetTicks() - lastMoveTime > MOVE_DELAY) {
            SDL_Point oldHead = snakeBody.front();
            SDL_Point newHead = {oldHead.x + dirX, oldHead.y + dirY};
           
            // Check for self-collision: if the new head position matches any segment of the snake's body, we end the game
            bool selfCollision = false;
            for(const SDL_Point& segment : snakeBody) {
                if(segment.x == newHead.x && segment.y == newHead.y) {
                    selfCollision = true;
                    break;
                }
            }
            
            // Check for wall collision: if the new head position is outside the grid boundaries, we end the game
            bool wallCollision = (newHead.x < 0 || newHead.x >= 40 || newHead.y < 0 || newHead.y >= 30);
           
            // If either collision occurs, we stop the game loop
            if(selfCollision || wallCollision) {
                running = false;
            }

            // If no collision, we add the new head to the front of the deque
            else{
                snakeBody.push_front(newHead);
                if(newHead.x == food.x && newHead.y == food.y) {
                    // If the snake eats the food, we generate a new food position
                    food.x = rand() % 40; // 800 / CELL_SIZE = 40
                    food.y = rand() % 30; // 600 / CELL_SIZE = 30
                } else {
                    // If the snake doesn't eat the food, we pop the tail to keep the length constant
                    snakeBody.pop_back();
                }
            }

            lastMoveTime = SDL_GetTicks();
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for the snake
        // Draw each segment of the snake
        for(const SDL_Point& segment : snakeBody) {
            SDL_Rect segmentRect = { segment.x * CELL_SIZE, segment.y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            SDL_RenderFillRect(renderer, &segmentRect);
        }

        // Draw the food
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for the food
        SDL_Rect foodRect = { food.x * CELL_SIZE, food.y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &foodRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}