#ifndef GAME_H
    #define GAME_H
    #include "./engine/engine.h"

    #define TITLE "UnR34lity"
    #define WIDTH        640 
    #define HEIGHT       480
    #define W_HALF       WIDTH/2 
    #define H_HALF       HEIGHT/2
    #define RENDERER_ACC 1
    #define FPS          60
    #define VSYNC        1
    #define RESIZABLE    1 

    #define ARGS (gsdl_init_info_t) { TITLE, WIDTH, HEIGHT, RENDERER_ACC, FPS, VSYNC, RESIZABLE }

    typedef struct game_state game_state_t;

    game_state_t * game_init(gsdl_props_t * prop);
    void game_loop(gsdl_props_t * props, game_state_t * state);
    void game_cleanup(gsdl_props_t * props, game_state_t * state);
    void game_state_info(const game_state_t * state);
#endif