# ifdef __EMSCRIPTEN__ 
    #include <emscripten.h>
    #include <emscripten/html5.h>
# endif


#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../Structs/Entity.h"
#include "../Includes/Vector.h"
#include "../Includes/Log.h"
#include "../Includes/Engine.h"


DynamicEnt e1;
DynamicEnt enemies[1000];
DynamicEnt stars[10000];

Vector2D scroll;

int movingRight = 0;
int movingLeft  = 0;
int movingUp    = 0;
int movingDown  = 0;

int g_time = 0;


TTF_Font* Sans; 
SDL_Color White;  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
SDL_Surface* surfaceMessage; 
SDL_Surface* _surfaceMessage; 
SDL_Surface * timeMsg;
SDL_Texture* Message;
SDL_Texture* _Message;
SDL_Texture * timeSS;
SDL_Rect Message_rect; //create a rect
SDL_Rect _Message_rect; //create a rect

SDL_Rect _Mesage_rect; //create a rect

SDL_Rect _time_rect;
char time_[10];

void DrawEntity(DynamicEnt * e, int r) {
    SDL_Rect entity;
    
    entity.w = e -> w;
    entity.h = e -> h;

    entity.x = e -> pos.x;
    entity.y = e -> pos.y;

    DrawRect(&entity, renderer, r);
}

int Init() { 
    InitSDL();
    CreateWindow("Free Palestine Game Jam", 640, 480, &flags);
    CreateRenderer();
    InitDynamicEnt(&e1, 100, 10, 20, 20, 10, 15);

    for (int i = 0; i < 500; i++) {
	InitDynamicEnt(&stars[i], rand() % 1000, rand() % 800, 1, 1, 0, 0);
    }
    
    for (int i = 0; i < 100; i++) {
	InitDynamicEnt(&enemies[i], rand() % 1000, rand() % 800, 15, 15, 3, 2);
    }

    TTF_Init();

    scroll.x = 0;
    scroll.y = 0;


    Sans = TTF_OpenFont("Resources/Sans.ttf", 100); //this opens a font style and sets a size
    White.r = 255;
    White.g = 255; 
    White.b = 255;  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

    surfaceMessage = TTF_RenderText_Solid(Sans, "Imagine these stars in the background are your rights... and they get taken away from you slowly over time, how would you feel?", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

    _surfaceMessage = TTF_RenderText_Solid(Sans, "This is what is happening in Palestine as of now                       This game was made as a project to raise awareness of the situation in Palestine", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
    Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture
    _Message = SDL_CreateTextureFromSurface(renderer, _surfaceMessage); //now you can convert it into a texture

    Message_rect.x = 320;  //controls the rect's x coordinate 
    Message_rect.y = 260; // controls the rect's y coordinte
    Message_rect.w = 3500; // controls the width of the rect
    Message_rect.h = 100; // controls the height of the rect

    _Message = SDL_CreateTextureFromSurface(renderer, _surfaceMessage); //now you can convert it into a texture

    _Message_rect.x = 320;  //controls the rect's x coordinate 
    _Message_rect.y = 260; // controls the rect's y coordinte
    _Message_rect.w = 4000; // controls the width of the rect
    _Message_rect.h = 100; // controls the height of the rect
    
    _time_rect.x = 120;
    _time_rect.y = 120;
    _time_rect.w = 100;
    _time_rect.h = 50;

    _Mesage_rect.x = 50;
    _Mesage_rect.y = 240;
    _Mesage_rect.w = 500;
    _Mesage_rect.h = 100;

    return 1;
}


void GameLoop() {
    HandleEvents();

    scroll.x += (int) (e1.pos.x - scroll.x - 290);
    scroll.y += (int) (e1.pos.y - scroll.y - 210);

    switch (event.type) {
	HANDLEQ;
	case SDL_KEYDOWN:
	    switch (event.key.keysym.scancode) {
		case SDL_SCANCODE_RIGHT:
		    movingRight = 1;
		    break;

		case SDL_SCANCODE_LEFT:
		    movingLeft = 1;
		    break;

		case SDL_SCANCODE_DOWN:
		    movingDown = 1;
		    break;

		case SDL_SCANCODE_UP:
		    movingUp = 1;
		    break;

		default:
		    break;
	    }

	    break;

	case SDL_KEYUP:
	    switch (event.key.keysym.scancode) {
		case SDL_SCANCODE_RIGHT:
		    movingRight = 0;
		    break;

		case SDL_SCANCODE_LEFT:
		    movingLeft = 0;
		    break;

		case SDL_SCANCODE_DOWN:
		    movingDown = 0;
		    break;

		case SDL_SCANCODE_UP:
		    movingUp = 0;
		    break;

		case SDL_SCANCODE_SPACE:
		    if (g_time == -1) g_time = 0;

		default:
		    break;
	    }

	    break;

	default:
	    break;
    }

    if (movingRight == 1) {
	MoveDynamicEntForwardX(&e1);
    }

    if (movingLeft == 1) {
	MoveDynamicEntBackwardX(&e1);
    }

    if (movingUp == 1) {
	MoveDynamicEntUpY(&e1);
    }

    if (movingDown == 1) {
	MoveDynamicEntDownY(&e1);
    }

    if (e1.pos.x > 1000) {
	e1.pos.x = 0;
    }

    if (e1.pos.x < 0) {
	e1.pos.x = 1000;
    }

    if (e1.pos.y > 800) {
	e1.pos.y = 0;
    }

    if (e1.pos.y < 0) {
	e1.pos.y = 800;
    }


    /* Update */
    UpdateDynamicEnt(&e1);


    /* Render */
    ClearScreen(0, 0, 0, 255);
    StartRender();
   	//Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes

	if (g_time < 4000 && g_time != -1) {
	    for (int i = 0; i < 10000; i++) {
		DynamicEnt star;
		star.pos.x = stars[i].pos.x - scroll.x;
		star.pos.y = stars[i].pos.y - scroll.y;
		DrawEntity(&star, 230);

	    }
	}

	if (g_time > 898 && g_time < 4300) {
	    SDL_RenderCopy(renderer, Message, NULL, &Message_rect); 
	    if (g_time < 4125) Message_rect.x -= 1;
	}

	if (g_time > 4300 && g_time < 8250) {
	    SDL_RenderCopy(renderer, _Message, NULL, &_Message_rect); 
	    if (g_time < 8000) _Message_rect.x -= 1;
	}

	if (g_time > 8300) {
	    for (int i = 0; i < 10000; i++) {
		DynamicEnt star;
		star.pos.x = stars[i].pos.x - scroll.x;
		star.pos.y = stars[i].pos.y - scroll.y;
		DrawEntity(&star, 230);

	    }

	    for (int i = 0; i < 1000; i++) {
		DynamicEnt enemy = enemies[i]; 
		enemy.pos.x = enemies[i].pos.x - scroll.x;
		enemy.pos.y = enemies[i].pos.y - scroll.y;
		DrawEntity(&enemy, 175);

		if (CollisionDetect(e1, enemies[i])) {
		    g_time = -1; 
		}
	    }

	}
	
	if (g_time == -1) {	    
	    timeMsg = TTF_RenderText_Solid(Sans, "Game Over!", White); 
	    timeSS = SDL_CreateTextureFromSurface(renderer, timeMsg);

	    SDL_RenderCopy(renderer, timeSS, NULL, &_Mesage_rect);
    	}
    

	if (g_time != -1)	g_time += 1;
	DynamicEnt p = e1;
	p.pos.x = e1.pos.x - scroll.x;
	p.pos.y = e1.pos.y - scroll.y;
	if (g_time != -1)	DrawEntity(&p, 23);



    EndRender();
}


int main(SDL_MAIN_ARGS) {
    if (!Init()) {
	LogError("Error Initializing Game!", NULL);
    }

    LogInfo("Successfully Initialized Game", NULL);

    # ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(GameLoop, 0, 1);
    # else 
	while (gameRunning) {
	    GameLoop();
	    Delay(16);
	}
    # endif

    UninitSDL();

    return 0;
}


