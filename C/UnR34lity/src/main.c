#include "./game/game.h"

#ifdef __EMSCRIPTEN__ 
    #include <emscripten.h>
    #include <emscripten/html5.h>

    gsdl_props_t * game;
    game_state_t * state;
    
    void loop_em() {
        game_loop(game, state);
    }

    i32 main(i32 argv, char ** args) {
        game = calloc(1, sizeof(gsdl_props_t));
        state = game_init(game);

        if (!state) {
            log_fatal_err("Failed to initialize");
        }

        emscripten_set_main_loop(loop_em, 0, 1);
        game_cleanup(game, state);
        log_msg("Successfully Exited");
        return 0;
    }
#else
    i32 main(i32 argv, char ** args) {
        gsdl_props_t * game = calloc(1, sizeof(gsdl_props_t));
        game_state_t * state = game_init(game);

        if (!state) {
            log_fatal_err("Failed to initialize");
        }

        while (game -> running) {
            game_loop(game, state);
        }

        game_cleanup(game, state);
        free(game);
        free(state);
 
        log_msg("Successfully Exited");
        return 0;
    }
#endif
