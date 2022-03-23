#include "../engine/include/engine.h"

/*
  * Note: Globals are being used because of Emscripten  
  * i32 init_game() -> initializes games and global variables
  * emp game_loop() -> game_loop: 
  *      frame start
  *      event handling
  *      updating
  *      rendering
  *      frame end & delayer
  *      repeat
  * i32 main()      -> main function for compiler
*/

// Basic Game Setup
#define SCREEN_W  640 
#define SCREEN_H  360
#define GAME_FPS  60 
#define VSYNC     1

SDL_Event event;


// Game Setup
#define TILE_SIZE 44


le_sfx_t jump_sfx;
le_sfx_t quit_sfx;
le_sfx_t dash_sfx;
le_sfx_t coin_sfx;

typedef struct {
    i08 moving_left;
    i08 moving_right;
    i08 moving_up;
    i08 moving_down;
} movement_t;

typedef struct {
    le_vec2_t     scroll;
    movement_t    movement;
    f32           y_momentum;
    f32           air_timer;
    le_entity_t   ent_comp;
    le_img_t *    img_comp; 
    le_str        state;
    i16           frame_index;
    i32           frame_dur;
    i32           d_frame_dur;
    i32           frames;
    f64           d_health;
    f64           health;
    i08           dead;
    f64           angle;
    i08           flip_horizontal;
    le_vec2_t     spawn_pos;
    i32           dash_timer_start;
    i32           d_d_dash_timer;
    i32           d_dash_timer;
    i32           dash_timer;
    f64           dash_x;
    i32           score;
} player_t;

emp init_player_t(
                    player_t * p, 
                    i32        spawn_x, i32 spawn_y, 
                    i32        acc_x, i32 acc_y, 
                    i32        w, i32 h,
                    le_str *   path,
                    i32        frames,
                    i32        tex_x, i32 tex_y,
                    le_str     state,
                    i32        frame_dur,
                    i32        d_health,
                    f64        angle,
                    i08        flip_horizontal
                 ) {
    p -> scroll = le_v2(0, 0);
    p -> movement.moving_down = 0;
    p -> movement.moving_up = 0;
    p -> movement.moving_left = 0;
    p -> movement.moving_right = 0;
    p -> y_momentum = 0;
    p -> air_timer = 0;
    p -> ent_comp = mk_le_entity_t(le_v2(spawn_x, spawn_y), le_v2(0, 0), le_v2(acc_x, acc_y), w, h);
    p -> img_comp = calloc(frames, sizeof(le_img_t));
    for (int i = 0; i < frames; i++) {
        p -> img_comp[i] = mk_le_img_t(path[i], tex_x, tex_y, w, h);
    }
    p -> frames = frames;
    p -> state = state;
    p -> frame_index = 0;
    p -> d_frame_dur = 30;
    p -> frame_dur = 30;
    p -> d_health = d_health;
    p -> health = d_health;
    p -> dead = 0;
    p -> angle = angle;
    p -> flip_horizontal = flip_horizontal;
    p -> spawn_pos = le_v2(spawn_x, spawn_y);
    p -> dash_timer_start = 0;
    p -> d_d_dash_timer = 120;
    p -> d_dash_timer = 120;
    p -> dash_timer = 120;
    p -> dash_x = 0;
    p -> score = 0;
}

emp calc_scroll_player_t(player_t * p) {
    p -> scroll.x += (int) ((p -> ent_comp.pos.x - p -> scroll.x - ((SCREEN_W * 0.5) - (TILE_SIZE * 0.5))) / 10);
    p -> scroll.y += (int) ((p -> ent_comp.pos.y - p -> scroll.y - ((SCREEN_H * 0.5) - (TILE_SIZE * 0.5))) / 10); 
}

emp draw_player_t(player_t * p) {
    le_entity_t cpy;
    cpy = p -> ent_comp;
    
    cpy.pos.x -= p -> scroll.x;
    cpy.pos.y -= p -> scroll.y;

    p -> frame_dur -= 1;
    if (p -> frame_dur <= 0) {
        p -> frame_index += 1;
        p -> frame_dur = p -> d_frame_dur;
    }

    if (p -> frame_index >= p -> frames) {
        p -> frame_index = 0;
    }
    
    if (p -> flip_horizontal == 0) {
        le_draw_le_entity_t_ex(
                               &cpy, 
                               &p -> img_comp[p -> frame_index],
                               p -> angle, 
                               NULL, 
                               SDL_FLIP_NONE
                              ); 
    }
    if (p -> flip_horizontal == 1) {
        le_draw_le_entity_t_ex(
                               &cpy, 
                               &p -> img_comp[p -> frame_index],
                               p -> angle, 
                               NULL, 
                               SDL_FLIP_HORIZONTAL
                              ); 
    }
}


i08 check_player_t_is_at_end(player_t * p, le_vec2_t * v) {
    le_entity_t cpy = mk_le_entity_t(*v, le_v2(0, 0), le_v2(0, 0), TILE_SIZE, TILE_SIZE);
    if (collision_detect(p -> ent_comp, cpy)) {
        return 1; 
    }
    return 0;
}


typedef struct {
    le_entity_t   ent_comp;
    le_img_t      img_comp;
} parallax_bg_t;

emp init_parallax_bg_t(parallax_bg_t * pbgt, le_str path) {
    pbgt -> ent_comp = mk_le_entity_t(le_v2(-320, 0), le_v2_zero(), le_v2_zero(), SCREEN_W * 2, SCREEN_H);
    pbgt -> img_comp = mk_le_img_t(path, 0, 0, SCREEN_W * 4, SCREEN_H);
}

emp draw_parallax_bg_t(parallax_bg_t * pbgt) {
    le_draw_le_entity_t(&pbgt -> ent_comp, &pbgt -> img_comp);
}


typedef struct {
    le_entity_t   ent_comp; 
    le_img_t      img_comp;
    le_vec2_t     spawn_pos; 
    i08           collision;
} enemy_t;

emp init_enemy_t(
                    enemy_t * e, 
                    le_str      path,
                    i32         tex_x, i32 tex_y,
                    i32         spawn_x, i32 spawn_y
                 ) {
    e -> img_comp = mk_le_img_t(path, tex_x, tex_y, TILE_SIZE, TILE_SIZE);
    e -> ent_comp = mk_le_entity_t(le_v2(spawn_x, spawn_y), le_v2_zero(), le_v2(6, 0), TILE_SIZE, TILE_SIZE);
    e -> spawn_pos = le_v2(spawn_x, spawn_y);
    e -> collision = 0;
}

emp check_enemy_collisions_t(enemy_t * e, player_t * p) {
    if (collision_detect(p -> ent_comp, e -> ent_comp)) {
        p -> dead = 1;   
    }
}

emp draw_enemy_t(enemy_t * e, player_t * p) {
    le_entity_t cpy;
    cpy = e -> ent_comp;
        
    cpy.pos.x -= p -> scroll.x;
    cpy.pos.y -= p -> scroll.y;

    le_draw_le_entity_t(&cpy, &e -> img_comp);
}

typedef struct {
    le_entity_t   ent_comp; 
    le_img_t      img_comp;
    le_vec2_t     spawn_pos; 
    i08           collision;
    f64           increment;
} coin_t;

emp init_coin_t(coin_t * c, le_str path, i32 tex_x, i32 tex_y, i32 spawn_x, i32 spawn_y, i32 coins_num) {
    c -> ent_comp = mk_le_entity_t(le_v2(spawn_x, spawn_y), le_v2_zero(), le_v2_zero(), TILE_SIZE, TILE_SIZE);
    c -> img_comp = mk_le_img_t(path, tex_x, tex_y, TILE_SIZE, TILE_SIZE);
    c -> spawn_pos = le_v2(spawn_x, spawn_y);
    c -> collision = 0;
    c -> increment = 120.0f / coins_num;
}

emp check_coin_collisions_t(coin_t * c, player_t * p, i32 coins_num) {
    c -> increment = 120.0f / coins_num;
    if (collision_detect(p -> ent_comp, c -> ent_comp)) {
        p -> score += c -> increment;
        c -> ent_comp.pos.y = -10000;
        le_sfx_t_play(coin_sfx);
    }
}

emp draw_coin_t(coin_t * c, player_t * p) {
    le_entity_t cpy;
    cpy = c -> ent_comp;
        
    cpy.pos.x -= p -> scroll.x;
    cpy.pos.y -= p -> scroll.y;  

    le_draw_le_entity_t(&cpy, &c -> img_comp);
}

typedef struct {
    le_str        tilemap;
    le_str        c_tiles;
    le_entity_t * m_tiles;
    i32           len;
} tiles_t;


// Hardcoded:
//  0 = air
//  1 = ground tile
//  2 = dirt tile
//  3 = map end
//  4 = player spawn
//  5 = enemy spawn
//  6 = coin spawn


emp init_tiles_t(tiles_t * t, le_str path, player_t * p, le_vec2_t * end_pos, enemy_t * enemy, coin_t * coins) {
    t -> tilemap = le_load_file(path);
    t -> len     = le_str_length(t -> tilemap);
    t -> c_tiles = calloc(t -> len, sizeof(char));
    t -> m_tiles = calloc(t -> len, sizeof(le_entity_t));

    i32 i = 0;
    i32 x = 0;
    i32 y = 0;
    i32 enemy_index = 0;
    i32 coin_index = 0;

    for (i = 0; i < t -> len; i++) {
        if (t -> tilemap[i] == '0') {
            t -> c_tiles[i] = 'n'; 
        }

        if (t -> tilemap[i] != '0' 
         && t -> tilemap[i] != 'n'
         && t -> tilemap[i] <= '3') {
            t -> c_tiles[i] = 'c'; 
        }

        if ((t -> tilemap[i] == '1') 
         || (t -> tilemap[i] != 'n'
         &&  t -> tilemap[i] <= '2')) {
            t -> m_tiles[i].pos.x = x * TILE_SIZE;
            t -> m_tiles[i].pos.y = y * TILE_SIZE;
            t -> m_tiles[i].w = TILE_SIZE;
            t -> m_tiles[i].h = TILE_SIZE;
        }

        if (t -> tilemap[i] == '3') {
            end_pos -> x = x * TILE_SIZE;
            end_pos -> y = y * TILE_SIZE;
        }

        if (t -> tilemap[i] == '4') {
            p -> ent_comp.pos.x = x * TILE_SIZE;    
            p -> ent_comp.pos.y = y * TILE_SIZE;    
            p -> spawn_pos.x = x * TILE_SIZE;
            p -> spawn_pos.y = y * TILE_SIZE;
           
        }

        if (t -> tilemap[i] == '5') {
            init_enemy_t(&enemy[enemy_index], "res/enemy.png", 0, 0, x * TILE_SIZE, y * TILE_SIZE);    
            enemy_index += 1;
        }

        if (t -> tilemap[i] == '6') {
            coin_index += 1;
            init_coin_t(&coins[coin_index - 1], "res/coin.png", 0, 0, x * TILE_SIZE, y * TILE_SIZE, coin_index);
        }

        if (t -> tilemap[i] != 'n') {
            x += 1;
        }

        if (t -> tilemap[i] == 'n') {
            x  = 0;
            y += 1;
        }

    }

}

emp draw_tiles_t(tiles_t * t, le_img_t * tile_one, le_img_t * tile_two, player_t * p) {
    for (int i = 0; i < t -> len; i++) {
        if (t -> tilemap[i] != '0') {
            if (t -> tilemap[i] == '1') {
                le_entity_t cpy = t -> m_tiles[i];
               
                if ((p -> ent_comp.pos.x - (10 * TILE_SIZE) < cpy.pos.x) && (cpy.pos.x < p -> ent_comp.pos.x + (10 * TILE_SIZE))) {
                    cpy.pos.x -= p -> scroll.x;
                    cpy.pos.y -= p -> scroll.y;
                    le_draw_le_entity_t(&cpy, tile_one);
                }
                

            }

            if (t -> tilemap[i] == '2') {
                le_entity_t cpy = t -> m_tiles[i];
                
                if ((p -> ent_comp.pos.x - (10 * TILE_SIZE) < cpy.pos.x) && (cpy.pos.x < p -> ent_comp.pos.x + (10 * TILE_SIZE))) {
                    cpy.pos.x -= p -> scroll.x;
                    cpy.pos.y -= p -> scroll.y;
                    le_draw_le_entity_t(&cpy, tile_two);
                }

            }
        }
    }
}

emp check_tile_t_collisions(tiles_t * t, parallax_bg_t * pbgt, player_t * p) {
    p -> ent_comp.vel.y += p -> y_momentum;
    p -> ent_comp.pos.x += p -> ent_comp.vel.x;

    
    for (int i = 0; i < t -> len; i++) {
        if (t -> tilemap[i] != '0' && t -> c_tiles[i] == 'c') {
            if (collision_detect(p -> ent_comp, t -> m_tiles[i])) {
                if (p -> ent_comp.vel.x > 0) {
                    p -> ent_comp.pos.x -= p -> ent_comp.acc.x;
                    p -> ent_comp.coll_r = 1;
                    p -> ent_comp.coll_l = 0;
                    if (p -> dash_x != 0) { p -> ent_comp.pos.x -= 10; };
                    pbgt -> ent_comp.pos.x += 1;
                }

                else if (p -> ent_comp.vel.x < 0) {
                    p -> ent_comp.pos.x += p -> ent_comp.acc.x;
                    p -> ent_comp.coll_r = 0;
                    p -> ent_comp.coll_l = 1;
                    if (p -> dash_x != 0) { p -> ent_comp.pos.x += 10; };
                    pbgt -> ent_comp.pos.x -= 1;
                }               
            }
        }
    }


    p -> ent_comp.pos.y += p -> ent_comp.vel.y;
    for (int i = 0; i < t -> len; i++) {
        if (t -> tilemap[i] != '0' && t -> c_tiles[i] == 'c') {
            if (collision_detect(p -> ent_comp, t -> m_tiles[i])) {
                if (p -> ent_comp.vel.y > 0) {
                    p -> ent_comp.pos.y -= p -> y_momentum;
                    p -> ent_comp.coll_b = 1;
                    p -> ent_comp.coll_t = 0;
                }

                else if (p -> ent_comp.vel.y < 0) {
                    p -> ent_comp.pos.y += fabsf(p -> y_momentum);
                    p -> ent_comp.coll_b = 0;
                    p -> ent_comp.coll_t = 1;
                }               
            }
        }
    }

}

le_img_t  tile_img = {0};
tiles_t   tiles_lvls[4] = {0};
le_vec2_t end_pos_lvls[4] = {0};
i32       curr_level;

enemy_t   enemies_lvls_one[2] = {0};
enemy_t   enemies_lvls_two[5] = {0};
enemy_t   enemies_lvls_three[7] = {0};
enemy_t   enemies_lvls_four[10] = {0};

coin_t    coins_lvls_one[2] = {0};
coin_t    coins_lvls_two[5] = {0};
coin_t    coins_lvls_three[7] = {0};
coin_t    coins_lvls_four[10] = {0};

i08 new_lvl;

le_str    player_imgs[2];
player_t  player = {0};

le_str        parallax_img;
parallax_bg_t parallax;

le_entity_t health;
f64         r_h;
le_entity_t dash;
le_entity_t score;

i08 game_menu;
i08 game_play;
i08 game_over;
i08 game_end;

le_img_t logo;


/* Initialize Game */
i32 init_game() { 
    le_init("Game", SCREEN_W, SCREEN_H, GAME_FPS, 1);

    curr_level = 0;
    tile_img = mk_le_img_t("res/tile.png", 0, 0, TILE_SIZE, TILE_SIZE);
    init_tiles_t(&tiles_lvls[curr_level], "res/map_0.txt", &player, &end_pos_lvls[curr_level], enemies_lvls_one, coins_lvls_one);


    player_imgs[0] = "res/player_0.png"; 
    player_imgs[1] = "res/player_1.png";
    init_player_t(&player, player.spawn_pos.x, player.spawn_pos.y, 8, 0, TILE_SIZE, TILE_SIZE, player_imgs, 2, 0, 0, "idle", 80, 360, 0.0f, 0);

    parallax_img = "res/mountains.png";
    init_parallax_bg_t(&parallax, parallax_img);

    health = mk_le_entity_t(le_v2(20, 20), le_v2_zero(), le_v2_zero(), 360, 10);
    r_h = 360;
    dash = mk_le_entity_t(le_v2(20, 30), le_v2_zero(), le_v2_zero(), 120, 10);
    score = mk_le_entity_t(le_v2(20, 40), le_v2_zero(), le_v2_zero(), 0, 10);

    new_lvl = 0;

    game_menu = 1;
    game_play = 0;
    game_over = 0;
    game_end = 0;

    logo = mk_le_img_t("res/logo.png", 0, 0, 88, 88);

    jump_sfx = mk_le_sfx_t("res/blip.wav");
    quit_sfx = mk_le_sfx_t("res/quit.wav");
    dash_sfx = mk_le_sfx_t("res/dash.wav");
    coin_sfx = mk_le_sfx_t("res/coin.wav");

    return 1;
}

/* Game Menu Loop */
emp game_menu_loop() {
    le_start_frame();

    /* Event Handling */
    event = le_events();
    le_quit_check(&event);
    switch (event.type) {
        case SDL_KEYDOWN:
            le_sfx_t_play(quit_sfx);
            game_play = 1;
            game_menu = 0;
            break;

        default:
            break;
    } 

    /* Updating */
    le_entity_t screen = mk_le_entity_t(le_v2(SCREEN_W * 0.5 - 100, SCREEN_H * 0.5 - 100), le_v2(0, 0), le_v2(0, 0), 200, 100);
    le_entity_t keyboard = mk_le_entity_t(le_v2(SCREEN_W * 0.5 - 100, SCREEN_H * 0.5), le_v2(0, 0), le_v2(0, 0), 200, 100);
    le_entity_t key = mk_le_entity_t(le_v2(SCREEN_W * 0.5 - 55, SCREEN_H * 0.5 + 70 + rand() % 3), le_v2(0, 0), le_v2(0, 0), 110, 20);

    /* Rendering */
    le_cls(10, 5, 40, 255);
    le_render_start();
   
        le_draw_le_entity_t_rect(&screen, 0, 0, 0, 255);
        le_draw_le_entity_t_rect(&keyboard, 255, 255, 255, 255);
        le_draw_le_entity_t_rect(&key, 23, 25, 0, 255);
        le_draw_le_img_t(&logo, SCREEN_W * 0.5 - 44, SCREEN_H * 0.5 - 88);


    le_render_end();


    /* FPS */
    le_end_frame();
}




/* Game Play Loop */
emp game_play_loop() {
    le_start_frame();

    /* Event Handling */
    event = le_events();
    le_quit_check(&event);
    switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_RIGHT:
                    player.movement.moving_right = 1;
                    player.state = "r";
                    player.flip_horizontal = 0;
                    break;

                case SDL_SCANCODE_LEFT:
                    player.movement.moving_left = 1;
                    player.state = "l";
                    player.flip_horizontal = 1;
                    break;

                case SDL_SCANCODE_UP:
                    if (player.air_timer < 2) {
                        player.y_momentum = -12;
                        le_sfx_t_play(jump_sfx);
                        if (parallax.ent_comp.pos.y != 10) {
                            parallax.ent_comp.pos.y += 1;
                        }
                    }
                    break;

                case SDL_SCANCODE_DOWN:
                    if (player.dash_timer > 0) {
                        if (player.state[0] == 'r' && player.movement.moving_right) {
                            player.ent_comp.pos.x += 10;
                            player.dash_x += 10;
                        }
                        if (player.state[0] == 'l' && player.movement.moving_left) {
                            player.ent_comp.pos.x -= 10;
                            player.dash_x -= 10;
                        }
                        le_sfx_t_play(dash_sfx);
                        player.dash_timer -= 1;
                        player.dash_timer_start = 1;
                    }
                    break;

                le_default_event_check;
            }
            break;

        case SDL_KEYUP:
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_RIGHT:
                    player.movement.moving_right = 0;
                    break;

                case SDL_SCANCODE_LEFT:
                    player.movement.moving_left = 0;
                    break;

                case SDL_SCANCODE_DOWN:
                    player.dash_x = 0;
                    break;

                le_default_event_check;
            }
            break;
        le_default_event_check;


    } 

    /* Updating */
    player.ent_comp.vel = le_v2_zero(); 
    calc_scroll_player_t(&player);
    
    if (player.dash_timer <= 0) {
        player.d_dash_timer -= 1;
    }

    if (player.d_dash_timer <= 0) {
        player.d_dash_timer = player.d_d_dash_timer;
        player.dash_timer = player.d_dash_timer;
    }

    if (player.movement.moving_left) {
        player.ent_comp.vel.x -= player.ent_comp.acc.x; 
        parallax.ent_comp.pos.x += 1;
    }

    if (player.movement.moving_right) {
        player.ent_comp.vel.x += player.ent_comp.acc.x; 
        parallax.ent_comp.pos.x -= 1;
    }

    if (player.ent_comp.coll_b) {
        player.y_momentum = 0;
        player.air_timer = 0;
        player.ent_comp.coll_b = 0;
        if (parallax.ent_comp.pos.y != 0) {
            parallax.ent_comp.pos.y -= 1;
        }
    }

    else {
        player.air_timer += 0.25;
        player.y_momentum += 0.75;
    }

    if (player.ent_comp.coll_t) {
        player.y_momentum = 0;
        player.y_momentum += 3;
    }


    if (player.y_momentum >= 6) {
        player.y_momentum = 6;
    }




    check_tile_t_collisions(&tiles_lvls[curr_level], &parallax, &player);
   
    switch (curr_level) {
        case 0:
            for (int i = 0; i < 2; i++) {
                check_coin_collisions_t(&coins_lvls_one[i], &player, 2);
                check_enemy_collisions_t(&enemies_lvls_one[i], &player);
            }
            break;

        case 1:
            for (int i = 0; i < 5; i++) {
                check_coin_collisions_t(&coins_lvls_two[i], &player, 5);
                check_enemy_collisions_t(&enemies_lvls_two[i], &player);
            }
            break;
        
        case 2:
            for (int i = 0; i < 7; i++) {
                check_coin_collisions_t(&coins_lvls_three[i], &player, 7);
                check_enemy_collisions_t(&enemies_lvls_three[i], &player);
            }
            break;

        case 3:
            for (int i = 0; i < 10; i++) {
                check_coin_collisions_t(&coins_lvls_four[i], &player, 10);
                check_enemy_collisions_t(&enemies_lvls_four[i], &player);
            }
            break;

        default:
            break;
    }

    if (check_player_t_is_at_end(&player, &end_pos_lvls[curr_level])) {
        new_lvl = 1;
        curr_level += 1;
    }

    if (curr_level > 3) {
        game_end = 1;
        game_play = 0;
        curr_level = -1;
    }

    if (new_lvl && curr_level > -1) {
        switch(curr_level) {
            case 1:
                init_tiles_t(&tiles_lvls[1], "res/map_1.txt", &player, &end_pos_lvls[1], enemies_lvls_two, coins_lvls_two);
                break;  

            case 2:
                init_tiles_t(&tiles_lvls[2], "res/map_2.txt", &player, &end_pos_lvls[2], enemies_lvls_three, coins_lvls_three);
                break;  

            case 3:
                init_tiles_t(&tiles_lvls[3], "res/map_3.txt", &player, &end_pos_lvls[3], enemies_lvls_four, coins_lvls_four);
                break;  

            default:
                break;  
        }


        init_player_t(&player, player.spawn_pos.x, player.spawn_pos.y, 8, 0, TILE_SIZE, TILE_SIZE, player_imgs, 1, 0, 0, "idle", 80, 360, 0.0f, 0);
        init_parallax_bg_t(&parallax, parallax_img);


        health = mk_le_entity_t(le_v2(20, 20), le_v2_zero(), le_v2_zero(), 360, 10);
        r_h = 360;
        dash = mk_le_entity_t(le_v2(20, 30), le_v2_zero(), le_v2_zero(), 120, 10);
        score = mk_le_entity_t(le_v2(20, 40), le_v2_zero(), le_v2_zero(), 0, 10);
        new_lvl = 0;
    }

    dash.w = player.dash_timer;
    score.w = player.score;
    r_h     -= 0.5;
    health.w = r_h;

    if (health.w <= 1) {
        player.dead = 1;
    }
    if (player.dead) {
        game_over = 1;
        game_play = 0;
    }


    /* Rendering */
    le_cls(0, 160, 151, 105);
    le_render_start();

        draw_parallax_bg_t(&parallax);

      
        switch (curr_level) {
            case 0:
                for (int i = 0; i < 2; i++) {
                    draw_coin_t(&coins_lvls_one[i], &player);
                    draw_enemy_t(&enemies_lvls_one[i], &player);
                }
                break;

            case 1:
                for (int i = 0; i < 5; i++) {
                    draw_coin_t(&coins_lvls_two[i], &player);
                    draw_enemy_t(&enemies_lvls_two[i], &player);
                }
                break;
            
            case 2:
                for (int i = 0; i < 7; i++) {
                    draw_coin_t(&coins_lvls_three[i], &player);
                    draw_enemy_t(&enemies_lvls_three[i], &player);
                }
                break;

            case 3:
                for (int i = 0; i < 10; i++) {
                    draw_coin_t(&coins_lvls_four[i], &player);
                    draw_enemy_t(&enemies_lvls_four[i], &player);
                }
                break;

            default:
                break;
        }


        draw_tiles_t(&tiles_lvls[curr_level], &tile_img, &tile_img, &player);
        draw_player_t(&player);
        le_draw_le_entity_t_rect(&health, 255, 000, 000, 255);
        le_draw_le_entity_t_rect(&dash, 255, 255, 255, 255);
        le_draw_le_entity_t_rect(&score, 255, 000, 255, 255);   


    le_render_end();


    /* FPS */
    le_end_frame();
}

emp game_loop() {
    if (game_menu) {
        game_menu_loop();
    }

    if (game_play) {
        game_play_loop();
    }

    if (game_over || game_end) {
        set_le_on(0);
    }

}

/* Main Function */
i32 main(le_arg) {
    if (!init_game()) {
        le_print_info("Error Initializing Game!", NULL);
    }

    le_print_info("Successfully Initialized Game", NULL);
    le_game_loop(game_loop);
    le_uninit();

    return 0;
}
