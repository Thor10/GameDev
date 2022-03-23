#include <engine.h>

u08    le_on;
u32    le_flags;
SDL_Window   * le_window;
SDL_Renderer * le_renderer;

i08 le_vsync;
f64 le_frame_delay;
f64 le_frame_start;
f64 le_dt;

le_timer_t dt_calc;
SDL_Event le_event;

emp le_init(const le_str win_title, i16 width, i16 height, i32 fps, i08 sync) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        le_print_error("Error Initializing Light Engine: %s", SDL_GetError());
    }
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    le_print_info("Successfully Initialized Light Engine", NULL);
    le_create_win(win_title, width, height);

    SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };
    le_vsync = sync;

    le_frame_delay = 1000 / fps;
    if (le_vsync) {
        le_frame_delay = 1000 / mode.refresh_rate;
    }

    le_create_rnd(sync);
    le_on = 1;
    init_le_timer_t(&dt_calc);

}

emp le_quit_check(SDL_Event * event) {
    if (event -> type == SDL_QUIT) {
        le_on = 0;
    }
}

emp le_handle_resizable(i16 resizable) {
    if (resizable) {
        #ifdef __APPLE__
            le_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
        #else
            le_flags = SDL_WINDOW_RESIZABLE;
        #endif
    }
}

emp le_create_win(const le_str title, i32 width, i32 height) {
    le_flags = 0;
    le_window = SDL_CreateWindow(title, 240, 120, width, height, le_flags);
    if (!le_window) { le_print_error("Error Making Window: %s", SDL_GetError()); }
}

emp le_create_rnd(i08 sync) {
    if (sync ==  0) { le_renderer = SDL_CreateRenderer(le_window, -1, SDL_RENDERER_ACCELERATED); }
    if (sync ==  1) { le_renderer = SDL_CreateRenderer(le_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); }
    if (!le_renderer) { le_print_error("Error Making Renderer: %s", SDL_GetError()); }
}


void le_draw_circle(i32 centreX, i32 centreY, i32 radius, i16 r, i16 g, i16 b, i16 a) {
   const int32_t diameter = (radius * 2);

   int32_t x = (radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - diameter);

   SDL_SetRenderDrawColor(le_renderer, r, g, b, a);
   while (x >= y)
   {
      SDL_RenderDrawPoint(le_renderer, centreX + x, centreY - y);
      SDL_RenderDrawPoint(le_renderer, centreX + x, centreY + y);
      SDL_RenderDrawPoint(le_renderer, centreX - x, centreY - y);
      SDL_RenderDrawPoint(le_renderer, centreX - x, centreY + y);
      SDL_RenderDrawPoint(le_renderer, centreX + y, centreY - x);
      SDL_RenderDrawPoint(le_renderer, centreX + y, centreY + x);
      SDL_RenderDrawPoint(le_renderer, centreX - y, centreY - x);
      SDL_RenderDrawPoint(le_renderer, centreX - y, centreY + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}


emp le_uninit() {
    SDL_DestroyWindow(le_window);
    SDL_DestroyRenderer(le_renderer);
    SDL_Quit();
}

emp le_cls(i16 r, i16 g, i16 b, i16 a) {
    SDL_SetRenderDrawColor(le_renderer, r, g, b, a);        
}
//fix dttt

SDL_Event le_events() {
    SDL_PollEvent(&le_event);
    return le_event;
}

SDL_Renderer * le_get_renderer() {
    return le_renderer;
}

emp le_render_start() {
    SDL_RenderClear(le_renderer);        
}

emp le_render_end() {
    SDL_RenderPresent(le_renderer);        
}

emp le_start_frame() {
    store_curr_time(&dt_calc); 
}

emp le_end_frame() {
    store_sec_passed(&dt_calc);
    le_dt = dt_calc.sec_passed;

    if (le_dt < le_frame_delay) {
        SDL_Delay(le_frame_delay - le_dt);
    }
}


f64 le_get_fps() {
    return 0.0f;
}

f64 le_get_dt() {
    return le_dt;
}

SDL_Window * le_get_window() {
    return le_window;
}

i08 get_le_on() {
    return le_on;
}


emp set_le_on(i08 setting) {
    le_on = setting;
}

emp le_game_loop(void (* game_loop)()) {
    # ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(game_loop, 0, 1);
    # else 
        while (le_on) {
            game_loop();
        }
    # endif
}



