#pragma once

#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include "../Includes/Engine.h"

typedef struct {
    SDL_Texture * tex;
    SDL_Rect srcR;
} image2d;

void init_image2d(image2d * img, const char * path, int w, int h, int x, int y) {
    img -> tex = le_load_texture(path);
    img -> srcR.x = x;
    img -> srcR.y = y;
    img -> srcR.w = w;
    img -> srcR.h = h;
}

