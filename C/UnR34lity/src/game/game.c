#include "./game.h"

// issues:
    // bullets
    // resizing with breaking fps
    // allocting too much memory with fps
#define GHOST_LIST_SIZE 6
#define PLAYER_STATES 4
#define MAX_Y_MOMENTUM 12
#define PSX 40 
#define TSX 64
#define TSY 64
#define SPAWN_POS (v2_t) { 0, -TSY * 7 }
#define ASPECT_RATIO (f32) WIDTH/HEIGHT

#define MAX_MS_SIZE 40 
#define MAX_MEM_USED 120

/*
typedef struct {
    f32 lifetime;
    gsdl_phys_obj_t obj;
} bullet_t;
    implement bullets
*/

/*

*/

typedef struct game_state {
    u08 g_state;
    gsdl_img_t render_texture;
    gsdl_phys_obj_t render_texture_obj;
    gsdl_phys_obj_t mouse;
    list_t texture_storage;
    list_t ptr_storage;

    // fps
    char * integer_str;
    char * final_dbg_str;

    // transition
    gsdl_circle_t transition_circle;
    u08 transition;

    // Menu
    gsdl_ui_label_t game_name;
    gsdl_ui_label_t press_to_play;
    gsdl_ui_label_t frame_time_label;
    SDL_Texture * frame_time_textures[MAX_MS_SIZE];
    gsdl_ui_label_t mem_used_label;
    SDL_Texture * mem_used_textures[MAX_MEM_USED];

    gsdl_circle_t sharingan;
    gsdl_circle_t sharingan_tomoe[4];

    // actual stuff
    u32 pframe_count;
    u32 frame_count;

    u08 player_img_idx; 
    gsdl_img_t player_idle_textures[2];
    gsdl_img_t player_move_textures[4];
    gsdl_img_t player_power_textures[4];
    gsdl_img_t player_jmp_texture;
    gsdl_img_t player_current_img; 
    gsdl_img_t player_idle_img; 
    gsdl_img_t player_jmp_img; 
    gsdl_img_t player_power_img; 
    gsdl_img_t player_run_img; 
    gsdl_img_t player_prev_img; 


    gsdl_phys_obj_t player_prev[GHOST_LIST_SIZE];    
    gsdl_phys_obj_t player;

    gsdl_circle_t bullet_circle;
    gsdl_circle_t bullet_circle_outline;
    f32 circle_timer;

    f32 player_air_timer;
    i32 player_can_dash;
    f32 player_dash_timer;
    char * player_state;

    gsdl_cam_t camera;
    gsdl_phys_obj_t tiles[101];
    gsdl_phys_obj_t tile_shadow[101];

    gsdl_img_t square_texture[30];
    gsdl_phys_obj_t square[30];

    gsdl_img_t bullet_texture;
    gsdl_phys_obj_t bullets[50];
    i32 bullet_alpha[50];
    f32 bullet_created[50];
    i32 bullet_len;
    f32 shoot_delay;
    f32 shoot_timer;
    u08 bullet_creating;
    u08 bullet_keyup;
} game_state_t;

game_state_t * game_init(gsdl_props_t * props) {
    gsdl_init(ARGS, props);
    game_state_t * state; 

    { // Game state initialization
        state = calloc(1, sizeof(game_state_t));
        state -> g_state = 0;
        state -> texture_storage = list_create(50);
        state -> ptr_storage = list_create(50);
        state -> integer_str = calloc(24, sizeof(char));
        state -> final_dbg_str = calloc(64, sizeof(char));
        SDL_SetWindowMinimumSize(props -> win, WIDTH, HEIGHT);
        //SDL_SetWindowMaximumSize(props -> win, 2560, 1440);
    }

    { // Transition
        gsdl_create_circle(&state -> transition_circle, W_HALF, H_HALF - 20, 0, 100, 30, 34, 255, 1);
        state -> transition = 0;
    }

    { // Create render texture
        gsdl_create_render_tex(&state -> render_texture, WIDTH, HEIGHT, 0, props -> renderer, &state -> texture_storage);
        gsdl_create_phys_obj(&state -> render_texture_obj, mk_v2(0, 0), mk_v2(0, 0), WIDTH, HEIGHT);
        gsdl_create_phys_obj(&state -> mouse, mk_v2(0, 0), mk_v2(0, 0), 16, 16); 
    }

    { // Create menu UI

        gsdl_create_ui_label(&state -> game_name, W_HALF, H_HALF - 100, TITLE, "./res/m5x7.ttf", cWHITE, 60, props -> renderer, &state -> texture_storage, &state -> ptr_storage);
        gsdl_center_ui_label(&state -> game_name);

        gsdl_create_ui_label(&state -> press_to_play, W_HALF, H_HALF, "Press Space to Play or Q to Quit", "./res/m5x7.ttf", cWHITE, 30, props -> renderer, &state -> texture_storage, &state -> ptr_storage);
        gsdl_center_ui_label(&state -> press_to_play);

        gsdl_create_ui_label(&state -> frame_time_label, 60, 10, "Calculating AVG Frame Time...", "./res/m5x7.ttf", cWHITE, 30, props -> renderer, &state -> texture_storage, &state -> ptr_storage);
        gsdl_create_ui_label(&state -> mem_used_label, 60, 30, "Calculating Memory Used (mb)...", "./res/m5x7.ttf", cWHITE, 30, props -> renderer, &state -> texture_storage, &state -> ptr_storage);
    }

    { // Create circle / Sharingan thingy 
        gsdl_create_circle(&state -> sharingan, W_HALF, H_HALF - 20, 148, 100, 30, 34, 255, 1);
        gsdl_create_circle(&state -> sharingan_tomoe[0], W_HALF, H_HALF - 20, 50, cBLACK, 1);
        gsdl_create_circle(&state -> sharingan_tomoe[1], W_HALF, H_HALF - 126, 20, cBLACK, 1);
        gsdl_create_circle(&state -> sharingan_tomoe[2], W_HALF - 64, H_HALF + 24, 20, cBLACK, 1);
        gsdl_create_circle(&state -> sharingan_tomoe[3], W_HALF + 64, H_HALF + 24, 20, cBLACK, 1);
    }

    { // Actual game stuff
        gsdl_load_textures(state -> player_idle_textures, 2, "res/player_img/idle/", 3, 0, 10, 16, props -> renderer, &state -> texture_storage, &state -> ptr_storage);
        gsdl_load_textures(state -> player_move_textures, 4, "res/player_img/run/", 0, 0, 16, 16, props -> renderer, &state -> texture_storage, &state -> ptr_storage);
        gsdl_load_textures(state -> player_power_textures, 2, "res/player_img/power/", 3, 0, 10, 16, props -> renderer, &state -> texture_storage, &state -> ptr_storage);
        gsdl_load_tex(&state -> player_jmp_texture, "res/player_img/jump.png", 0, 0, 14, 16, props -> renderer, &state -> texture_storage, &state -> ptr_storage);
        
        gsdl_load_tex(&state -> player_idle_img, "res/player_img/idle/1.png", 3, 0, 10, 16, props -> renderer, &state -> texture_storage, &state -> ptr_storage);
        gsdl_load_tex(&state -> player_run_img,"res/player_img/run/0.png", 0, 0, 16, 16, props -> renderer, &state -> texture_storage, &state -> ptr_storage);
        gsdl_load_tex(&state -> player_jmp_img, "res/player_img/jump.png", 0, 0, 14, 16, props -> renderer, &state -> texture_storage, &state -> ptr_storage);
        gsdl_load_tex(&state -> player_power_img, "res/player_img/power/1.png", 0, 0, 14, 16, props -> renderer, &state -> texture_storage, &state -> ptr_storage);


        gsdl_set_img_alpha((&state -> player_idle_img), 60);
        gsdl_set_img_alpha((&state -> player_run_img), 60);
        gsdl_set_img_alpha((&state -> player_jmp_img), 60);
        gsdl_set_img_alpha((&state -> player_power_img), 60);


        gsdl_create_phys_obj(&state -> player, SPAWN_POS, mk_v2(9, 0), PSX, TSY);
        gsdl_create_cam(&state -> camera);


        for (u32 u = 0; u < 100; u++) {
            gsdl_phys_obj_t obj;
            i32 y = u - 10;
            if (y > 0) {
                gsdl_create_phys_obj(&obj, mk_v2(TSX * u, 100), mk_v2(0, 0), TSX, TSY);
            } else {
                gsdl_create_phys_obj(&obj, mk_v2(TSX * u, 100 - TSY), mk_v2(0, 0), TSX, TSY);
            }
            state -> tiles[u] = obj;
        }

        for (u32 u = 0; u < 100; u++) {
            gsdl_phys_obj_t obj = state -> tiles[u];
            obj.pos.x -= 1;
            obj.pos.y -= 1;
            obj.w += 2;
            obj.h += 2;
            state -> tile_shadow[u] = obj;
        }

        state -> player_state = calloc(64, sizeof(char));
        list_insert(&state -> ptr_storage, state -> player_state);

        for (u32 u = 0; u < 30; u++) {
            u32 w = randnum_gen(32, 64);
            gsdl_create_phys_obj(&state -> square[u], mk_v2(randnum_gen(0, WIDTH), randnum_gen(0, HEIGHT)), mk_v2(0, randnum_gen(1, 3)), w, w);
            gsdl_create_tex(&state -> square_texture[u], state -> square[u].w, state -> square[u].h, 130, 60, 64, randnum_gen(50, 150), props -> renderer, &state -> texture_storage, &state -> ptr_storage);
        }
        
        gsdl_create_tex(&state -> bullet_texture, 20, 16, 240, 60, 50, 255, props -> renderer, &state -> texture_storage, &state -> ptr_storage);
        state -> bullet_texture.angle = 45;
        gsdl_create_circle(&state -> bullet_circle, 0, 0, 0, 255, 100, 150, 255, 1);
        gsdl_create_circle(&state -> bullet_circle_outline, state -> bullets[state -> bullet_len].pos.x, state -> bullets[state -> bullet_len].pos.y, 0, 255, 255, 255, 255, 0);
         
         
        memset(state -> frame_time_textures, 0, MAX_MS_SIZE);
        memset(state -> mem_used_textures, 0, MAX_MEM_USED);
    }

    //SDL_SetWindowFullscreen(props -> win, SDL_WINDOW_FULLSCREEN_DESKTOP);

    return state;
}

void menu_loop(gsdl_props_t * props, game_state_t * state) { 
    { // Handle events
        SDL_PollEvent(&props -> event);
        switch (props -> event.type) {
            case SDL_QUIT:
                props -> running = 0;
                break;
            
            case SDL_KEYDOWN:
                if (gsdl_check_key(props -> event, SDL_SCANCODE_Q)) {
                    props -> running = 0;
                }
                if (gsdl_check_key(props -> event, SDL_SCANCODE_SPACE)) {
                    state -> transition = 1;
                }

            default:
                break;
        }
    }

    { // Update
        SDL_SetRenderTarget(props -> renderer, state -> render_texture.tex);
        if (state -> transition - 1 == 0) {
            state -> transition_circle.rad += 40;
            if (state -> transition_circle.rad > WIDTH) {
                state -> g_state = 1;
                state -> render_texture.angle = 0;
            }
        }

        if (state -> transition - 1 == 1) {
            state -> transition_circle.rad -= 40;
            if (state -> transition_circle.rad < 0) {
                state -> transition = 0;
            }
        }
    }

    { // Rendering
        gsdl_start_render(props, cBLACK, state -> render_texture.tex);
            { // Main rendering stuff
                gsdl_draw_circle(&state -> sharingan, props -> renderer);
                for (u08 i = 0; i < 4; i++) {
                    gsdl_draw_circle(&state -> sharingan_tomoe[i], props -> renderer);
                }
                gsdl_draw_ui_label(&state -> press_to_play, props -> renderer);
                gsdl_draw_ui_label(&state -> game_name, props -> renderer);
                gsdl_draw_circle(&state -> transition_circle, props -> renderer);
            }
        gsdl_end_render(props);
    }

    gsdl_render_all(props, cBLACK, &state -> render_texture, &state -> render_texture_obj, NULL, 1, WIDTH, HEIGHT);
}

i32 saved_dt_avg;
i32 saved_mem_usage;
i32 f_count = 0;

void main_loop(gsdl_props_t * props, game_state_t * state) { 
    gsdl_start_frame_time_rec(props);
    { // Handle events
        SDL_PollEvent(&props -> event);
        switch (props -> event.type) {
            case SDL_QUIT:
                props -> running = 0;
                break;

            default:
                break;
        }

        props -> keys_pressed = SDL_GetKeyboardState(NULL);
        if (props -> keys_pressed[SDL_SCANCODE_ESCAPE]) {
            state -> transition = 2;
        }


        if (props -> keys_pressed[SDL_SCANCODE_D] || props -> keys_pressed[SDL_SCANCODE_RIGHT]) {
            state -> player.move_r = 1;
        } else {
            state -> player.move_r = 0;
        }

        if (props -> keys_pressed[SDL_SCANCODE_A] || props -> keys_pressed[SDL_SCANCODE_LEFT]) {
            state -> player.move_l = 1;
        } else {
            state -> player.move_l = 0;
        }

        if (props -> keys_pressed[SDL_SCANCODE_SPACE] || props -> keys_pressed[SDL_SCANCODE_UP]) {
            if (state -> player_air_timer < 2) {
                state -> player.y_momentum = -11;
            }
        } 

        if (props -> keys_pressed[SDL_SCANCODE_Y]) {
            SDL_Delay(100);
        }

        if (props -> keys_pressed[SDL_SCANCODE_S] || props -> keys_pressed[SDL_SCANCODE_DOWN]) {
            // fix problem with smaller squares not being registered
            if (state -> shoot_delay < 8 && state -> bullet_keyup <= 1) {
                state -> player.move_l = 0;
                state -> player.move_r = 0;
                state -> player.move_b = 0;
                state -> player.move_t = 0;

                state -> player_state = "power";

                state -> bullet_creating = 1;
                u08 move_l = state -> player_current_img.flip;
                u08 move_r = !state -> player_current_img.flip;

                if (state -> bullet_created[state -> bullet_len] >= 1) {
                    gsdl_create_phys_obj(&state -> bullets[state -> bullet_len], mk_v2(0, 0), mk_v2(0, 0), 0, 0);
                    state -> bullet_created[state -> bullet_len] = 0;
                }

                state -> bullet_created[state -> bullet_len] = 0.5;
                if (move_r) {
                    gsdl_create_phys_obj(&state -> bullets[state -> bullet_len], mk_v2(state -> player.pos.x + state -> player.w + 2, state -> player.pos.y), mk_v2(10, 0), state -> bullets[state -> bullet_len].w + 1, state -> bullets[state -> bullet_len].h + 1);
                } else {
                    gsdl_create_phys_obj(&state -> bullets[state -> bullet_len], mk_v2(state -> player.pos.x - state -> player.w - 2, state -> player.pos.y), mk_v2(10, 0), state -> bullets[state -> bullet_len].w + 1, state -> bullets[state -> bullet_len].h + 1);
                }

                state -> bullets[state -> bullet_len].move_l = move_l;
                state -> bullets[state -> bullet_len].move_r = move_r;

                if (state -> bullet_creating) {
                    state -> bullet_alpha[state -> bullet_len] = 255;
                }
                if (!state -> bullet_keyup) {
                    state -> bullet_keyup++;
                }

            }
        }

        if (props -> keys_pressed[SDL_SCANCODE_R]) {
            state -> player.pos = SPAWN_POS;
        }

    }

    { // Update
        SDL_SetRenderTarget(props -> renderer, state -> render_texture.tex);
        state -> frame_count += 1;
        state -> pframe_count += 1;

        if (state -> player_dash_timer != 0 && state -> player_dash_timer > 0.1) {
            state -> player_dash_timer -= 0.5;
            state -> player_can_dash = 0;
        }

        if (state -> player_dash_timer <= 0) {
            state -> player_dash_timer = 0;
            state -> player_can_dash = 1;
        }


        if (state -> transition - 1 == 0) {
            state -> transition_circle.rad -= 40;
            if (state -> transition_circle.rad < 0) {
                state -> transition = 0;
            }
        }

        if (state -> transition - 1 == 1) {
            state -> transition_circle.rad += 40;
            if (state -> transition_circle.rad > WIDTH) {
                state -> g_state = 0;
            }
        }

        if (state -> bullet_circle.rad > 0) {
            if (state -> circle_timer >= 0.5) {
                state -> bullet_circle.rad = -10;
                state -> bullet_circle_outline.rad = -10;
                state -> circle_timer = 0;
            } else {
                state -> circle_timer += 0.05;
            }
        }

        if (state -> bullet_creating) {
            i32 cx = state -> bullets[state -> bullet_len].pos.x - state -> camera.x;
            i32 cy = state -> bullets[state -> bullet_len].pos.y - state -> camera.y;
            state -> shoot_delay += 0.25;
            if (state -> shoot_delay >= 8) {
                state -> player_state = "idle";
                state -> bullet_creating = 0;
                if (state -> bullets[state -> bullet_len].move_l) {
                    state -> player.pos.x += 10;
                }
                if (state -> bullets[state -> bullet_len].move_r) {
                    state -> player.pos.x -= 10;
                }
                gsdl_create_circle(&state -> bullet_circle, cx + 25, cy, 25, 255, 255, 255, 255, 1);
                gsdl_create_circle(&state -> bullet_circle_outline, cx + 25, cy, state -> bullet_circle.rad + 4, 255, 100, 150, 255, 1);
            }
        }

        if (!state -> bullet_creating) {
            if (state -> bullet_len + 1 < 50) {
                state -> bullet_created[state -> bullet_len] = 1;
                state -> bullet_len++;
                state -> bullet_creating = 0;
            }
            if (!(state -> bullet_len + 1 < 50)) {
                state -> bullet_len = 0;
            }
            state -> bullet_keyup = 0;
        }

        if (state -> shoot_delay >= 8) {
            state -> shoot_timer += 0.25;
            if (state -> shoot_timer > 18) { 
                state -> shoot_delay = 0;
                state -> shoot_timer = 0;
            }
        }

        for (u32 u = 0; u < 30; u++) {
            state -> square[u].move_t = 1;
            gsdl_update_phys_obj_vel(&state -> square[u]);
            gsdl_update_phys_obj_pos(&state -> square[u], NULL, 0);
            state -> square_texture[u].angle += 1; 

            if (state -> square[u].pos.y > HEIGHT + 64) {
                state -> square[u].pos.y = -100;
            }
        }

        if (state -> player.move_l) {
            state -> player_current_img.flip = 1;
            state -> player_run_img.flip = 1;
        }
        if (state -> player.move_r) {
            state -> player_current_img.flip = 0;
            state -> player_run_img.flip = 0;
        }

        gsdl_calc_cam_pos(&state -> camera, props, &state -> player, TSX, TSY);
        gsdl_update_phys_obj_vel(&state -> player);

        if (props -> keys_pressed[SDL_SCANCODE_W]) {
            if (state -> player_can_dash) {
                if (state -> player.move_l) {
                    state -> player.vel.x -= 10;
                }  
                if (state -> player.move_r) {
                    state -> player.vel.x += 10;
                }
                state -> player_dash_timer += 0.1;
            }
        }

        gsdl_update_phys_obj_pos(&state -> player, state -> tiles, 100);

        if ((state -> player.vel.x < 0 || state -> player.vel.x > 0) && state -> player.y_momentum == 0) {
            state -> player_state = "moving";
        } 
        
        if ((state -> player.vel.x == 0 && state -> player.vel.y == 0) && state -> player.y_momentum == 0 && strcmp(state -> player_state, "power") != 0) {
            state -> player_state = "idle";
        }
        
        if (((state -> player.y_momentum < 0 || state -> player.y_momentum >= MAX_Y_MOMENTUM) && state -> player_air_timer > 0.25)) {
            state -> player_state = "jumping";
        }

        if (state -> pframe_count % 30 == 0 && strcmp(state -> player_state, "idle") == 0) {
            state -> player_img_idx++;
        }

        if (state -> pframe_count % 5 == 0 && strcmp(state -> player_state, "moving") == 0) {
            state -> player_img_idx++;
        }

        if (state -> pframe_count % 30 == 0 && strcmp(state -> player_state, "power") == 0) {
            state -> player_img_idx++;
        }

        u08 flip = state -> player_current_img.flip;
        if (strcmp(state -> player_state, "power") == 0) {
            if (state -> player_img_idx < 2) {
                state -> player_current_img = state -> player_power_textures[state -> player_img_idx];
                state -> player_current_img.flip = flip;
            } else {
                state -> player_img_idx = 0;
                state -> player_current_img = state -> player_power_textures[state -> player_img_idx];
                state -> player_current_img.flip = flip;
            }
            state -> player_prev_img = state -> player_power_img;
            state -> player_prev_img.flip = flip;
        }

        if (strcmp(state -> player_state, "moving") == 0) {
            if (state -> player_img_idx < 4) {
                state -> player_current_img = state -> player_move_textures[state -> player_img_idx];
                state -> player_current_img.flip = flip;
            }
            else {
                state -> player_img_idx = 0;
                state -> player_current_img = state -> player_move_textures[state -> player_img_idx];
                state -> player_current_img.flip = flip;
            }
            state -> player_prev_img = state -> player_run_img;
            state -> player_prev_img.flip = flip;
        }
        if (strcmp(state -> player_state, "idle") == 0) {
            if (state -> player_img_idx < 2) {
                state -> player_current_img = state -> player_idle_textures[state -> player_img_idx];
                state -> player_current_img.flip = flip;
            }
            else {
                state -> player_img_idx = 0;
                state -> player_current_img = state -> player_idle_textures[state -> player_img_idx];
                state -> player_current_img.flip = flip;
            }
            state -> player_prev_img = state -> player_idle_img;
            state -> player_prev_img.flip = flip;
        }

        if (strcmp(state -> player_state, "jumping") == 0) {
            state -> player_current_img = state -> player_jmp_texture;
            state -> player_current_img.flip = flip;
            state -> player_prev_img = state -> player_jmp_img;
            state -> player_prev_img.flip = flip;
        }

        for (u32 u = 0; u < 50; u++) {
            gsdl_phys_obj_t obj = state -> bullets[u];
            if (state -> bullet_created[u] >= 1) {
                gsdl_update_phys_obj_vel(&obj);
                gsdl_update_phys_obj_pos(&obj, NULL, 0);
            }
            state -> bullets[u] = obj;
        } 

        if (state -> frame_count < GHOST_LIST_SIZE) {
            state -> player_prev[state -> frame_count] = state -> player;
        } else {
            state -> frame_count = 0;
        }

        state -> player.y_momentum += 1.5;

        if (state -> player.coll_b) {
            state -> player.y_momentum = 0;
            state -> player_air_timer = 0;
        } else {
            state -> player_air_timer += 0.25;
        }

        if (state -> player.y_momentum >= MAX_Y_MOMENTUM) {
            state -> player.y_momentum = MAX_Y_MOMENTUM;
        }

    }

    i32 changed_frame_time = 0;
    saved_dt_avg = props -> dt;
    gsdl_get_frame_time_str("AVG Frame Time (ms): ", state -> integer_str, state -> final_dbg_str, props);
    if (saved_dt_avg != props -> dt) {
        if (props -> dt >= MAX_MS_SIZE - 1) {
            state -> frame_time_label.label_text = "Frame Time Is TOO High (> 35ms)";
            state -> frame_time_textures[MAX_MS_SIZE - 1] = SDL_CreateTextureFromSurface(props -> renderer, TTF_RenderText_Blended(state -> frame_time_label.label_font, state -> frame_time_label.label_text, state -> frame_time_label.text_color));
        }

        if (props -> dt < MAX_MS_SIZE && state -> frame_time_textures[props -> dt] == 0x0) {
            state -> frame_time_label.label_text = state -> final_dbg_str;
            state -> frame_time_textures[props -> dt] = SDL_CreateTextureFromSurface(props -> renderer, TTF_RenderText_Blended(state -> frame_time_label.label_font, state -> frame_time_label.label_text, state -> frame_time_label.text_color));
        }
        saved_dt_avg = props -> dt;
        changed_frame_time = 1;
    } 

    memset(state -> final_dbg_str, 0, strlen(state -> final_dbg_str));
    i32 changed_mem_usage = 0;
    saved_mem_usage = props -> mb_used;
    gsdl_get_mem_used_str(state -> integer_str, state -> final_dbg_str, props);
    if ((saved_mem_usage != props -> mb_used) || (f_count == 0)) {
        if (props -> mb_used >= MAX_MEM_USED - 1) {
            state -> mem_used_label.label_text = "Mem Usage TOO High (> 50mb)";
            state -> mem_used_textures[MAX_MEM_USED - 1] = SDL_CreateTextureFromSurface(props -> renderer, TTF_RenderText_Blended(state -> mem_used_label.label_font, state -> mem_used_label.label_text, state -> mem_used_label.text_color));
        }

        if (props -> mb_used < MAX_MEM_USED && state -> mem_used_textures[props -> mb_used] == 0x0) {
            state -> mem_used_label.label_text = state -> final_dbg_str;
            state -> mem_used_textures[props -> mb_used] = SDL_CreateTextureFromSurface(props -> renderer, TTF_RenderText_Blended(state -> mem_used_label.label_font, state -> final_dbg_str, state -> mem_used_label.text_color));
        }
        saved_mem_usage = props -> mb_used;
        changed_mem_usage = 1;
        f_count++;
    } 


    { // Rendering
        gsdl_start_render(props, 80, 30, 34, 255, state -> render_texture.tex);
        { // Main rendering stuff   

            for (u32 u = 0; u < 30; u++) {
                gsdl_draw_phys_obj(&state -> square_texture[u], &state -> square[u], NULL, props -> renderer);
            }

            for (u32 u = 0; u < 50; u++) {
                gsdl_phys_obj_t obj = state -> bullets[u];
                gsdl_set_img_alpha((&state -> bullet_texture), state -> bullet_alpha[u]);
                gsdl_draw_phys_obj(&state -> bullet_texture, &obj, &state -> camera, props -> renderer);
            }            

            for (u08 u = 0; u < GHOST_LIST_SIZE; u++) { 
                gsdl_draw_phys_obj(&state -> player_prev_img, &state -> player_prev[u], &state -> camera, props -> renderer);
            }

            gsdl_draw_phys_obj(&state -> player_current_img, &state -> player, &state -> camera, props -> renderer);
            gsdl_draw_circle(&state -> bullet_circle_outline, props -> renderer);
            gsdl_draw_circle(&state -> bullet_circle, props -> renderer);


            for (u32 u = 0; u < 100; u++) {
                gsdl_draw_phys_obj_rect(&state -> tile_shadow[u], 0, 0, 0, 255, &state -> camera, props -> renderer);
                gsdl_draw_phys_obj_rect(&state -> tiles[u], 255, u * 20, u * 20, 255, &state -> camera, props -> renderer);
            }

            gsdl_draw_circle(&state -> transition_circle, props -> renderer); 

            if (changed_frame_time) {
                state -> frame_time_label.label_tex = state -> frame_time_textures[props -> dt];   
                SDL_QueryTexture(state -> frame_time_label.label_tex, NULL, NULL, &state -> frame_time_label.pos.w, &state -> frame_time_label.pos.h);
            }

            gsdl_draw_ui_label(&state -> frame_time_label, props -> renderer);

            if (changed_mem_usage) {
                state -> mem_used_label.label_tex = state -> mem_used_textures[props -> mb_used];
                SDL_QueryTexture(state -> mem_used_label.label_tex, NULL, NULL, &state -> mem_used_label.pos.w, &state -> mem_used_label.pos.h);
            }

            gsdl_draw_ui_label(&state -> mem_used_label, props -> renderer);
            memset(state -> final_dbg_str, 0, strlen(state -> final_dbg_str));

        }
        gsdl_end_render(props);
    }

    gsdl_render_all(props, cBLACK, &state -> render_texture, &state -> render_texture_obj, NULL, 1, WIDTH, HEIGHT);
    gsdl_stop_frame_time_rec(props);
}

// setoolkit
void game_loop(gsdl_props_t * props, game_state_t * state) {
    { // Handle game states 
        switch (state -> g_state) {
            case 0:
                menu_loop(props, state);
                break;

            case 1:
                main_loop(props, state);
                break;

            default:
                props -> running = 0;
                break;
        }
    }
}

void game_cleanup(gsdl_props_t * props, game_state_t * state) {
    /* for (u32 u = 0; u < MAX_MS_SIZE; u++) {
        if (state -> frame_time_textures[u] != 0x0) {
            free(state -> frame_time_textures[u]);
        }
    }
    fix this by adding texture to texture storage
    for (u32 u = 0; u < MAX_MEM_USED; u++) {
        if (state -> mem_used_textures[u] != 0x0) {
            free(state -> mem_used_textures[u]);
        }
    } */ 
    
    gsdl_serialize_img(&state -> render_texture, "render.png", props -> renderer);

    free(state -> integer_str);
    free(state -> final_dbg_str);
    // add to pointer stg

    gsdl_destroy(props, &state -> texture_storage, &state -> ptr_storage);    
}
