#ifndef _light_engine_draw
    
    #define _light_engine_draw
    #include <SDL2/SDL.h>
    #include "img.h"
    #include "entity.h"
    #include "types.h"
    #include "font.h"

    emp _le_draw_rect(SDL_Rect * obj, i16 r, i16 g, i16 b, i16 a, SDL_Renderer * renderer);
    emp _le_draw_le_entity_t(le_entity_t * entity, le_img_t * image, SDL_Renderer * renderer);
    emp _le_draw_le_entity_t_rect(le_entity_t * entity, i16 r, i16 g, i16 b, i16 a, SDL_Renderer * renderer); 
    emp _le_draw_le_img_t(le_img_t * image, i32 x, i32 y, SDL_Renderer * renderer);
    emp _le_draw_le_ui_label_t(le_ui_label_t * label, SDL_Renderer * renderer); 
    emp _le_draw_le_entity_t_ex(le_entity_t * entity, le_img_t * img, SDL_Renderer * renderer, f64 angle, SDL_Point * center, SDL_RendererFlip flip);
    emp _le_centered_draw_le_entity_t(le_entity_t * entity, le_img_t * image, SDL_Renderer * renderer);

#endif

