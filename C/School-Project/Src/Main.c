#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>


typedef struct {
    int x, y;
} Vector2D;

typedef struct {
    Vector2D speed;
    SDL_Rect srcRect;
    SDL_Rect destRect;
    SDL_Texture * texture;
    int animFrm;
    Vector2D movement;
    Vector2D scroll;
    const char * name;
} GameObj;

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

SDL_Texture * LoadTexture(const char * fileName, SDL_Renderer * renderer) {
    SDL_Surface * tempSrf = IMG_Load(fileName);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, tempSrf);

    if (texture == NULL) {
        printf("Error Loading Texture\n");
    }

    SDL_FreeSurface(tempSrf);

    return texture;
}

void ResetObjRect(GameObj * obj, int width, int height) {
    obj -> srcRect.w = width;
    obj -> srcRect.h = height;

    obj -> srcRect.x = 0;
    obj -> srcRect.y = 0;

    obj -> destRect.w = obj -> srcRect.w;
    obj -> destRect.h = obj -> srcRect.h;

    obj -> destRect.x = 0;
    obj -> destRect.y = 0;
}

void InitObj(GameObj * obj, int charProps[4], const char * name) {
    if (obj -> name == name) {
        ResetObjRect(obj, charProps[2], charProps[3]);
        
        obj -> speed.x = charProps[0];
        obj -> speed.y = charProps[1];

        obj -> scroll.x = 0;
        obj -> scroll.y = 0;
    }
}

void CalcScroll(GameObj * obj) {
    obj -> scroll.x += (int) (obj -> destRect.x - obj -> scroll.x - 290);
    obj -> scroll.y += (int) (obj -> destRect.y - obj -> scroll.y - 210);
}

void DrawRect(GameObj * obj, SDL_Renderer * renderer, int r) {
    SDL_SetRenderDrawColor(renderer, r, 0, 255, 10);
    SDL_RenderDrawRect(renderer, &obj -> destRect);
    SDL_RenderFillRect(renderer, &obj -> destRect);
}

void DrawObj(GameObj * obj, SDL_Renderer * renderer, Vector2D * scroll) {
    SDL_Rect n_rect = obj -> destRect;

    n_rect.w += 2;
    n_rect.h += 2;

    if (obj -> texture != NULL && scroll == NULL) {
        SDL_RenderCopy(renderer, obj -> texture, &obj -> srcRect, &n_rect);
    }

    if (obj -> texture != NULL && scroll != NULL) {
        n_rect.x = n_rect.x - scroll -> x;
        n_rect.y = n_rect.y - scroll -> y;

        SDL_RenderCopy(renderer, obj -> texture, &obj -> srcRect, &n_rect);
    }
}

void ResetObjMovementVec(GameObj * obj) {
    obj -> movement.x = 0;
    obj -> movement.y = 0;
}

int ColDetect(GameObj * a, GameObj * b) {
    if (a -> destRect.x + a -> destRect.w >= b -> destRect.x &&
	b -> destRect.x + b -> destRect.w >= a -> destRect.x &&
	a -> destRect.y + a -> destRect.h >= b -> destRect.y &&
	b -> destRect.y + b -> destRect.h >= a -> destRect.y) {
	    return 1;
    }

    return 0;
}

# ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
# endif

uint32_t flags;

SDL_Window * window;
SDL_Renderer * renderer;

GameObj player;
GameObj ledges[100], ledges2[100];

const uint8_t * keyState;
SDL_Event event;

int gameRunning;

int Fabien[4] = { 13, 6, 32, 32 };

SDL_Rect playerRect;

#define FPS 60

const int pfDur = 30;
const int maxFr = 1;

int frameTime;

SDL_Texture * frames[2];
SDL_Texture * brick;

int applyGravity;
int jumped;
int timer;
int looped;
int yMomentum;
int gravity;
int timerStart;
int jumpCount;

int Init() {
    InitSDL();

    window = CreateWindow("Game", 640, 480, &flags);
    renderer = CreateRenderer(window);

    frames[0] = LoadTexture("Resources/Images/Player1.png", renderer);
    frames[1] = LoadTexture("Resources/Images/Player2.png", renderer);

    brick = LoadTexture("Resources/Images/tile.png", renderer);

    player.animFrm = 0;
    player.name = "Fabien";

    InitObj(&player, Fabien, "Fabien");

    player.destRect.x = 10;
    player.destRect.y = 100;

    for (int i = 0; i < 98; i++) {
        ledges[i].texture = brick;
        
        ledges[i].srcRect.w = 32;
        ledges[i].srcRect.h = 32;

        ledges[i].destRect.w = 32;
        ledges[i].destRect.h = 32;

        ledges[i].destRect.x = 32 * i;
        ledges[i].destRect.y = 400;
    }

    ledges[99].texture = brick;

    ledges[99].srcRect.w = 32;
    ledges[99].srcRect.h = 32;

    ledges[99].destRect.w = 32;
    ledges[99].destRect.h = 32;

    ledges[99].destRect.x = 128;
    ledges[99].destRect.y = 370;

    for (int i = 0; i < 100; i++) {
        ledges2[i].texture = brick;
        
        ledges2[i].srcRect.w = 32;
        ledges2[i].srcRect.h = 32;

        ledges2[i].destRect.w = 32;
        ledges2[i].destRect.h = 32;

        ledges2[i].destRect.x = 32 * i;
        ledges2[i].destRect.y = 400;
    }

    gameRunning = 1;
    jumped = 0;
    jumpCount = 0;
    timerStart = 0;
    timer = 25;
    applyGravity = 0;
    looped = 0;
    yMomentum = 0;
    gravity = 0;
    
    return 1;

}

void GameLoop() {
    if (looped) {
        if (keyState[SDL_SCANCODE_SPACE] && player.movement.y >= 0 && !jumped && jumpCount <= 2) {
            yMomentum -= 50;

            jumped = 1;
            jumpCount += 1;
        
            timerStart = 1;
            applyGravity = 0;
        }

        looped = 0;
    }

    ResetObjMovementVec(&player);
    looped = 1;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            gameRunning = 0;
            break;
    }

    keyState = SDL_GetKeyboardState(NULL);

    if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]) {
        player.movement.x -= player.speed.x;
    }

    if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]) {
        player.movement.x += player.speed.x;
    }

    frameTime++;

    if (frameTime >= pfDur) {
        player.animFrm++;
        frameTime = 0;
    }

    if (player.animFrm > maxFr) {
        player.animFrm = 0;
    }

    player.texture = frames[player.animFrm];

    player.destRect.x += player.movement.x;
    for (int i = 0; i < 100; i++) {
        if (ColDetect(&player, &ledges[i]) || ColDetect(&player, &ledges2[i])) {
        
            if (player.movement.x < 0) {
                player.destRect.x += player.speed.x;
            }

            if (player.movement.x > 0) {
                player.destRect.x -= player.speed.x;
            }
        }

    }

    if (yMomentum < 0) {
        player.movement.y += yMomentum;
        yMomentum = 0;
    }

    if (applyGravity) {
        gravity += 1;
        player.movement.y += gravity;

        if (gravity > 5) {
            gravity = 5;
        }
    }

    if (timerStart && timer > -1) {
        timer -= 1;
    }

    if (timerStart && timer <= 0) {
        jumped = 0;
        timer = 25;
        timerStart = 0;
    }

    player.destRect.y += player.movement.y;
    for (int i = 0; i < 100; i++) {
        if (ColDetect(&player, &ledges[i]) || ColDetect(&player, &ledges2[i])) {

            if (player.movement.y < 0) {
                player.destRect.y += 10;
                applyGravity = 1;
            }

            if (player.movement.y > 0) {
                int m = gravity + 1;
                player.destRect.y -= m;

                if (!jumped) {
                    jumpCount = 0;
                }

                applyGravity = 0;
                gravity = 0;
            }
	    }

        if ((!ColDetect(&player, &ledges[i]) && !ColDetect(&player, &ledges2[i])) || yMomentum < 0) {
            applyGravity = 1;
	    }
    }

    CalcScroll(&player);

    SDL_RenderClear(renderer);

        for (int i = 0; i < 100; i++) {
            DrawObj(&ledges[i], renderer, &player.scroll);
            DrawObj(&ledges2[i], renderer, &player.scroll);
        }

        DrawObj(&player, renderer, &player.scroll);

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

