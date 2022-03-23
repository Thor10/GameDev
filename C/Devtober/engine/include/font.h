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

    #define _mk_le_ui_label_t(_x, _y, text, font, _r, _g, _b, _a, size, renderer) \
        mk_label(_x, _y, text, font, _r, _g, _b, _a, size, renderer)

    le_ui_label_t mk_label(i32 x, i32 y, le_str text, le_str font, i32 r, i32 g, i32 b, i32 a, i32 size,
                            SDL_Renderer * renderer); 


#endif

