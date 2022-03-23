#include <img.h>
#include <util.h>
#include <types.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <engine.h>

SDL_Texture * le_tex(const le_str file, SDL_Renderer * renderer) {
    SDL_Surface * surf = IMG_Load(file);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surf);
    if (texture == NULL) { le_print_error("Error Loading Texture: %s", file); }
    free(surf);
    return texture;
}


le_img_t le_create_img_t(i32 r, i32 g, i32 b, i32 a, i32 w, i32 h) {
    SDL_Surface * surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    SDL_LockSurface(surface);

    u08 * px = (u08 *) surface -> pixels;

    for (i32 y = 0; y < h; y++) {
        for (i32 x = 0; x < w; x++) {
            px[y * surface -> pitch + x * surface -> format -> BytesPerPixel + 2] = r;
            px[y * surface -> pitch + x * surface -> format -> BytesPerPixel + 0] = g;
            px[y * surface -> pitch + x * surface -> format -> BytesPerPixel + 1] = b;             
        }
    }
    
    SDL_UnlockSurface(surface);

    if (surface == NULL) {
        le_print_warning("Failed to make surface: %s", SDL_GetError());
    }
    
    le_img_t img; 

    img.tex = SDL_CreateTextureFromSurface(le_get_renderer(), surface);

    if (img.tex == NULL) {
        le_print_warning("Failed to make texture: %s", SDL_GetError());
    }

    img.src.x = 0;
    img.src.y = 0;
    img.src.w = w;
    img.src.h = h;

    img.alpha = a;         
    SDL_SetTextureBlendMode(img.tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(img.tex, a);
    

    return img;

}


