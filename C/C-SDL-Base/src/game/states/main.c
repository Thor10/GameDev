#include <game.h>

void main_handle_events(gsdl_props_t * props, game_state_t * state) {
    SDL_PollEvent(&props -> event);
    switch (props -> event.type) {
        case SDL_QUIT:
            props -> running = 0;
            break;

        default:
            break;
    }

    props -> keys_pressed = SDL_GetKeyboardState(NULL);
}

void main_update(gsdl_props_t * props, game_state_t * state) {

}


void main_render(gsdl_props_t * props, game_state_t * state) {
    gsdl_start_render(props, 60, 30, 34, 255, state -> render_texture.tex);
    { // Main rendering stuff   

    }
    gsdl_end_render(props);
    gsdl_render_all(props, cBLACK, &state -> render_texture, &state -> render_texture_obj, NULL, 1, WIDTH, HEIGHT);
}
void main_loop(gsdl_props_t * props, game_state_t * state) { 
    gsdl_start_frame_time_rec(props);
        // Handle events
        main_handle_events(props, state);
        // Update
        main_update(props, state);
        // Rendering
        main_render(props, state); 
    gsdl_stop_frame_time_rec(props);

    state -> saved_dt_avg = props -> dt;
    gsdl_get_avg_frame_time(props);
    if (state -> saved_dt_avg != props -> dt) {
        printf("\rFrame Time: %d ms", props -> dt);
        fflush(stdout);
    } 
}

