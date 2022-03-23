#ifndef _light_engine_img
    
    #define _light_engine_img
    #include "types.h"
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    
    typedef struct {
        SDL_Texture * tex; // Texture
        SDL_Rect      src; // Source Rect
        int         alpha; // Alpha Value
    } le_img_t;

    SDL_Texture * le_tex(const le_str file, SDL_Renderer * renderer);
    le_img_t le_create_img_t(i32 r, i32 g, i32 b, i32 a, i32 w, i32 h);
    #define _le_img_t(path, _x, _y, _w, _h, renderer) \
        (le_img_t)  { .tex = le_tex(path, renderer), \
                      .src.x = _x, \
                      .src.y = _y, \
                      .src.w = _w, \
                      .src.h = _h   } 
    #define le_clip_img_t(path, _x, _y, _w, _h, img) \
        (le_img_t)  { .tex = img.tex, \
                      .src.x = _x, \
                      .src.y = _y, \
                      .src.w = _w, \
                      .src.h = _h   } 
    
    #define le_set_img_t_colorkey(img, val) \
        SDL_SetColorKey(img.tex, SDL_TRUE, val);

    #define le_map_rgba(surf_format, r, g, b, a) \
        SDL_MapRGBA(surf_format, r, g, b, a);

    #define le_set_img_t_alpha(img, val) \
        img -> alpha = val;         \
        SDL_SetTextureAlphaMod(img -> tex, val);
    
#endif
