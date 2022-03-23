#ifndef _light_engine_font

    #define _light_engine_font
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>    
    #include "types.h"

    #define le_load_font(path, size) \
        TTF_OpenFont(path, size)        
    
    typedef struct {
        SDL_Rect      pos;         // Position
        le_str        label_text;  // Text
        TTF_Font    * label_font;  // Font
        SDL_Color     text_color;  // Color
        SDL_Texture * label_tex;   // Texture
    } le_ui_label_t;

    #define mk_le_ui_label_t(_x, _y, text, font, _r, _g, _b, _a, size) \
        (le_ui_label_t) { .pos.x = _x, .pos.y = _y, .pos.w = 0, .pos.h = 0, .label_text = text, .label_font = le_load_font(font, size), .text_color.r = _r, .text_color.g = _g, .text_color.b = _b, .text_color.a = _a, .label_tex = NULL  \
        }; 

    #define _create_le_ui_label_t_text(label, renderer) \
        label -> label_tex = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(label -> label_font, label -> label_text, label -> text_color)); \
        SDL_QueryTexture(label -> label_tex, NULL, NULL, &label -> pos.w, &label -> pos.h);


#endif

