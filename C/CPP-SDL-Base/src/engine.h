#ifndef ENGINE_H
    #include <extd_cstd/lib.h>
    #include <vector>
    #include <string>

    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>

    i32 main(i32 argv, char ** args);
    #define SDL_main main

    #define AVG_DT 200 

    // decide on an architecture, biggest problem
    namespace gsdl { 
        typedef struct {
            std::string title;
            i32 w;
            i32 h;
            i32 renderer_accel;
            i32 fps;
            i32 vsync;
            i32 resizable;
        } init_info_t;

        class program {
            public:
                program(const init_info_t init);
                ~program(); 

                u08 check_keypress(SDL_Scancode key);
                u08 check_key(SDL_Scancode key);

                void start_dt_calc();
                void end_dt_calc();

                i32 get_frame_time();
                i32 get_mem_used();

                SDL_Window * get_win();
                SDL_Renderer * get_renderer();

                SDL_Event get_event();

                u08 is_running();
                void set_running(u08 s);

                SDL_Surface * load_surf(const char * file, u32 x, u32 y, u32 w, u32 h);
                SDL_Surface * create_surf(const char * file, u32 x, u32 y, u32 w, u32 h);
                SDL_Texture * create_tex(SDL_Surface * surf);

            private:
                SDL_Window * window;
                SDL_Renderer * renderer;
                SDL_Event event;
                const u08 * keys;
                u16 ms;
                u32 last_step;
                u32 dt;
                u16 avg_dt[200];
                u16 curr_dt_idx; 
                u16 mb;
                u08 running;
                u08 vsync;
                std::vector<void *> ptr_storage;
                std::vector<SDL_Texture *> texture_storage; 
        };

        class image {
            public:
                image(SDL_Surface * surface, i32 colorkey = -1);
                    // SDL_SetColorKey(surf, SDL_TRUE, val);
                ~image();

                inline void set_alpha(u08 a) { alpha = a; SDL_SetTextureAlphaMod(tex, alpha); };
                inline void clip_tex(u32 x, u32 y, u32 w, u32 h) { src.x = x; src.y = y; src.w = w; src.h = h; };

            private: 
                SDL_Surface * surf;
                SDL_Texture * tex;
                SDL_Rect src;
                u08 alpha;
                f64 angle;
                SDL_Point * center;
                u08 flip; // XOR: 0 = none, 1 = horizontal and 2 = vertical
        };

    } // namespace gsdl
    
#endif