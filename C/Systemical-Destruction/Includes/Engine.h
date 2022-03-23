#ifndef _light_engine
    #define _light_engine
    #include <stdio.h>
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include "Log.h"

    #define SDL_MAIN_ARGS int argc, char * argv[]

    uint32_t le_g_flags = 0;
    SDL_Window * le_g_window;
    SDL_Renderer * le_g_renderer;

    SDL_Event le_g_event;
    const uint8_t * le_g_key_state;

    int le_g_game_running = 1;


    #define le_log_error(...)     log_error(__VA_ARGS__)
    #define le_log_info(...)      log_info(__VA_ARGS__)
    #define le_log_fatal(...)     log_fatal(__VA_ARGS__)
    #define le_log_important(...) log_important(__VA_ARGS__)
    #define le_log_warn(...)      log_warn(__VA_ARGS__)   

    void le_init() {
	    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
	        le_log_error("Error Initializing SDL: %s", SDL_GetError());
	    }

	    le_log_info("Successfully Initialized SDL", NULL);
    }

    uint32_t le_handle_resizable(int resizable) {
	    uint32_t flags = 0;

	    if (resizable) {
	        # ifndef __APPLE__
		    flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
	        # else 
		    flags = SDL_WINDOW_RESIZABLE;
	        # endif
	    }
	
	    return flags;
    }

    void le_create_window(const char * title, int width, int height, uint32_t * flags) {
        le_g_window = SDL_CreateWindow(title, 240, 120, width, height, *flags);
        if (!le_g_window) {
            le_log_error("Error Creating Window: %s", SDL_GetError());
	    }

    }

    void le_create_renderer() {
	    le_g_renderer = SDL_CreateRenderer(le_g_window, -1, SDL_RENDERER_ACCELERATED);
	    if (!le_g_renderer) {
	        le_log_error("Error Creating Renderer: %s", SDL_GetError());
	    }

    }

    void le_draw_rect(SDL_Rect * obj, int r, int g, int b, int a) {
        SDL_SetRenderDrawColor(le_g_renderer, r, g, b, a);
        SDL_RenderDrawRect(le_g_renderer, obj);
        SDL_RenderFillRect(le_g_renderer, obj);
    }

     
    void le_draw_circle(int32_t centreX, int32_t centreY, int32_t radius) {
        const int32_t diameter = (radius * 2);

        int32_t x = (radius - 1);
        int32_t y = 0;
        int32_t tx = 1;
        int32_t ty = 1;
        int32_t error = (tx - diameter);

        while (x >= y) {
        
            SDL_SetRenderDrawColor(le_g_renderer, 255, 255, 255, 255);
          /*  Each of the following renders an octant of the circle */
            SDL_RenderDrawPoint(le_g_renderer, centreX + x, centreY - y);
            SDL_RenderDrawPoint(le_g_renderer, centreX + x, centreY + y);
            SDL_RenderDrawPoint(le_g_renderer, centreX - x, centreY - y);
            SDL_RenderDrawPoint(le_g_renderer, centreX - x, centreY + y);
            SDL_RenderDrawPoint(le_g_renderer, centreX + y, centreY - x);
            SDL_RenderDrawPoint(le_g_renderer,centreX + y, centreY + x);
            SDL_RenderDrawPoint(le_g_renderer, centreX - y, centreY - x);
            SDL_RenderDrawPoint(le_g_renderer, centreX - y, centreY + x);

	        if (error <= 0) {
	            ++y;
	            error += ty;
	            ty += 2;
	        }

            if (error > 0) {
	            --x;
	            tx += 2;
	            error += (tx - diameter);
	        }
        }
    }


    SDL_Texture * le_load_texture(const char * fileName) {
	    SDL_Surface * tempSrf = IMG_Load(fileName);
	    SDL_Texture * texture = SDL_CreateTextureFromSurface(le_g_renderer, tempSrf);

	    if (texture == NULL) {
	        le_log_error("Error Loading Texture, %s", fileName);
	    }

	    SDL_FreeSurface(tempSrf);

	    return texture;
    }

    void le_uninit() {
	    SDL_DestroyWindow(le_g_window);
	    SDL_DestroyRenderer(le_g_renderer);
	    SDL_Quit();
    }

    void le_clear_screen(int r, int g, int b, int a) {
	    SDL_SetRenderDrawColor(le_g_renderer, r, g, b, a);
    }
    
    #define HANDLEQUIT \
	    case SDL_QUIT:  \
	        le_g_game_running = 0;  \
	        break;            

    void le_get_events() {
	    SDL_PollEvent(&le_g_event);
	    le_g_key_state = SDL_GetKeyboardState(NULL);
    }

    void le_start_render() {
        SDL_RenderClear(le_g_renderer);
    }

    void le_end_render() {
        SDL_RenderPresent(le_g_renderer);
    }

    void le_delay(int fpms) {
        SDL_Delay(fpms);
    }


#endif
