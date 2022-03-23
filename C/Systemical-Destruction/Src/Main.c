#include "./Includes.h"
#include <string.h>

/*
 * Note:
 * Globals are being used as Emscripten really wants functional programming so yeah I don't really 
 * know another way
 */



#define SCREEN_W 640 
#define SCREEN_H 512

#define GAME_FPS 90 

#define TILE_SIZE 32

#define ENEMY_NUM 10

typedef struct {
    int moving_left;
    int moving_right;
    int moving_up;
    int moving_down;
} movement_t;

typedef struct {
    vec2 scroll;
    movement_t movement;
    dynamic_entity_t entity_comp;
    float y_momentum;
    float jump_timer;
    image2d image_comp[5]; 
    char * state;
    int anim_index;
    int frame_dur;
    int actual_health;
    int health;
    int dead;
    SDL_RendererFlip flip;
    int spawn_x;
    int spawn_y;
    int delay;
    int energy;
    int start_delay;
} player_t;

typedef struct {
    dynamic_entity_t entity_comp;
    image2d image_comp;
} tile_t;

typedef struct {
    dynamic_entity_t entity_comp; 
    image2d image_comp;
    int collision;
    int move;
    int timer;
    int spawn_x;
    int spawn_y;
} enemy_t;

typedef struct {
    dynamic_entity_t entity_comp;
    image2d image_comp[10];
    int anim_index;
    char time;
    int increment;
} wallpaper_t;

typedef struct {
    dynamic_entity_t entity_comp;
    image2d image_comp;
} parallax_mts_t;

typedef struct {
    dynamic_entity_t entity_comp;
    image2d image_comp[2];
} health_t;

typedef struct {
    dynamic_entity_t entity_comp;
    image2d image_comp;
} over_t;

typedef struct {
    dynamic_entity_t entity_comp;
    image2d image_comp[9];
    int frame_dur;
    int anim_index;
    int increment;
    int spawn;
} particles_t;

char * map;

player_t player;
player_t player_cpy;

dynamic_entity_t * tiles;
tile_t tile_one;
tile_t tile_two;

enemy_t enemy;
enemy_t enemy_cpy;

int y;
int x;
int i;

int frame_start;
int frame_time; 
const int FRAME_DELAY = 1000 / GAME_FPS;

float dist_from_player;
float angle_from_player;


wallpaper_t wallpaper;
parallax_mts_t mountains;
over_t over;
int day_time;

health_t health;


vec2 tmp;
vec2 predicted;

particles_t particles;

float dist;

int despawnx;
int despawny;

char * coll_tiles;

/* Initializing game, mainly variables needed and the little engine behind the scenes */
int init_game() { 
    le_init();
    le_create_window("Systemical Destruction", SCREEN_W, SCREEN_H, &le_g_flags);
    le_create_renderer();

    frame_time = 0;
    frame_start = 0;

    init_dynamic_entity(&enemy.entity_comp, 0, 0, 9 * 3, 15 * 3, 3, 9);
    init_image2d(&enemy.image_comp, "Resources/enemy.png", 9 * 3, 15 * 3, 0, 0);
    enemy.collision = 0;
    enemy.move = 0;
    enemy.timer = 180;


    init_dynamic_entity(&player.entity_comp, 128, 128, 9 * 3, 15 * 3, 3, 9);
    init_image2d(&player.image_comp[0], "Resources/player_idle_0.png", 9 * 3, 15 * 3, 0, 0);
    init_image2d(&player.image_comp[1], "Resources/player_idle_1.png", 9 * 3, 15 * 3, 0, 0);
    init_image2d(&player.image_comp[2], "Resources/player_run_0.png",  9 * 3, 15 * 3, 0, 0);
    init_image2d(&player.image_comp[3], "Resources/player_run_1.png",  9 * 3, 15 * 3, 0, 0);
    init_image2d(&player.image_comp[4], "Resources/player_hit.png",  9 * 3, 15 * 3, 0, 0);

    init_dynamic_entity(&health.entity_comp, 0, 0, 20, 20, 0, 0);
    init_image2d(&health.image_comp[0], "Resources/heart.png", 20, 20, 0, 0);
    init_image2d(&health.image_comp[1], "Resources/heart_empty.png", 20, 20, 0, 0);

    player.y_momentum = 0;
    player.jump_timer = 0;
    player.actual_health = 5;
    player.health = 5;

    y = 0;
    x = 0;

    player.scroll.x = 0;
    player.scroll.y = 0;

    player.state = "idle";
    player.anim_index = 0;
    player.frame_dur = 20;
    player.dead = 0;
    player.energy = 10;
    player.delay = 100;

    init_dynamic_entity(&tile_one.entity_comp, 0, 0, TILE_SIZE, TILE_SIZE, 0, 0);
    init_image2d(&tile_one.image_comp, "Resources/tile_0.png", TILE_SIZE, TILE_SIZE, 0, 0);

    init_dynamic_entity(&tile_two.entity_comp, 0, 0, TILE_SIZE, TILE_SIZE, 0, 0);
    init_image2d(&tile_two.image_comp, "Resources/tile_1.png", TILE_SIZE, TILE_SIZE, 0, 0);

    init_dynamic_entity(&over.entity_comp, 0, 0, 640, 512, 0, 0);
    init_image2d(&over.image_comp, "Resources/over.png", 640, 512, 0, 0);

    map = load_text_file("Resources/map_0.txt");
    printf("%s\n", map);
   
    tiles = malloc(strlen(map) * sizeof(dynamic_entity_t));
    coll_tiles = (char *) malloc(strlen(map) * sizeof(dynamic_entity_t));

    for (i = 0; i < strlen(map); i++) {
        if (map[i] == '1' || map[i] == '2' || map[i] == '6') {
            tiles[i].pos.x = x * 32;
            tiles[i].pos.y = y * 32;
            tiles[i].w     = 32;
            tiles[i].h     = 32;
            coll_tiles[i]  = 'c';
        }

        if (map[i] == '3') {
            player.entity_comp.pos.x = x * 32;
            player.entity_comp.pos.y = y * 32;
            player.spawn_x = x * 32;
            player.spawn_y = y * 32;
        }

        if (map[i] == '4') {
            enemy.entity_comp.pos.x = x * 32;
            enemy.entity_comp.pos.y = y * 32;
            enemy.spawn_x = x * 32;
            enemy.spawn_y = y * 32;
        }

        if (map[i] != 'n') {
            x += 1;
        }

        if (map[i] == 'n') {
            x = 0;
            y += 1;
        }
        if (map[i] == '6') {
            despawnx = x * 32;
            despawny = y * 32;
            coll_tiles[i]  = 'c';
        }

        if (map[i] == '0') {
            tiles[i].pos.x = -500;
            tiles[i].pos.y = -500;
            tiles[i].w     = 32;
            tiles[i].h     = 32;
            coll_tiles[i]  = 'n';
        }

    }

    init_dynamic_entity(&wallpaper.entity_comp, 0, 0, 640, 512, 0, 0); 
    init_image2d(&wallpaper.image_comp[0], "Resources/cycle_0.png", 640, 512, 0, 0);
    init_image2d(&wallpaper.image_comp[1], "Resources/cycle_1.png", 640, 512, 0, 0);
    init_image2d(&wallpaper.image_comp[2], "Resources/cycle_2.png", 640, 512, 0, 0);
    init_image2d(&wallpaper.image_comp[3], "Resources/cycle_3.png", 640, 512, 0, 0);
    init_image2d(&wallpaper.image_comp[4], "Resources/cycle_4.png", 640, 512, 0, 0);
    init_image2d(&wallpaper.image_comp[5], "Resources/cycle_5.png", 640, 512, 0, 0);
    init_image2d(&wallpaper.image_comp[6], "Resources/cycle_6.png", 640, 512, 0, 0);
    init_image2d(&wallpaper.image_comp[7], "Resources/cycle_7.png", 640, 512, 0, 0);
    init_image2d(&wallpaper.image_comp[8], "Resources/cycle_8.png", 640, 512, 0, 0);
    init_image2d(&wallpaper.image_comp[9], "Resources/cycle_9.png", 640, 512, 0, 0);

    init_dynamic_entity(&mountains.entity_comp, -640, 0, 1280 * 2, 512, 0, 0); 
    init_image2d(&mountains.image_comp, "Resources/mountains.png", 1280 * 2, 512, 0, 0);

    wallpaper.anim_index = 0;
    wallpaper.increment = 1;
    wallpaper.time = 'd';

    day_time = 500;

    x = 0;
    y = 0;

    init_dynamic_entity(&particles.entity_comp, 0, 0, 160, 80, 0, 0);
    init_image2d(&particles.image_comp[0], "Resources/particle_0.png", 160, 80, 0, 0);
    init_image2d(&particles.image_comp[1], "Resources/particle_1.png", 160, 80, 0, 0);
    init_image2d(&particles.image_comp[2], "Resources/particle_2.png", 160, 80, 0, 0);
    init_image2d(&particles.image_comp[3], "Resources/particle_3.png", 160, 80, 0, 0);
    init_image2d(&particles.image_comp[4], "Resources/particle_4.png", 160, 80, 0, 0);
    init_image2d(&particles.image_comp[5], "Resources/particle_5.png", 160, 80, 0, 0);
    init_image2d(&particles.image_comp[6], "Resources/particle_6.png", 160, 80, 0, 0);
    init_image2d(&particles.image_comp[7], "Resources/particle_7.png", 160, 80, 0, 0);
    init_image2d(&particles.image_comp[8], "Resources/particle_8.png", 160, 80, 0, 0);
       
    particles.frame_dur = 10;
    particles.anim_index = 0;
    particles.increment = 1;
    particles.spawn = 0;

    player.entity_comp.pos.x = player.spawn_x;
    player.entity_comp.pos.y = player.spawn_y;

    return 1;
}

/* Game loop... */
void game_loop() {
    frame_start = SDL_GetTicks();
    
    player.scroll.x += (int) ((player.entity_comp.pos.x - player.scroll.x - ((SCREEN_W * 0.5) - (TILE_SIZE * 0.5))) / 10);
    player.scroll.y += (int) ((player.entity_comp.pos.y - player.scroll.y - ((SCREEN_H * 0.5) - (TILE_SIZE * 0.5))) / 10); 


    le_get_events();

    switch (le_g_event.type) {
        case SDL_QUIT:
            le_g_game_running = 0;
            break;

        case SDL_KEYDOWN:
            switch (le_g_event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                case SDL_SCANCODE_SPACE:
                    if (player.jump_timer < 3) {
                        player.y_momentum = -9;
                        if (mountains.entity_comp.pos.y != 10)
                            mountains.entity_comp.pos.y += 1;
                    }
                    
                    break;

                case SDL_SCANCODE_LEFT:
                case SDL_SCANCODE_A:
                    player.movement.moving_left = 1;
                    player.state = "run";
                    player.flip = SDL_FLIP_HORIZONTAL;
                    break;

                case SDL_SCANCODE_RIGHT:
                case SDL_SCANCODE_D:
                    player.movement.moving_right = 1;
                    player.state = "run";
                    player.flip = SDL_FLIP_NONE;
                    break;

                case SDL_SCANCODE_R:
                    if (player.dead) {
                        player.entity_comp.pos.x = player.spawn_x;
                        player.entity_comp.pos.y = player.spawn_y;
                        enemy.entity_comp.pos.x = enemy.spawn_x;
                        enemy.entity_comp.pos.y = enemy.spawn_y;
                        player.actual_health = player.health;
                        player.energy = 10;
                        player.dead = 0;
                    }


                case SDL_SCANCODE_DOWN:
                    if (player.energy > 0) {
                        particles.spawn = 1; 
                        tmp.x = player.entity_comp.pos.x;
                        tmp.y = player.entity_comp.pos.y;
                    }

                default:
                    break;
            }
            break;

        case SDL_KEYUP:
            switch (le_g_event.key.keysym.scancode) {
                case SDL_SCANCODE_LEFT:
                case SDL_SCANCODE_A:
                    player.movement.moving_left = 0;
                    break;

                case SDL_SCANCODE_RIGHT:
                case SDL_SCANCODE_D:
                    player.movement.moving_right = 0;
                    break;

                default:
                    break;
            }
            break;

        default:
            break; 
    } 

    /* Update */
    day_time -= 1;
    player.frame_dur -= 1;

    if (particles.spawn == 1) {
        particles.frame_dur -= 1;

        particles.entity_comp.pos.x = tmp.x - player.scroll.x;
        particles.entity_comp.pos.y = tmp.y - player.scroll.y;
        
        if (particles.frame_dur == 0) { 
            particles.anim_index += particles.increment;
            particles.frame_dur = 10;
        }

        if (particles.anim_index > 8) {
            particles.anim_index = 8;
            particles.increment = -1;
        }

        if (particles.anim_index < 0) {
            particles.anim_index = 0;
            particles.increment = 1;
            particles.spawn = 0;
            player.energy -= 1;
        }

    }



    if (day_time == 0) {
        wallpaper.anim_index += wallpaper.increment;
        day_time = 500;
    }

    if (wallpaper.anim_index > 9) {
        wallpaper.anim_index = 9;
        wallpaper.increment = -1;
    }

    if (wallpaper.anim_index < 0) {
        wallpaper.anim_index = 0;
        wallpaper.increment = 1;
    }

    if (player.movement.moving_left) {
        player.entity_comp.vel.x -= player.entity_comp.acc.x;
        mountains.entity_comp.pos.x += 1;
    }

    if (player.movement.moving_right) {
        player.entity_comp.vel.x += player.entity_comp.acc.x;
        mountains.entity_comp.pos.x -= 1;
    }

    if (player.entity_comp.bottom_collision) { 
        player.y_momentum = 0;
        player.jump_timer = 0;
        player.entity_comp.bottom_collision = 0;
        if (mountains.entity_comp.pos.y != 0) { 
            mountains.entity_comp.pos.y -= 1;
        }

    }

    else {
        player.jump_timer += 0.25;
        player.y_momentum += 1;
    }

    if (player.entity_comp.top_collision) {
        player.y_momentum += 3;
    }
    

    if (player.y_momentum >= 6) {
        player.y_momentum = 6;
    }

    if (enemy.move == 0) {
        enemy.timer -= 1;  
    }

    if (enemy.timer == 0) {
        enemy.move = 1;
        enemy.timer = 180;
    }

    predicted = player.entity_comp.pos;
    if (particles.spawn && (particles.entity_comp.pos.x - enemy.entity_comp.pos.x) < 100)
        predicted = particles.entity_comp.pos;
    
    dist_from_player = sqrt(pow((predicted.x - enemy.entity_comp.pos.x), 2) + pow((predicted.y - enemy.entity_comp.pos.y), 2));

    if (dist_from_player < 150 && enemy.move) {
        angle_from_player = atan2(predicted.y - enemy.entity_comp.pos.y, predicted.x - enemy.entity_comp.pos.x);
        enemy.entity_comp.pos.x += cos(angle_from_player) * 3;
        enemy.collision = collision_detect(player.entity_comp, enemy.entity_comp);
        if (enemy.collision) {
            player.entity_comp.vel.x += cos(angle_from_player) * 40;
            enemy.entity_comp.pos.x -= cos(angle_from_player) * 9;
            enemy.move = 0;
            player.anim_index = 4;
            player.state = "hit";
            player.frame_dur = 20;
            player.actual_health -= 1;
            
        }
    }



   
    player.entity_comp.vel.y += player.y_momentum;

    player.entity_comp.pos.x += player.entity_comp.vel.x;

    for (i = 0; i < strlen(map); i++) {
        if (map[i] != '0' && coll_tiles[i] != 'n' && coll_tiles[i] == 'c') {
            if (collision_detect(player.entity_comp, tiles[i])) {
                if (map[i] == '6') player.dead = 1;

                if (player.entity_comp.vel.x > 0) {
                    player.entity_comp.pos.x -= player.entity_comp.acc.x;
                    if (player.state[0] == 'h') player.entity_comp.pos.x -= cos(angle_from_player) * 41;
                    mountains.entity_comp.pos.x += 1;
                    player.entity_comp.right_collision = 1;
                    player.entity_comp.left_collision = 0;
                }

                else if (player.entity_comp.vel.x < 0) {
                    player.entity_comp.pos.x += player.entity_comp.acc.x;
                    if (player.state[0] == 'h') player.entity_comp.pos.x -= cos(angle_from_player) * 41;
                    mountains.entity_comp.pos.x -= 1;
                    player.entity_comp.right_collision = 0;
                    player.entity_comp.left_collision = 1;
                }

            }   
        }
    }

    player.entity_comp.pos.y += player.entity_comp.vel.y;
    for (i = 0; i < strlen(map); i++) {
        if (map[i] != '0' && coll_tiles[i] != 'n' && coll_tiles[i] == 'c') {
            if (collision_detect(player.entity_comp, tiles[i])) {
                if (map[i] == '6') player.dead = 1;
                if (player.entity_comp.vel.y > 0) {
                    player.entity_comp.pos.y -= player.y_momentum;
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


    if (player.entity_comp.vel.x == 0 && player.entity_comp.vel.y == 0 && player.state[0] != 'h') {
        player.state = "idle";
    }

    player.entity_comp.vel.x = 0;
    player.entity_comp.vel.y = 0; 

    if (player.frame_dur <= 0 && player.state[0] != 'h') {
        player.frame_dur = 20;
        player.anim_index += 1;

    }
      
    if (player.anim_index > 1 && player.state[0] != 'h') {
        player.anim_index = 0;
    }

    if (player.state[0] == 'h') {
        if (player.frame_dur == 0) {
            player.frame_dur = 20;
            player.anim_index = 0;
        }
    }

    if (player.actual_health <= 0) {
        player.dead = 1;
    }


    /* Render */
    le_clear_screen(0, 0, 10, 255);
    le_start_render();
	
	/* Real Rendering here */

    draw_dynamic_entity_img(&wallpaper.entity_comp, &wallpaper.image_comp[wallpaper.anim_index], 0.0f, NULL, 0);
    draw_dynamic_entity_img(&mountains.entity_comp, &mountains.image_comp, 0.0f, NULL, 0);



    for (i = 0; i < strlen(map); i++) {
        if (map[i] != '0' && coll_tiles[i] != 'n' && coll_tiles[i] == 'c') {
            if (map[i] == '2') {
                tile_one.entity_comp = tiles[i];
               
                if ((player.entity_comp.pos.x - (10 * TILE_SIZE) < tile_one.entity_comp.pos.x) && (tile_one.entity_comp.pos.x < player.entity_comp.pos.x + (10 * TILE_SIZE))) {
                    tile_one.entity_comp.pos.x -= player.scroll.x;
                    tile_one.entity_comp.pos.y -= player.scroll.y;
                    draw_dynamic_entity_img(&tile_one.entity_comp, &tile_one.image_comp, 0.0f, NULL, SDL_FLIP_NONE);
                }
                draw_dynamic_entity_rect(&tiles[i], 0, 0, 0, 0);
                

            }

            if (map[i] == '1') {
                tile_two.entity_comp = tiles[i];
                
                if ((player.entity_comp.pos.x - (10 * TILE_SIZE) < tile_two.entity_comp.pos.x) && (tile_two.entity_comp.pos.x < player.entity_comp.pos.x + (10 * TILE_SIZE))) {
                    tile_two.entity_comp.pos.x -= player.scroll.x;
                    tile_two.entity_comp.pos.y -= player.scroll.y;
                    draw_dynamic_entity_img(&tile_two.entity_comp, &tile_two.image_comp, 0.0f, NULL, SDL_FLIP_NONE);
                }
                draw_dynamic_entity_rect(&tiles[i], 0, 0, 0, 0);

            }

        }

    }



    enemy_cpy = enemy;

    enemy_cpy.entity_comp.pos.x -= player.scroll.x;
    enemy_cpy.entity_comp.pos.y -= player.scroll.y;

    draw_dynamic_entity_img(&enemy_cpy.entity_comp, &enemy_cpy.image_comp, 0.0f, NULL, 0);


    player_cpy = player;

    player_cpy.entity_comp.pos.x -= player.scroll.x;
    player_cpy.entity_comp.pos.y -= player.scroll.y;


    if (player.state[0] == 'r') {
	    draw_dynamic_entity_img(&player_cpy.entity_comp, &player_cpy.image_comp[player.anim_index + 2], 0.0f, NULL, player.flip);
    }
    else {
	    draw_dynamic_entity_img(&player_cpy.entity_comp, &player_cpy.image_comp[player.anim_index], 0.0f, NULL, player.flip);
    }

    if (particles.spawn)
        draw_dynamic_entity_img(&particles.entity_comp, &particles.image_comp[particles.anim_index], 0.0f, NULL, SDL_FLIP_NONE);

    i = 0;
    for (i = 0; i < player.health; i++) {
        health.entity_comp.pos.x = (1 + i) * 30;
        health.entity_comp.pos.y = 50;

        draw_dynamic_entity_img(&health.entity_comp, &health.image_comp[1], 0.0f, NULL, SDL_FLIP_NONE);
    }
    i = 0;

    i = 0;
    for (i = 0; i < player.actual_health; i++) {
        health.entity_comp.pos.x = (1 + i) * 30;
        health.entity_comp.pos.y = 50;

        draw_dynamic_entity_img(&health.entity_comp, &health.image_comp[0], 0.0f, NULL, SDL_FLIP_NONE);
    }
    i = 0;

    if (player.dead) {
        draw_dynamic_entity_img(&over.entity_comp, &over.image_comp, 0.0f, NULL, 0.0f);        
    }
    
    le_end_render();

    frame_time = SDL_GetTicks() - frame_start;
    if (frame_time < FRAME_DELAY) {
        le_delay(FRAME_DELAY - frame_time); 
    }
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
