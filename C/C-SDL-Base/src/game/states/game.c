#include <game.h>

game_state_t * game_init(gsdl_props_t * props) {
    gsdl_init(ARGS, props);
    game_state_t * state; 

    { // Game state initialization
        state = calloc(1, sizeof(game_state_t));
        state -> g_state = 0;
        SDL_SetWindowMinimumSize(props -> win, WIDTH, HEIGHT);
        //SDL_SetWindowMaximumSize(props -> win, 2560, 1440);
    }

    { // Create render texture
        gsdl_create_render_tex(&state -> render_texture, WIDTH, HEIGHT, 0, props -> renderer, &props -> texture_storage);
        gsdl_create_phys_obj(&state -> render_texture_obj, mk_v2(0, 0), mk_v2(0, 0), WIDTH, HEIGHT);
        gsdl_create_phys_obj(&state -> mouse, mk_v2(0, 0), mk_v2(0, 0), 16, 16); 
    }

    //SDL_SetWindowFullscreen(props -> win, SDL_WINDOW_FULLSCREEN_DESKTOP);
    return state;
}


// setoolkit
void game_loop(gsdl_props_t * props, game_state_t * state) {
    { // Handle game states 
        switch (state -> g_state) {
            case 0:
                main_loop(props, state);
                break;

            default:
                props -> running = 0;
                break;
        }
    }
}

void game_cleanup(gsdl_props_t * props, game_state_t * state) {
    gsdl_destroy(props);    
}