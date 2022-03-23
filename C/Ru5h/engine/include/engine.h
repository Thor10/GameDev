#ifndef _light_engine
    #define _light_engine
    #include <stdio.h>
    #include <stdlib.h>
    #include "entity.h"
    #include "img.h"
    #include "util.h"
    #include "draw.h"
    #include "file.h"
    #include "maths.h"
    #include "str.h"
    #include "timer.h"
    #include "audio.h"
    #ifdef __EMSCRIPTEN__
        #include <emscripten.h>
        #include <emscripten/html5.h>
    #endif

    #define le_arg int argc, char * argv[]
    #define le_default_event_check default: break;

    emp le_init(const le_str win_title, i16 width, i16 height, i32 fps, i08 sync);
    emp le_quit_check(SDL_Event * event);
    emp le_handle_resizable(i16 resizable);
    emp le_create_win(const le_str title, i32 width, i32 height);
    emp le_create_rnd(i08 sync);
    emp le_draw_circle(i32 centreX, i32 centreY, i32 radius, i16 r, i16 g, i16 b, i16 a);
    emp le_uninit();
    emp le_cls(i16 r, i16 g, i16 b, i16 a);
    SDL_Event le_events();
    emp le_render_start();
    emp le_render_end();
    emp le_start_frame();
    emp le_end_frame();
    f64 le_get_fps();
    f64 le_get_dt();
    i08 get_le_on();
    emp set_le_on(i08 setting);
    emp le_game_loop(void (* game_loop)());
    SDL_Renderer * le_get_renderer();
    SDL_Window * le_get_window();
    

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


#endif
