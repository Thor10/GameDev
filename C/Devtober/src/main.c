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

#define SCREEN_W  640 
#define SCREEN_H  480
#define GAME_FPS  60 
#define PROFILER  10
#define ENEMIES_NUM 10

le_events event = {0};
le_prof_entry_t prof_entries[PROFILER] = {0};

le_img_t wall_img = {0};
le_entity_t wall_one = {0};
le_entity_t wall_two = {0};

le_img_t player_img = {0};
le_img_t player_img_smaller = {0};
le_entity_t player  = {0};
le_entity_t player_smaller = {0};
f64 player_rotation = 0.0f;
i08 player_left = 0;
i08 player_right = 0;
i08 player_down = 0;
i08 player_up =   0;
i08 player_coll_with_wall = 0;
i32 player_cooldown = 0;
le_particles_t player_particles = {0};
i32 player_particles_cooldown = 0;
i32 player_particles_dead = 0;

le_img_t enemy_img = {0};
le_entity_t enemies[ENEMIES_NUM] = {0};

u32 game_menu = 1;
u32 game_over = 0;
u32 game_play = 0;

le_ui_label_t go_to_play   = {0};
le_entity_t   go_to_play_ent = {0};
le_ui_label_t quit_game    = {0};
le_entity_t   quit_game_ent = {0};
le_ui_label_t back_to_menu = {0};
le_entity_t back_to_menu_ent = {0};
u64 score = 0;
u64 h_score = 0;
le_str h_score_str;
le_ui_label_t h_score_label = {0};
le_ui_label_t h_score_actual_label = {0};

le_sfx_t play = {0};
le_sfx_t explosion = {0};
le_sfx_t move = {0};

//
// USE PRE-RENDERED TEXT 
// SAVED IN A PNG FILE 
// Hash table for pngs
//  Looks up score in hash table

/* Initialize Game */
i32 init_game() { 

    le_init("Game", SCREEN_W, SCREEN_H, GAME_FPS, 1, 0);

    play = mk_le_sfx_t("res/hit.wav");
    explosion = mk_le_sfx_t("res/b_hit.wav");
    move = mk_le_sfx_t("res/move.wawavv");



    wall_img = le_create_img_t(30, 40, 10, 255, 100, 640);
    wall_one = mk_le_entity_t(le_v2(0, 0), le_v2(0, 0), le_v2(0, 0), 
                              100, 640);
    wall_two = mk_le_entity_t(le_v2(545, 0), le_v2(0, 0), le_v2(0, 0), 
                              100, 640);

    player_img = le_create_img_t(255, 255, 255, 200, 32, 32);
    player_img_smaller = le_create_img_t(0, 0, 0, 255, 16, 16);
    player = mk_le_entity_t(le_v2(320, 240), le_v2(0, 0), le_v2(10, -10), 
                            32, 32);
    player_smaller = mk_le_entity_t(le_v2(player.pos.x + 8, 
                                    player.pos.y + 8), 
                                    le_v2(0, 0), le_v2(10, -10), 
                                    16, 16);

    player_particles = create_le_particles_t("res/particle.png", 8, 16, 
                       le_v2(player.pos.x + player.w / 2, 
                        player.pos.y + player.h / 2), 0, 0, 0, 0, 255, 255, 100);

    enemy_img = le_create_img_t(134, 200, 200, 100, 32, 32);
    for (int i = 0; i < ENEMIES_NUM; i++) {
        enemies[i] = mk_le_entity_t(le_v2(randnum(0, 640), randnum(0, 100)), 
                                    le_v2(0, 0), 
                                    le_v2(randnum(-2, 2), randnum(1, 3)), 32, 32);
    }

    go_to_play = mk_le_ui_label_t(320, 240, "Play", "res/Sans.ttf", 
                                  255, 255, 255, 255, 40); 

    go_to_play_ent = mk_le_entity_t(le_v2(320, 240), le_v2(0, 0), le_v2(0, 0), 
                                    go_to_play.pos.w, go_to_play.pos.h);

    quit_game    = mk_le_ui_label_t(320, 280, "Quit", "res/Sans.ttf", 255, 
                                    255, 255, 255, 40);

    quit_game_ent = mk_le_entity_t(le_v2(320, 280), le_v2(0, 0), le_v2(0, 0), 
                                    quit_game.pos.w, quit_game.pos.h);

    back_to_menu = mk_le_ui_label_t(320, 240, "Back To Menu", "res/Sans.ttf", 255, 
                                    255, 255, 255, 40);

    back_to_menu_ent = mk_le_entity_t(le_v2(320, 240), le_v2(0, 0), le_v2(0, 0),
                                      back_to_menu.pos.w, back_to_menu.pos.h);


    go_to_play.pos.x = go_to_play.pos.x - go_to_play.pos.w / 2;
    quit_game.pos.x = quit_game.pos.x - quit_game.pos.w / 2;
    back_to_menu.pos.x = back_to_menu.pos.x - back_to_menu.pos.w / 2;
    h_score_str = i32_to_le_str(h_score);


    h_score_label = mk_le_ui_label_t(15, 10, "High Score: ", "res/Sans.ttf", 255, 
                                     255, 255, 255, 40);
    h_score_actual_label = mk_le_ui_label_t(160, 10, h_score_str, "res/Sans.ttf", 255,
                                            255, 255, 255, 40);
    h_score_actual_label.label_text = realloc(h_score_actual_label.label_text, 150);

    return 1;
}

emp game_menu_loop() {

    le_start_frame();

    event = le_get_event(0);
    
    le_quit_check(&event);

    int x = 0;
    int y = 0;
    switch (event.type) {

        case SDL_MOUSEBUTTONDOWN:
            SDL_GetMouseState(&x, &y);
             
        default:
            break;
    }

    score = 0;
    player = mk_le_entity_t(le_v2(320, 240), le_v2(0, 0), le_v2(10, -10), 
                            32, 32);
    player_smaller = mk_le_entity_t(le_v2(player.pos.x + 8, 
                                    player.pos.y + 8), 
                                    le_v2(0, 0), le_v2(10, -10), 
                                    16, 16);
    for (int i = 0; i < ENEMIES_NUM; i++) {
        enemies[i] = mk_le_entity_t(le_v2(randnum(0, 640), randnum(0, 100)), 
                                    le_v2(0, 0), 
                                    le_v2(randnum(-2, 2), randnum(1, 3)), 32, 32);
    }

    if (x > 300 && x < go_to_play.pos.x + go_to_play.pos.w 
     && y > 240 && y < go_to_play.pos.y + go_to_play.pos.h) {
        le_sfx_t_play(play);
        game_menu = 0;
        game_play = 1;
        
    }

    if (x > 300 && x < quit_game.pos.x + quit_game.pos.w 
     && y > 280 && y < quit_game.pos.y + quit_game.pos.h) {
        game_menu = 0;
        set_le_on(0); 
    }

    player_coll_with_wall = 0;

    player_left = 0;
    player_right = 0;
    player_up = 0;
    player_down = 0;
     

    le_cls(47, 17, 155, 255);

        le_draw_le_ui_label_t(&go_to_play);
        le_draw_le_ui_label_t(&quit_game);
                

     /* FPS */
    le_end_frame();

}

/* Game Loop */
emp gameplay_loop() {
    le_start_frame();


    /* Event Handling */
    event = le_get_event(0);
    
    le_quit_check(&event);

    player.vel.x = 0;
    player.vel.y = 0;

    switch (event.type) {

        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_RIGHT:
                    player_right = 1;
                    break;

                case SDL_SCANCODE_LEFT:
                    player_left = 1;
                    break;

                case SDL_SCANCODE_UP:
                    player_up = 1;
                    break;

                case SDL_SCANCODE_DOWN:
                    player_down = 1;
                    break;

                default:
                    break;


                break;
            }
            break;

        case SDL_KEYUP:
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_RIGHT:
                    player_right = 0;
                    break;

                case SDL_SCANCODE_LEFT:
                    player_left = 0;
                    break;

                case SDL_SCANCODE_UP:
                    player_up = 0;
                    break;

                case SDL_SCANCODE_DOWN:
                    player_down = 0;
                    break;

                default:
                    break;

                break;
            }
            break;

        default:
            break;


    }

    /* Updating */

    if (player_down) {
        player.vel.y += 5;
        player_rotation += 1.0f; 
    }

    if (player_up) {
        player.vel.y -= 5;
        player_rotation += 1.0f; 
    }

    if (player_right) {
        player.vel.x += 5;
        player_rotation += 1.0f; 
    }

    if (player_left) {
        player.vel.x -= 5;
        player_rotation += 1.0f; 
    }

    if (player_particles_dead > le_get_max_particles_num() / 2) {

        f32 x = player.pos.x;
        f32 x_big = player.pos.x + player.w;
        f32 y_big = player.pos.y + player.h;

        // allocation problems?
        repopulate_le_particles_t(&player_particles, 8, 16, 
                                  x, y_big, x_big, y_big, 0, 0,
                                  player.vel.x, player.vel.y, 255, 255, 100, 
                                  player_particles_dead);
        
    }  


    player_particles_dead = update_le_particles_t(&player_particles, 0.1);
    for (int i = 0; i < le_get_max_particles_num(); i++) {  
        i32 x = player.pos.x;
        i32 x_big = player.pos.x + player.w;
        i32 y_big = player.pos.y + player.h;
        player_particles.physics_comp[i].pos.x = randnum(x, x_big);
        player_particles.physics_comp[i].pos.y = randnum(y_big, y_big + 20);
    }
    

    for (i32 i = 0; i < ENEMIES_NUM; i++) {
        enemies[i].pos.x += enemies[i].acc.x;
        enemies[i].pos.y += enemies[i].acc.y;
        if (enemies[i].pos.y > 481) {
            enemies[i].pos.y = -32;
            enemies[i].acc.y = randnum(1, 5);
        }

        if (enemies[i].pos.x < 0) {
            enemies[i].pos.x = 632;
            enemies[i].acc.x = randnum(-3, -1);
        }
        if (enemies[i].pos.x > 640) {
            enemies[i].pos.x = -32;
            enemies[i].acc.x = randnum(1, 3);
        }

        if (collision_detect(player, enemies[i])) {
            le_sfx_t_play(explosion);
            game_over = 1;
            game_play = 0;
        }
    }

    // Check direction of collison
    // and then subtract the -5 or something
    // and then multiply -1
    
    if (collision_detect(player, wall_one) 
     || collision_detect(player, wall_two)) {
        player_coll_with_wall += 1;
        le_sfx_t_play(play);
        player.vel.x *= -2;
    } 

    if (player.pos.y <= 0 
     || player.pos.y >= 480) {
        player_coll_with_wall += 1;
        le_sfx_t_play(play);
        player.vel.y *= -2;
    }

    if (player_coll_with_wall > 0) {
        i32 n = 1;
        for (i32 i = 0; i < player_coll_with_wall; i++) { n *= -1; }
        player.vel.x *= n;
        player.vel.y *= n;
        //player_cooldown += 1;
    }


    player.pos.x += player.vel.x;
    player.pos.y += player.vel.y;

    player_smaller.pos.x = player.pos.x + 8;
    player_smaller.pos.y = player.pos.y + 8;

    score += 1;


    /* Rendering */
    le_cls(47, 17, 155, 255);

        for (i32 i = 0; i < ENEMIES_NUM; i++) {
            le_draw_le_entity_t_ex(&enemies[i], &enemy_img, i * 3, NULL, 0);
        }
        le_draw_le_entity_t_ex(&player, &player_img,  player_rotation, 
                               NULL, 0);
        le_draw_le_entity_t_ex(&player_smaller, &player_img_smaller, 
                               -player_rotation + 45, NULL, 0);



        draw_le_particles_t(&player_particles);
        le_draw_le_entity_t(&wall_one, &wall_img);
        le_draw_le_entity_t(&wall_two, &wall_img);
        le_draw_le_ui_label_t(&h_score_label);
        le_draw_le_ui_label_t(&h_score_actual_label);

     /* FPS */
    le_end_frame();
}

emp game_over_loop() {
    le_start_frame();

    /* Event Handling */
    event = le_get_event(0);
    
    le_quit_check(&event);

    /* Updating */

    if (score > h_score) {
        h_score = score;
        h_score_str = i32_to_le_str(h_score);
        h_score_actual_label.label_text = h_score_str;

        h_score_actual_label.label_tex = SDL_CreateTextureFromSurface(le_get_renderer(), TTF_RenderText_Blended(
                    h_score_actual_label.label_font,
                    h_score_actual_label.label_text,
                    h_score_actual_label.text_color));
        SDL_QueryTexture(h_score_actual_label.label_tex, NULL, NULL, &h_score_actual_label.pos.w,
                        &h_score_actual_label.pos.h);
    }

    int x = 0;
    int y = 0;
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            SDL_GetMouseState(&x, &y);
             
        default:
            break;
    }


    if (x > 300 
     && x < back_to_menu.pos.x + back_to_menu.pos.w 
     && y > 220 
     && y < back_to_menu.pos.y + back_to_menu.pos.h) {
        le_sfx_t_play(play);
        game_over = 0;
        game_menu = 1;
    }


    /* Rendering */
    le_cls(47, 17, 155, 255);

        le_draw_le_ui_label_t(&back_to_menu);

     /* FPS */
    le_end_frame();
}

emp game_loop() {
    if (game_menu) {
        game_menu_loop();
    }
    if (game_play) {
        gameplay_loop();
    }
    if (game_over) {
        game_over_loop();
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
