#include <engine.h>
#include <iostream>

i32 main(i32 argv, char ** args) {
    gsdl::init_info_t info = {
        .title = "Test",
        .w = 640, 
        .h = 480,
        .renderer_accel = 1,
        .fps = 60,
        .vsync = 1,
        .resizable = 0
    };
    gsdl::program program(info); 

    u32 time;
    u32 last_time;

    while (program.is_running()) {
        program.start_dt_calc();
        { // Events
            SDL_Event e = program.get_event();
            switch (e.type) {
                case SDL_QUIT:
                    program.set_running(0);
                    break;
                
                case SDL_KEYDOWN:
                    if (program.check_key(SDL_SCANCODE_0)) {
                        printf("lol\n");
                    }
                    break;
            
                default:
                    break;
            }

        }        
        { // Update
            time = program.get_frame_time();
            if (time != last_time) {
                std::cout << program.get_frame_time() << std::endl;
                last_time = time;
            }

        }
        { // Rendering
            SDL_SetRenderDrawColor(program.get_renderer(), 255, 255, 255, 255);        
            SDL_RenderClear(program.get_renderer());           

            SDL_RenderPresent(program.get_renderer());
        }

        program.end_dt_calc();        
    }
}