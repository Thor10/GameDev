#ifndef _light_engine
    #define _light_engine
    #include <stdio.h>
    #include <stdlib.h>
    #include "prof.h"
    #include "entity.h"
    #include "img.h"
    #include "util.h"
    #include "draw.h"
    #include "file.h"
    #include "maths.h"
    #include "str.h"
    #include "audio.h"
    #include "particles.h"
    #include "anim.h"
    #include "ht.h"
    #include "font.h"
    #ifdef __EMSCRIPTEN__
        #include <emscripten.h>
        #include <emscripten/html5.h>
    #endif

    #define le_arg int argc, char * argv[]
    #define le_default_event_check default: break;
    #define le_events SDL_Event

    emp le_init(const le_str win_title, i16 width, i16 height, i32 fps, i08 sync, i08 renderer_accel);
    emp le_quit_check(SDL_Event * event);
    emp le_handle_resizable(i16 resizable);
    emp le_set_win_flags(u32 flags); 
    emp le_create_win(const le_str title, i32 width, i32 height);
    emp le_create_rnd(i08 sync, i08 renderer_accel);
    emp le_draw_circle(i32 centreX, i32 centreY, i32 radius, i16 r, i16 g, i16 b, i16 a);
    emp le_uninit();
    emp le_cls(i16 r, i16 g, i16 b, i16 a);
    emp le_start_frame();
    emp le_end_frame();
    f64 le_get_fps();
    f64 le_get_dt();
    i08 le_get_keydown(const i32 scancode, SDL_Event * event); 
    i08 le_get_mouse_button_down(const i32 scancode, SDL_Event * event);
    i08 get_le_on();
    emp set_le_on(i08 setting);
    emp le_game_loop(void (* game_loop)());
    SDL_Renderer * le_get_renderer();
    SDL_Window * le_get_window();
    le_events le_get_event(i08 wait);
    emp le_scale_renderer(i32 scale_x, i32 scale_y);
    

    #define mk_le_img_t(path, x, y, w, h) \
        _le_img_t(path, x, y, w, h, le_get_renderer());

    #define le_draw_rect(rect, r, g, b, a) \
        _le_draw_rect(rect, r, g, b, a, le_get_renderer());

    #define le_draw_le_entity_t(entity, image) \
        _le_draw_le_entity_t(entity, image, le_get_renderer());

    #define le_draw_le_entity_t_rect(entity, r, g, b, a) \
        _le_draw_le_entity_t_rect(entity, r, g, b, a, le_get_renderer());

    #define le_draw_le_img_t(image, x, y) \
        _le_draw_le_img_t(image, x, y, le_get_renderer());

    #define create_le_ui_label_t_text(label) \
        _create_le_ui_label_t_text(label, le_get_renderer());

    #define le_draw_le_ui_label_t(label) \
        _le_draw_le_ui_label_t(label, le_get_renderer());

    #define le_draw_le_entity_t_ex(entity, img, angle, center, flip) \
        _le_draw_le_entity_t_ex(entity, img, le_get_renderer(), angle, center, flip)

    #define le_centered_draw_le_entity_t(entity, img) \
        _le_draw_centered_le_entity_t(entity, img, le_get_renderer())

    #define mk_le_ui_label_t(_x, _y, text, font, _r, _g, _b, _a, size) \
        _mk_le_ui_label_t(_x, _y, text, font, _r, _g, _b, _a, size, le_get_renderer())
#endif
