#include <img.h>
#include <util.h>
#include <types.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>


SDL_Texture * le_tex(const le_str file, SDL_Renderer * renderer) {
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, IMG_Load(file));
    if (texture == NULL) { le_print_error("Error Loading Texture: %s", file); }
    return texture;
}


