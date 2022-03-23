#include <engine.h>
#include <time.h>

// 12:46

u08    le_on;
u32    le_flags;
SDL_Window   * le_window;
SDL_Renderer * le_renderer;

i08 le_vsync;
f64 le_frame_delay;
f64 le_frame_start;
f64 le_dt;

// le_timer_t dt_calc;
u64       timer_start;
u64       timer_end;
le_events le_event;

f64 le_fps_avg[5] = {0};
i32 recorded_frm  = 0; 

emp le_init(const le_str win_title, i16 width, i16 height, i32 fps, i08 sync, i08 renderer_accel) {
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

    le_create_rnd(sync, renderer_accel);
    le_on = 1;
    le_init_table();
    srand(time(0));
    // init_le_timer_t(&dt_calc);

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

emp le_set_win_flags(u32 flags) {
    le_flags = flags;
}

emp le_create_win(const le_str title, i32 width, i32 height) {
    le_flags = 0;
    le_window = SDL_CreateWindow(title, 240, 120, width, height, le_flags);
    if (!le_window) { le_print_error("Error Making Window: %s", SDL_GetError()); }
}

emp le_create_rnd(i08 sync, i08 renderer_accel) {
    if (sync == 0 && renderer_accel == 1) { le_renderer = SDL_CreateRenderer(le_window, -1, SDL_RENDERER_ACCELERATED); }
    if (sync == 1 && renderer_accel == 0) { le_renderer = SDL_CreateRenderer(le_window, -1, SDL_RENDERER_PRESENTVSYNC); }
    if (sync == 1 && renderer_accel == 1) { le_renderer = SDL_CreateRenderer(le_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); }
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
    SDL_RenderClear(le_renderer);        
}

le_events le_get_event(i08 wait) {
    if (!wait) {
        SDL_PollEvent(&le_event);
        return le_event;
    }

    SDL_WaitEvent(&le_event);
    return le_event;
}

SDL_Renderer * le_get_renderer() {
    return le_renderer;
}

emp le_start_frame() {
    // store_curr_time(&dt_calc); 
    timer_start = SDL_GetPerformanceCounter();
}

emp le_end_frame() {
    // store_sec_passed(&dt_calc);
    // le_dt = dt_calc.sec_passed;
    SDL_RenderPresent(le_renderer);    
    timer_end = SDL_GetPerformanceCounter();
    le_dt = (timer_end - timer_start) / (f64) SDL_GetPerformanceFrequency();

    le_fps_avg[recorded_frm] = 1.0/le_dt;
    recorded_frm += 1;
    if (recorded_frm >= 5) {
        recorded_frm = 0;
    }

    if (!le_vsync) {
        SDL_Delay(le_frame_delay - le_dt);
    }
}


f64 le_get_fps() {
    f64 sum = 0.0f;
    for (int i = 0; i < 5; i++) {
        sum += le_fps_avg[i];
    }
    return (sum / 5);
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

i08 le_get_keydown(const i32 scancode, SDL_Event * event) { 
    if (event -> type == SDL_KEYDOWN && event -> key.keysym.scancode == scancode) {
        return 1;
    }
    return 0;
}

i08 le_get_mouse_button_down(const i32 scancode, SDL_Event * event) {
    if (event -> type == SDL_MOUSEBUTTONDOWN && event -> button.button == scancode) {
        return 1;
    }
    return 0;
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


emp le_scale_renderer(i32 scale_x, i32 scale_y) {
    SDL_RenderSetScale(le_renderer, scale_x, scale_y);
}

