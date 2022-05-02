#ifndef STATE_H
    #define STATE_H
    #include <engine.h>

    #define TITLE "UnR34lity"
    #define WIDTH        1280 
    #define HEIGHT       720
    #define W_HALF       WIDTH/2 
    #define H_HALF       HEIGHT/2
    #define RENDERER_ACC 1
    #define FPS          65 
    #define VSYNC        1
    #define RESIZABLE    1 
    #define ASPECT_RATIO (f32) WIDTH/HEIGHT
    
    #define ARGS (gsdl_init_info_t) { TITLE, WIDTH, HEIGHT, RENDERER_ACC, FPS, VSYNC, RESIZABLE }

    typedef struct game_state {
        u08 g_state;
        gsdl_img_t render_texture;
        gsdl_phys_obj_t render_texture_obj;
        gsdl_phys_obj_t mouse;
        i32 saved_dt_avg;
    } game_state_t;

#endif