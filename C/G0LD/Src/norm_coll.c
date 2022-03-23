#include "./Includes.h"

/*
 * Note:
 * Globals are being used as Emscripten really wants functional programming so yeah I don't really 
 * know another way
 */

/* Declaring the global variables I need */
#define SCREEN_W 640 
#define SCREEN_H 512

#define TILE_SIZE 32

#define ENEMY_NUM 10

typedef struct {
    dynamic_entity_t entity_comp;
    float y_momentum;
    float jump_timer;
    image2d image_comp; 
} player_union;

typedef struct {
    dynamic_entity_t entity_comp;
    image2d image_comp;
} tile_union;

typedef struct {
    dynamic_entity_t entities[ENEMY_NUM]; 
    image2d image_comp;
} enemies_union;

typedef struct {
    int moving_left;
    int moving_right;
    int moving_up;
    int moving_down;
} movement_union;

player_union player;
movement_union player_movement;

tile_union tile;

int y;
int x;

/* Initializing game, mainly variables needed and the little engine behind the scenes */
int init_game() { 
    le_init();
    le_create_window("G0LD", SCREEN_W, SCREEN_H, &le_g_flags);
    le_create_renderer();

    init_dynamic_entity(&player.entity_comp, 128, 128, TILE_SIZE, TILE_SIZE, 3, 9);
    init_image2d(&player.image_comp, "Resources/player.png", TILE_SIZE, TILE_SIZE, 0, 0);

    init_dynamic_entity(&tile.entity_comp, 0, 0, TILE_SIZE, TILE_SIZE, 0, 0);
    init_image2d(&tile.image_comp, "Resources/tile.png", TILE_SIZE, TILE_SIZE, 0, 0);

    player.y_momentum = 0;
    player.jump_timer = 0;

    y = 0;
    x = 0;

    return 1;
}

/* Game loop... */
void game_loop() {
    le_get_events();

    switch (le_g_event.type) {
        case SDL_QUIT:
            le_g_game_running = 0;
            break;

        case SDL_KEYDOWN:
            switch (le_g_event.key.keysym.scancode) {
                case SDL_SCANCODE_UP:
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_SPACE:
                    if (player.jump_timer < 3) {
                        player.y_momentum = -9;
                        //player.entity_comp.vel.y -= player.entity_comp.acc.y * 2;
                    }

                    break;

                case SDL_SCANCODE_LEFT:
                case SDL_SCANCODE_A:
                    player_movement.moving_left = 1;
                    break;

                case SDL_SCANCODE_RIGHT:
                case SDL_SCANCODE_D:
                    player_movement.moving_right = 1;
                    break;

                default:
                    break;
            }
            break;

        case SDL_KEYUP:
            switch (le_g_event.key.keysym.scancode) {
                case SDL_SCANCODE_LEFT:
                case SDL_SCANCODE_A:
                    player_movement.moving_left = 0;
                    break;

                case SDL_SCANCODE_RIGHT:
                case SDL_SCANCODE_D:
                    player_movement.moving_right = 0;
                    break;

                default:
                    break;
            }
            break;

        default:
            break; 
    } 

    /* Update */
 

    player.entity_comp.vel.y += player.y_momentum;
    player.y_momentum += 0.75;

    if (player.y_momentum >= 5.5) {
        player.y_momentum = 5.5;
    }

    if (player_movement.moving_left) {
        player.entity_comp.vel.x -= player.entity_comp.acc.x;
    }

    if (player_movement.moving_right) {
        player.entity_comp.vel.x += player.entity_comp.acc.x;
    }

    if (player.entity_comp.bottom_collision) { 
        player.y_momentum = 0;
        player.jump_timer = 0;
        player.entity_comp.bottom_collision = 0;
    }

    else {
        player.jump_timer += 0.25;
    }

    if (player.entity_comp.top_collision) {
        player.y_momentum += 3;
    }
    
    player.entity_comp.pos.x += player.entity_comp.vel.x;

	y = 0;
	x = 0;
	for (y = 0; y < (SCREEN_H / TILE_SIZE); y++) {
	    for (x = 0; x < (SCREEN_W / TILE_SIZE); x++) {
            tile.entity_comp.pos.x = x * TILE_SIZE;
            tile.entity_comp.pos.y = y * TILE_SIZE;

            if ((y == 0 || y == (SCREEN_H / TILE_SIZE) - 1) 
            ||  (x == 0 || x == (SCREEN_W / TILE_SIZE) - 1)
               ) {

                if (collision_detect(player.entity_comp, tile.entity_comp)) {
                    if (player.entity_comp.vel.x > 0) {
                        player.entity_comp.pos.x -= player.entity_comp.acc.x;
                        player.entity_comp.right_collision = 1;
                        player.entity_comp.left_collision = 0;
                    }

                    else if (player.entity_comp.vel.x < 0) {
                        player.entity_comp.pos.x += player.entity_comp.acc.x;
                        player.entity_comp.right_collision = 0;
                        player.entity_comp.left_collision = 1;
                    }

                }
            }
        }
	}

    player.entity_comp.vel.x = 0;

    player.entity_comp.pos.y += player.entity_comp.vel.y;

	y = 0;
	x = 0;
	for (y = 0; y < (SCREEN_H / TILE_SIZE); y++) {
	    for (x = 0; x < (SCREEN_W / TILE_SIZE); x++) {
            tile.entity_comp.pos.x = x * TILE_SIZE;
            tile.entity_comp.pos.y = y * TILE_SIZE;

            if ((y == 0 || y == (SCREEN_H / TILE_SIZE) - 1) 
            ||  (x == 0 || x == (SCREEN_W / TILE_SIZE) - 1)
               ) {

                if (collision_detect(player.entity_comp, tile.entity_comp)) {
                    if (player.entity_comp.vel.y > 0) {
                        player.entity_comp.pos.y -= 1;
                        player.entity_comp.bottom_collision = 1;
                        player.entity_comp.top_collision = 0;
                        player.jump_timer = 0;
                    }

                    else if (player.entity_comp.vel.y < 0) {
                        player.entity_comp.pos.y += player.entity_comp.acc.y;
                        player.entity_comp.bottom_collision = 0;
                        player.entity_comp.top_collision = 1;
                    }


                }

            }
        }
	}

    player.entity_comp.vel.y = 0;


    /* Render */
    le_clear_screen(0, 0, 10, 255);
    le_start_render();
	
	/* Real Rendering here */

	y = 0;
	x = 0;
	for (y = 0; y < (SCREEN_H / TILE_SIZE); y++) {
	    for (x = 0; x < (SCREEN_W / TILE_SIZE); x++) {
            tile.entity_comp.pos.x = x * TILE_SIZE;
            tile.entity_comp.pos.y = y * TILE_SIZE;

            if ((y == 0 || y == (SCREEN_H / TILE_SIZE) - 1) 
            ||  (x == 0 || x == (SCREEN_W / TILE_SIZE) - 1)
               )
                draw_dynamic_entity_img(&tile.entity_comp, &tile.image_comp);
        }
	}

	draw_dynamic_entity_img(&player.entity_comp, &player.image_comp);

    le_end_render();
}

/* Main function.. */
int main(SDL_MAIN_ARGS) {
    if (!init_game()) {
        le_log_error("Error Initializing Game!", NULL);
    }

    le_log_info("Successfully Initialized Game", NULL);

    # ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(game_loop, 0, 1);
    # else 
        while (le_g_game_running) {
            game_loop();
            le_delay(16);
        }
    # endif

    le_uninit();

    return 0;
}

/*

#include "./Includes.h"

#define SCREEN_W 640 
#define SCREEN_H 480

image2d g_map_tile_1;
image2d g_map_tile_2;
char * g_map;
static_entity * g_map_tile_rects;
int g_map_x = 0;
int g_map_y = 0;

vec2 scroll;
dynamic_entity g_player;
dynamic_entity g_player_img_scrolled;

image2d g_player_idle_frames[2];
image2d g_player_run_frames[2];

int g_player_frame_duration = 50;
int g_current_player_frame = 0;

char * g_player_state;

int g_player_jump;
int g_player_can_jump;
float g_player_jump_timer;
float g_player_y_momentum;
int g_player_move_right;
int g_player_move_left;
int g_player_right_collision;
int g_player_left_collision;
int g_player_up_collision;
int g_player_bottom_collision;

int i;

char * g_game_state;

int g_map_player_x;
int g_map_player_y;

dynamic_entity g_dice;
image2d g_dice_frames[6];

int g_genned_dice_num;
int g_guessed_dice_num;
int g_guessed_num;
int g_actual_num;



int init_game() { 
    le_init();
    le_create_window("Game", SCREEN_W, SCREEN_H, &le_g_flags);
    le_create_renderer();

    g_map = load_text_file("Resources/Maps/map_0.txt");
   
    g_map_tile_rects = malloc(strlen(g_map) * sizeof(static_entity));

    for (i = 0; i < strlen(g_map); i++) {
        if (g_map[i] == '1' || g_map[i] == '2') {
            g_map_tile_rects[i].pos.x = g_map_x * 32;
            g_map_tile_rects[i].pos.y = g_map_y * 32;
            g_map_tile_rects[i].w     = 32;
            g_map_tile_rects[i].h     = 32;
        }

        if (g_map[i] == '3') {
            g_map_player_x = g_map_x * 32;
            g_map_player_y = g_map_y * 32;
        }

        if (g_map[i] != 'n') {
            g_map_x += 1;
        }

        if (g_map[i] == 'n') {
            g_map_x = 0;
            g_map_y += 1;
        }

    }

    init_image2d(
	    &g_map_tile_1,
	    "Resources/Images/tile.png",
	    32, 32,
	    0, 0
    );

    init_image2d(
	    &g_map_tile_2,
	    "Resources/Images/tile_under.png",
	    32, 32,
	    0, 0
    );

    init_dynamic_entity(&g_player, g_map_player_x, g_map_player_y, ((9 / 16) * 32) + 20, 32, 9, 0);

    init_image2d(
	    &g_player_idle_frames[0], 
	    "Resources/Images/Player/idle/player_idle_0.png", 
	    32, 32, 
	    0, 0
    );

    init_image2d(
	    &g_player_idle_frames[1], 
	    "Resources/Images/Player/idle/player_idle_1.png", 
	    32, 32, 
	    0, 0
    );

    init_image2d(
	    &g_player_run_frames[0], 
	    "Resources/Images/Player/run/player_run_0.png", 
	    32, 32, 
	    0, 0
    );

    init_image2d(
	    &g_player_run_frames[1], 
	    "Resources/Images/Player/run/player_run_1.png", 
	    32, 32, 
	    0, 0
    );

    g_player_state = "idle";
    g_player_y_momentum = 0.5;

    g_game_state = "playing";

    init_dynamic_entity(&g_dice, 320 - 64, 240 - 64, 128, 128, 0, 0);

    init_image2d(
	    &g_dice_frames[0],
	    "Resources/Images/Dice/1.png",
	    128, 128,
	    0, 0
    );

    init_image2d(
	    &g_dice_frames[1],
	    "Resources/Images/Dice/2.png",
	    128, 128,
	    0, 0
    );

    init_image2d(
	    &g_dice_frames[2],
	    "Resources/Images/Dice/3.png",
	    128, 128,
	    0, 0
    );

    init_image2d(
	    &g_dice_frames[3],
	    "Resources/Images/Dice/4.png",
	    128, 128,
	    0, 0
    );

    init_image2d(
	    &g_dice_frames[4],
	    "Resources/Images/Dice/5.png",
	    128, 128,
	    0, 0
    );

    init_image2d(
	    &g_dice_frames[5],
	    "Resources/Images/Dice/6.png",
	    128, 128,
	    0, 0
    );

    g_genned_dice_num = 0;
    g_guessed_dice_num = 0;


    return 1;
}

void game_loop() {
    le_get_events();

    scroll.x += (int) ((g_player.pos.x - scroll.x - ((SCREEN_W * 0.5) - (g_player.w * 0.5))) / 10);
    scroll.y += (int) ((g_player.pos.y - scroll.y - ((SCREEN_H * 0.5) - (g_player.h * 0.5))) / 10);

    switch (le_g_event.type) {
        case SDL_QUIT:
            le_g_game_running = 0;
            break;

        case SDL_KEYDOWN:

            switch(le_g_event.key.keysym.scancode) {

                case SDL_SCANCODE_UP:
                    if (g_player_jump_timer             < 2
                    && strcmp(g_game_state, "playing") == 0) {
                        g_player_y_momentum = -8;
                        g_player_state = "jump";
                    }
                    break;

                case SDL_SCANCODE_SPACE:
                    if (g_player_jump_timer             < 2
                    && strcmp(g_game_state, "playing") == 0) {
                        g_player_y_momentum = -8;
                        g_player_state = "jump";
                    }
                    break;

                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    if (strcmp(g_game_state, "playing") == 0) {
                        g_player_move_left = 1;
                        g_player_state = "run";
                    }
                    break;

                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    if (strcmp(g_game_state, "playing") == 0) {
                        g_player_move_right = 1;
                        g_player_state = "run";
                    }
                    break;

                default:
                    break;
            }

            break;


        case SDL_KEYUP:

            switch(le_g_event.key.keysym.scancode) {

                case SDL_SCANCODE_UP:
                case SDL_SCANCODE_SPACE:
                    break;

                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    g_player_move_left = 0;
                    break;

                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    g_player_move_right = 0;
                    break;

                default:
                    break;
            }

            break;

        default:
            break; 
    } 

    
    g_player_frame_duration -= 1;

    if (   g_player_frame_duration         <= 0 
	&& g_player.vel.y                  == 0
	&& strcmp(g_player_state, "idle")  == 0) {
        g_player_frame_duration = 50; 
        g_current_player_frame += 1;
    }

    if (   g_player_frame_duration         <= 0 
	&& g_player.vel.y                  == 0
	&& strcmp(g_player_state, "run")  == 0) {
        g_player_frame_duration = 5; 
        g_current_player_frame += 1;
    }

    if (g_current_player_frame > 1) {
        g_current_player_frame = 0;
    }

    if (g_player_move_right && g_player_right_collision == 0) {
        g_player.vel.x += g_player.acc.x;
    }

    if (g_player_move_left && g_player_left_collision == 0) {
        g_player.vel.x -= g_player.acc.x;
    }

    if ( g_player_bottom_collision) { 
        g_player_jump_timer = 0;
        g_player_y_momentum = 0;
    }

    if (!g_player_bottom_collision) {
        g_player_jump_timer += 0.25;
    }

    if ( g_player_up_collision) {
        g_player_y_momentum += 3;
    }
    
    g_player.vel.y += g_player_y_momentum;
    g_player_y_momentum += 0.5;

    if (g_player_y_momentum >= 6) {
        g_player_y_momentum = 6;
    }

    if (g_player.vel.x == 0) {
        g_player_state = "idle";
    }
   
    g_player_bottom_collision = 0;
    g_player_up_collision     = 0;
    g_player_right_collision  = 0;
    g_player_left_collision   = 0;

    g_player.pos.x += g_player.vel.x;
    for (i = 0; 
	 i < strlen(g_map);
	 i++) {

        if (collision_detect(g_player, g_map_tile_rects[i])) {
            if (g_player.vel.x > 0) { 
                g_player.pos.x -= g_player.acc.x;
                g_player_right_collision = 1;
                g_player_left_collision = 0;

            } else if (g_player.vel.x < 0) { 
                g_player.pos.x += g_player.acc.x;
                g_player_left_collision = 1;
                g_player_right_collision = 0;  
            }
        
	    }

    }

    g_player.pos.y += g_player.vel.y;
    for (i = 0; 
	 i < strlen(g_map);
	 i++) {

        if (collision_detect(g_player, g_map_tile_rects[i])) {
	        if (g_player.vel.y > 0){ 
		        g_player.pos.y -= 1; 
		        g_player_bottom_collision = 1; 
		        g_player_up_collision = 0;

	        } else if (g_player.vel.y < 0) {
		        g_player.pos.y += 8; 
		        g_player_bottom_collision = 0;
		        g_player_up_collision = 1;
	        }
	    }
    }


    g_player.vel.x = 0;
    g_player.vel.y = 0;

    if (strcmp(g_game_state, "playing") == 0) 
        le_clear_screen(0, 32, 151, 255);

    if (strcmp(g_game_state, "rolling_dice") == 0) 
        le_clear_screen(0, 0, 0, 0);

    le_start_render();

	if (strcmp(g_game_state, "rolling_dice") == 0) {
	    draw_entity_img(&g_dice, &g_dice_frames[rand() % 6]);
	}

	for (i = 0; 
	     i < strlen(g_map);
	     i++) {
        dynamic_entity scrolled_tile;

	    scrolled_tile.pos.x = g_map_tile_rects[i].pos.x - scroll.x;
	    scrolled_tile.pos.y = g_map_tile_rects[i].pos.y - scroll.y;
	    
	    scrolled_tile.w     = g_map_tile_rects[i].w;
	    scrolled_tile.h     = g_map_tile_rects[i].h;

	    if (g_map[i] == '1' && strcmp(g_game_state, "playing") == 0) {
            draw_entity_img(
                &scrolled_tile,
                &g_map_tile_2
            );
	    }

	    if (g_map[i] == '2' && strcmp(g_game_state, "playing") == 0) {
            draw_entity_img(
                &scrolled_tile,
                &g_map_tile_1
            );
	    }

	}

	g_player_img_scrolled = g_player;
	
	g_player_img_scrolled.pos.x -= scroll.x;
	g_player_img_scrolled.pos.y -= scroll.y;
	
	if ((  strcmp(g_player_state, "idle") == 0 
	    || strcmp(g_player_state, "jump") == 0)
	    && strcmp(g_game_state, "playing") == 0) {
            draw_entity_img(
                &g_player_img_scrolled, 
                &g_player_idle_frames[g_current_player_frame]
            );
	}

	if (   strcmp(g_player_state, "run") == 0 
	    && strcmp(g_game_state, "playing") == 0) {
            draw_entity_img(
                &g_player_img_scrolled, 
                &g_player_run_frames[g_current_player_frame]
            );
	}


    le_end_render();
}

int main(SDL_MAIN_ARGS) {
    if (!init_game()) {
        le_log_error("Error Initializing Game!", NULL);
    }

    le_log_info("Successfully Initialized Game", NULL);

    # ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(game_loop, 0, 1);
    # else 
        while (le_g_game_running) {
            game_loop();
            le_delay(16);
        }
    # endif

    le_uninit();

    return 0;
}



*/
