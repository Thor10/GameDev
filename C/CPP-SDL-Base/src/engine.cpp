#include <engine.h>
#include <iostream>

gsdl::program::program(const gsdl::init_info_t init) {
    {
        i32 initializers = SDL_Init(SDL_INIT_EVERYTHING) + IMG_Init(IMG_INIT_PNG) + TTF_Init() + Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
        if ((initializers - IMG_INIT_PNG) != 0) {
            std::cout << "Failed to init SDL2"<< std::endl; 
        }
    }    
    {
        u32 win_flags = 0;
        if (init.resizable) {
            win_flags = win_flags | SDL_WINDOW_RESIZABLE;
        }

        window = SDL_CreateWindow(init.title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, init.w, init.h, win_flags);
        if (!window) {
            std::cout << "Error making window" << std::endl;
        }
    }

    {
        u32 renderer_flags = 0;
        if (init.renderer_accel) {
            renderer_flags = renderer_flags | SDL_RENDERER_ACCELERATED;
        }
        if (init.vsync) {
            renderer_flags = renderer_flags | SDL_RENDERER_PRESENTVSYNC;
        }

        renderer = SDL_CreateRenderer(window, -1, renderer_flags);
        if (!renderer) {
            std::cout << "Error making renderer" << std::endl;
        }
    }

    {
        #ifndef __EMSCRIPTEN__
            if (init.vsync) {
                SDL_DisplayMode mode;
                SDL_GetDisplayMode(0, 0, &mode);
                ms = 1000 / mode.refresh_rate;
            }
            else {
                ms = 1000 / init.fps;
            }
        #endif
        vsync = init.vsync;
        running = 1;
        last_step = 0;
        dt = 0;
        memset(avg_dt, 0, sizeof(avg_dt));
        curr_dt_idx = 0;
    }


}

gsdl::program::~program() {
    for (u32 u = 0; u < texture_storage.size(); u++) {
        SDL_DestroyTexture(texture_storage[u]);
    }
    for (u32 u = 0; u < ptr_storage.size(); u++) {
        free(ptr_storage[u]);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();

    SDL_Quit();
}

u08 gsdl::program::check_keypress(SDL_Scancode key) {
    return (event.key.keysym.scancode == key);
}

u08 gsdl::program::check_key(SDL_Scancode key) {
    return keys[key];
}

void gsdl::program::start_dt_calc() {
    last_step = SDL_GetTicks();    
}

void gsdl::program::end_dt_calc() {
    i32 idx = curr_dt_idx;
    avg_dt[idx] = SDL_GetTicks64() - last_step;
    if (curr_dt_idx < AVG_DT) {
        curr_dt_idx++;
    } else {
        curr_dt_idx = 0;
    }
}

i32 gsdl::program::get_frame_time() {
    u64 avg = 0;
    for (u16 u = 0; u < AVG_DT; u++) {
        avg += avg_dt[u]; 
    }
    avg = avg / AVG_DT;
    dt = avg;
    return dt;
}

#ifdef __WIN32__
    #include <Windows.h>
    #include <psapi.h>
#else
    #include <sys/resource.h>
    #include <unistd.h>
#endif
i32 gsdl::program::get_mem_used() {
    #ifdef __WIN32__
        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
        u64 virtualMemUsedByMe = pmc.PrivateUsage;
        mb = (i32) roundl(virtualMemUsedByMe / 1000000);
        return mb;
    #else
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        mb = (i32) roundl(usage.ru_maxrss / 1000000);
        return mb;
    #endif
}



SDL_Window * gsdl::program::get_win() {
    return window;
}

SDL_Renderer * gsdl::program::get_renderer() {
    return renderer;
}

SDL_Event gsdl::program::get_event() {
    SDL_PollEvent(&event);
    keys = SDL_GetKeyboardState(NULL);
    return event;
}


u08 gsdl::program::is_running() {
    return running;
}

void gsdl::program::set_running(u08 s) {
    running = s;
}