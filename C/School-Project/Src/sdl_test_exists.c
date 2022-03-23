#include <SDL2/SDL.h>
#include <stdio.h>

void InitSDL() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error Initializing SDL: %s\n", SDL_GetError());
    }

    printf("Successfully Initialized SDL\n");
}

uint32_t HandleResizable(int resizable) {
    uint32_t flags = 0;

    if (resizable) {
	# ifndef __APPLE__
	    flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
	# else 
	    flags = SDL_WINDOW_RESIZABLE;
	# endif
    }
    
    return flags;
}

SDL_Window * CreateWindow(const char * title, int width, int height, uint32_t * flags) {
    SDL_Window * window = SDL_CreateWindow(title, 240, 120, width, height, *flags);
    if (!window) {
        printf("Error Creating Window: %s\n", SDL_GetError());
    }

    return window;
}

SDL_Renderer * CreateRenderer(SDL_Window * window) {
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Error Creating Renderer: %s\n", SDL_GetError());
    }

    return renderer;
}

# ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
# endif

uint32_t flags;

SDL_Window * window;
SDL_Renderer * renderer;

SDL_Event event;

int gameRunning;

#define FPS 60

const int pfDur = 30;
const int maxFr = 1;

int frameTime;


int Init() {
    InitSDL();

    window = CreateWindow("Game", 640, 480, &flags);
    renderer = CreateRenderer(window);
   
    return 1;

}

void GameLoop() {
    
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            gameRunning = 0;
            break;
    }

    SDL_SetRenderDrawColor(renderer, 245, 100, 0, 255);
    SDL_RenderClear(renderer);


    SDL_RenderPresent(renderer);
}

int main(void) {
    int load = Init();

    if (load != 1) {
        printf("Error Initializing Game!\n");
    }

    printf("Successfully Initialized Game\n");

    # ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(GameLoop, 0, 1);
    # else 
        while (gameRunning) {
            GameLoop();
            SDL_Delay(16);
        }
    # endif

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;

}







