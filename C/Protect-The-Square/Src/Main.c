#include "./Includes.h"

/*
 * Note:
 * Globals are being used as Emscripten really wants functional programming so yeah I don't really 
 * know another way
 */

/* Declaring the global variables I need */
#define SCREEN_W 640 
#define SCREEN_H 480

/* Initializing game, mainly variables needed and the little engine behind the scenes */

typedef struct {
    Mix_Chunk * sfx;
} le_sfx_t;

typedef struct {
    Mix_Music * music;   
} le_music_t;

#define mk_le_music_t(path) (le_music_t) { .music = Mix_LoadMUS(path) };
#define   mk_le_sfx_t(path) (le_sfx_t) { .sfx   = Mix_LoadWAV(path) };
#define music_play_loop(music)   Mix_PlayMusic(music.music, -1);
#define music_play_sfx(sf)      Mix_PlayChannel(-1, sf.sfx, 0);



char * std_i32_to_str(int num) {
    char * string = (char *) malloc(141); 
    snprintf(string, 139, "%d", num); 
    string[140] = '\0';
    return string;
}


#define le_load_font(path, size) \
    TTF_OpenFont(path, size)        

typedef struct {
    SDL_Rect      pos;
    char *        label_text;
    TTF_Font    * label_font;
    SDL_Color     text_color;
    SDL_Texture * label_tex;
} le_ui_label_t;

#define mk_le_ui_label_t(_x, _y, text, font, _r, _g, _b, _a, size) (le_ui_label_t) { .pos.x = _x, .pos.y = _y, .pos.w = 0, .pos.h = 0, .label_text = text, .label_font = le_load_font(font, size), .text_color.r = _r, .text_color.g = _g, .text_color.b = _b, .text_color.a = _a, .label_tex = NULL  \
        }; 


void _create_le_ui_label_t_text(le_ui_label_t * label) {
    label -> label_tex = SDL_CreateTextureFromSurface(le_g_renderer, TTF_RenderText_Blended(label -> label_font, label -> label_text, label -> text_color));
    SDL_QueryTexture(label -> label_tex, NULL, NULL, &label -> pos.w, &label -> pos.h);
}
void _le_draw_le_ui_label_t(le_ui_label_t * label)  {
    SDL_RenderCopy(le_g_renderer, label -> label_tex, NULL, &label -> pos);
}


dynamic_entity_t entity;
image2d player[3];
int entity_move_down;
int entity_move_up;
int  score;
int prev_score;
int high_score;
int curr_player_img;
char * score_str;
char * high_score_str;

image2d enemy_img;
dynamic_entity_t enemies[10];

dynamic_entity_t entity_cpy;
dynamic_entity_t enemy_cpy;


int i;

TTF_Font * game_font;
SDL_Texture * score_tex;
SDL_Rect score_pos;
SDL_Color color;
SDL_Surface * surf;

le_ui_label_t game_score_txt_orig;
le_ui_label_t game_score_txt;


le_ui_label_t game_high_score_txt_orig;
le_ui_label_t game_high_score_txt;

int game_menu;
int game_play;
int game_over;
int game_end;

image2d img;
image2d img_over;
dynamic_entity_t menu;

vec2 scroll;

int player_health_full;
int player_health;

image2d health_red;
image2d health_white;
dynamic_entity_t health_draw;

image2d bg;
dynamic_entity_t bg_one;
dynamic_entity_t bg_two;

le_sfx_t explosion;

int init_game() { 
    le_init();
    le_create_window("Game", SCREEN_W, SCREEN_H, &le_g_flags);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    le_create_renderer();
    score = 0;

    score_pos.x = 150;
    score_pos.y = 20;
    color.r = 255;
    color.g = 255;
    color.b = 255;
    prev_score = score;
    high_score = 0;

    explosion = mk_le_sfx_t("Resources/explosion.wav");

    score_str = std_i32_to_str(score); 
    game_score_txt = mk_le_ui_label_t(138, 42, std_i32_to_str(score), "Resources/Fonts/Sans.ttf", 255, 255, 255, 255, 50);
    game_score_txt_orig = mk_le_ui_label_t(32, 42, "Score: ", "Resources/Fonts/Sans.ttf", 255, 255, 255, 255, 50);
    _create_le_ui_label_t_text(&game_score_txt);
    _create_le_ui_label_t_text(&game_score_txt_orig);
    game_score_txt.label_text = realloc(game_score_txt.label_text, 150);

    score_str = std_i32_to_str(score); 
    game_high_score_txt = mk_le_ui_label_t(218, 72, std_i32_to_str(high_score), "Resources/Fonts/Sans.ttf", 255, 255, 255, 255, 50);
    game_high_score_txt_orig = mk_le_ui_label_t(32, 72, "High Score: ", "Resources/Fonts/Sans.ttf", 255, 255, 255, 255, 50);
    _create_le_ui_label_t_text(&game_high_score_txt);
    _create_le_ui_label_t_text(&game_high_score_txt_orig);
    game_high_score_txt.label_text = realloc(game_high_score_txt.label_text, 150);



    init_dynamic_entity(&entity, 550, 230, 32, 32, 0, 0);
    init_image2d(&player[0], "Resources/player.png", 32, 32, 0, 0);
    init_image2d(&player[1], "Resources/player_1.png", 32, 32, 0, 0);
    init_image2d(&player[2], "Resources/player_2.png", 32, 32, 0, 0);
    curr_player_img = 0;

    entity_move_down = 0;
    entity_move_up   = 0;

    init_image2d(&enemy_img, "Resources/tile.png", 32, 32, 0, 0);
    for (i = 0; i < 10; i++) {
        init_dynamic_entity(&enemies[i], (rand() % (250 - (-640) + 1) + (-640)), (rand() % (480 - (0) + 1) + (0)), 32, 32, rand() % 10, 0);
    }

    game_menu = 1;
    game_play = 0;
    game_over = 0;
    game_end = 0;

    init_image2d(&img, "Resources/640x.png", 640, 480, 0, 0);
    init_image2d(&img_over, "Resources/over.png", 640, 480, 0, 0);
    init_dynamic_entity(&menu, 0, 0, 640, 480, 0, 0);

    init_image2d(&health_red, "Resources/heart.png", 32, 32, 0, 0);
    init_image2d(&health_white, "Resources/heart_empty.png", 32, 32, 0, 0);
    init_dynamic_entity(&health_draw, 1, 1, 32, 32, 0, 0);

    init_image2d(&bg, "Resources/bg.png", 640, 480, 0, 0);
    init_dynamic_entity(&bg_one, 0, 0, 640, 480, 0, 0);
    init_dynamic_entity(&bg_two, 640, 0, 640, 480, 0, 0);

    scroll.x = 0;
    scroll.y = 0; 

    player_health_full = 3;
    player_health = player_health_full;

    return 1;

}

void menu_loop() {
    le_get_events();

    switch (le_g_event.type) {
        case SDL_QUIT:
            le_g_game_running = 0;
            break;
        
        case SDL_KEYDOWN:
            game_play = 1;
            game_menu = 0;

        default:
            break; 
    } 

    /* Update */

    /* Render */
    le_clear_screen(0, 32, 151, 255);
    le_start_render();
        /* Real Rendering here */
        draw_dynamic_entity_img(&menu, &img);

    le_end_render();
  
}

void play_loop() {
    le_get_events();

    scroll.x = 0;
    scroll.y = 0;
    switch (le_g_event.type) {
        case SDL_QUIT:
            le_g_game_running = 0;
            break;
        
        case SDL_KEYDOWN:
            switch (le_g_event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    entity_move_up = 1;
                    break;

                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    entity_move_down = 1;
                    break;

                default:
                    break;
            }
            break;

        case SDL_KEYUP:
            switch (le_g_event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    entity_move_up = 0;
                    break;

                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    entity_move_down = 0;
                    break;

                default:
                    break;
            }
            break;

        default:
            break; 
    } 

    /* Update */
    if (entity_move_down == 1) {
        entity.pos.y += 5;
        entity_move_down = 0;
    }

    if (entity_move_up == 1) {
        entity.pos.y -= 5;
        entity_move_up = 0;
    }

    if (entity.pos.y > SCREEN_H) {
        entity.pos.y = 0;
    }

    if (entity.pos.y < 0) {
        entity.pos.y = SCREEN_H;
    }

    for (i = 0; i < 10; i++) {
        enemies[i].pos.x += enemies[i].acc.x;

        if (enemies[i].pos.x > 650) {
            enemies[i].pos.x = (rand() % (250 - (-640) + 1) + (-640));
            enemies[i].pos.y = (rand() % (480 - (0) + 1) + (0));
            enemies[i].acc.x = rand() % 13;
            prev_score = score;
            score += 1;
            score += 99;
        }

        if (collision_detect(entity, enemies[i])) {
            scroll.x += rand() % 32;
            player_health -= 1;
            enemies[i].w -= 1;
            enemies[i].h -= 1;
            entity.pos.y -= 14;
            enemies[i].pos.x = (rand() % (250 - (-640) + 1) + (-640));
            enemies[i].pos.y = (rand() % (480 - (0) + 1) + (0));
            enemies[i].acc.x = rand() % 13;
            if (curr_player_img + 1 != 3) {
                curr_player_img += 1;
            }
            else {
                game_over = 1;
            }
            music_play_sfx(explosion);
        }
    }

    if (player_health <= 0) {
        game_over = 1;
    }

    if (game_over) {
        game_play = 0;
    }

    if (score != prev_score) {
        score_str = std_i32_to_str(score);
        game_score_txt.label_text = score_str;
        _create_le_ui_label_t_text(&game_score_txt);
    }

    /* Render */
    le_clear_screen(0, 32, 151, 255);
    le_start_render();
        /* Real Rendering here */
        draw_dynamic_entity_img(&bg_one, &bg);
        draw_dynamic_entity_img(&bg_two, &bg);

        bg_one.pos.x -= 1;
        bg_two.pos.x -= 1;

        if (bg_one.pos.x <= -640) {
            bg_one.pos.x = 640;
        }
        if (bg_two.pos.x <= -640) {
            bg_two.pos.x = 640;
        }

        entity_cpy = entity;
        entity_cpy.pos.x -= scroll.x;
        entity_cpy.pos.y -= scroll.y;
        draw_dynamic_entity_img(&entity_cpy, &player[curr_player_img]);

        _le_draw_le_ui_label_t(&game_score_txt_orig);
        _le_draw_le_ui_label_t(&game_score_txt);

        _le_draw_le_ui_label_t(&game_high_score_txt_orig);
        _le_draw_le_ui_label_t(&game_high_score_txt);

        for (i = 0; i < 10; i++) {
            enemy_cpy = enemies[i];
            enemy_cpy.pos.x -= scroll.x;
            enemy_cpy.pos.y -= scroll.y;
            draw_dynamic_entity_img(&enemy_cpy, &enemy_img);

        }

        for (i = 0; i < player_health_full; i++) {
            health_draw.pos.x = 30 * (i + 1);
            health_draw.pos.y = 20;
            draw_dynamic_entity_img(&health_draw, &health_white);
        }
        for (i = 0; i < player_health; i++) {
            health_draw.pos.x = 30 * (i + 1);
            health_draw.pos.y = 20;
            draw_dynamic_entity_img(&health_draw, &health_red);
        }






    le_end_render();
}

void game_over_func() { 
    le_get_events();

    switch (le_g_event.type) {
        case SDL_QUIT:
            le_g_game_running = 0;
            break;
        
        case SDL_KEYDOWN:
            game_play = 1;
            game_over = 0;

        default:
            break; 
    } 

    /* Update */

    /* Render */
    le_clear_screen(0, 32, 151, 255);
    le_start_render();
        /* Real Rendering here */
        draw_dynamic_entity_img(&menu, &img_over);

    le_end_render();
}


/* Game loop... */
void game_loop() {
    if (game_menu) {
        menu_loop();
        player_health = player_health_full;
        score = 0;
    }
    if (game_play) {
        play_loop();
    }

    if (game_over) {
        game_over_func();
        player_health = player_health_full;
        if (high_score < score) { high_score = score; };
        score = 0;
        curr_player_img = 0;
        high_score_str = std_i32_to_str(high_score);
        game_high_score_txt.label_text = high_score_str;
        _create_le_ui_label_t_text(&game_high_score_txt);
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
            le_delay(16);
        }
    # endif

    le_uninit();

    return 0;
}
