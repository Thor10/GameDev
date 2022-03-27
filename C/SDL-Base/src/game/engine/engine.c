#include "./engine.h"

void gsdl_init(const gsdl_init_info_t init, gsdl_props_t * props) {
    { // SDL Init
        i32 initializers = SDL_Init(SDL_INIT_EVERYTHING) + IMG_Init(IMG_INIT_PNG) + TTF_Init() + Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
        if ((initializers - IMG_INIT_PNG) != 0) {
            printf("%u\n", initializers);
            printf("Failed to initialize SDL2: %s\n", SDL_GetError());
        }       
    }
    { // Window 
        u32 window_flags = 0;
        if (init.resizable) {
            #ifdef __APPLE__
                window_flags = window_flags | SDL_WINDOW_RESIZABLE;
            #else
                window_flags = window_flags | SDL_WINDOW_RESIZABLE;
            #endif
        }

        props -> win = SDL_CreateWindow(init.title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, init.w, init.h, window_flags);
        if (!props -> win) {
            printf("Error making SDL2 window: %s\n", SDL_GetError());
        }
    }
    { // Renderer
        u32 renderer_flags = 0;
        if (init.renderer_accel) {
            renderer_flags = renderer_flags | SDL_RENDERER_ACCELERATED;
        }
        if (init.vsync) {
            renderer_flags = renderer_flags | SDL_RENDERER_PRESENTVSYNC;
        }

        props -> renderer = SDL_CreateRenderer(props -> win, -1, renderer_flags);
        if (!props -> renderer) {
            printf("Error making SDL2 renderer: %s\n", SDL_GetError());
        }
    }
    { // FPS + Other Setups
        #ifndef __EMSCRIPTEN__
            if (init.vsync) {
                SDL_DisplayMode mode; 
                SDL_GetDisplayMode(0, 0, &mode);
                props -> ms_delay = 1000 / mode.refresh_rate;
                props -> fps = mode.refresh_rate;
            }
            else {
                props -> ms_delay = 1000 / init.fps;
                props -> fps = init.fps;
            }
        #endif

        props -> vsync = init.vsync;
        props -> running = 1;
        props -> last_step = 0;
        props -> dt = 0;
        memset(props -> avg_dt, 0, sizeof(props -> avg_dt));
        props -> curr_dt = 0;
    }
}

void gsdl_destroy(gsdl_props_t * props, list_t * texture_storage, list_t * ptr_storage) {
    for (u32 u = 0; u < texture_storage -> len; u++) {
        if (texture_storage -> data[u]) {
            SDL_DestroyTexture(texture_storage -> data[u]);
        }
    }
    free(texture_storage -> data);
    list_rm(ptr_storage);

    SDL_DestroyRenderer(props -> renderer);
    SDL_DestroyWindow(props -> win);
    Mix_CloseAudio();

    SDL_Quit();
}

void gsdl_start_frame_time_rec(gsdl_props_t * props) {
    props -> last_step = SDL_GetTicks();
}

void gsdl_stop_frame_time_rec(gsdl_props_t * props) {
    i32 idx = props -> curr_dt;
    props -> avg_dt[idx] = SDL_GetTicks() - props -> last_step;
    if (props -> curr_dt < sizeof(props -> avg_dt) / sizeof(i32)) {
        props -> curr_dt++;
    } else {
        props -> curr_dt = 0;
    }
}

i32 gsdl_get_avg_frame_time(gsdl_props_t * props) {
    u64 avg = 0;
    for (u16 u = 0; u < sizeof(props -> avg_dt) / sizeof(i32); u++) {
        avg += props -> avg_dt[u];
    } 
    avg = avg / (sizeof(props -> avg_dt) / sizeof(i32));
    props -> dt = avg;
    return props -> dt;
}

void gsdl_get_frame_time_str(const char * frame_time_about, char * int_dest, char * dest, gsdl_props_t * props) {
    i32 dt = gsdl_get_avg_frame_time(props);
    snprintf(int_dest, 24, "%d", dt);
    if (frame_time_about != NULL) {
        strncat(dest, frame_time_about, strlen(frame_time_about));
    }
    strncat(dest, int_dest, strlen(int_dest));
}

#ifdef __WIN32__
    #include <Windows.h>
    #include <psapi.h>
#else
    #include <sys/resource.h>
    #include <unistd.h>
#endif

i32 gsdl_get_mem_used(gsdl_props_t * props) {
    #ifdef __WIN32__
        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
        u64 virtualMemUsedByMe = pmc.PrivateUsage;
        props -> mb_used = (i32) roundl(virtualMemUsedByMe / 1000000);
        return props -> mb_used;
    #else
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        props -> mb_used = (i32) roundl(usage.ru_maxrss / 1000000);
        return props -> mb_used;
    #endif
}

void gsdl_get_mem_used_str(char * int_dest, char * dest, gsdl_props_t * props) {
    const char * about = "Mem Used (mb): ";
    i32 mem_used = gsdl_get_mem_used(props); 
    snprintf(int_dest, 24, "%d", mem_used);
    strncat(dest, about, strlen(about));
    strncat(dest, int_dest, strlen(int_dest));
}

u08 gsdl_check_key(SDL_Event e, SDL_Scancode key) {
    return (e.key.keysym.scancode == key);
}

void gsdl_load_tex(gsdl_img_t * img, const char * file, u32 x, u32 y, u32 w, u32 h, SDL_Renderer * renderer, list_t * texture_storage, list_t * surf_storage) {
    img -> surf = IMG_Load(file);
    img -> tex = SDL_CreateTextureFromSurface(renderer, img -> surf);
    if (!img -> tex) { log_fatal_err("Error Loading %s", file); }
    img -> src.w = w;
    img -> src.h = h;
    img -> src.x = x;
    img -> src.y = y;
    list_insert(texture_storage, img -> tex);
    list_insert(surf_storage, img -> surf);
}

void gsdl_clip_tex(gsdl_img_t * img, u32 x, u32 y, u32 w, u32 h) {
    img -> src.x = x;
    img -> src.y = y;
    img -> src.w = w;
    img -> src.h = h;
}

void gsdl_load_textures(gsdl_img_t * imgs, u16 frames, char * prefix, u32 x, u32 y, u32 w, u32 h, SDL_Renderer * renderer, list_t * texture_storage, list_t * surf_storage) {
    char * curr_image = calloc(512, sizeof(char));
    char * final = calloc(512, sizeof(char));
    char * ext = ".png";
    for (int i = 0; i < frames; i++) {
        char * curr_frame = i32_to_str(i);

        strncat(curr_image, prefix, strlen(prefix));
        strncat(curr_image, curr_frame, strlen(curr_frame));
        strncat(curr_image, ext, strlen(ext));

        memcpy(final, curr_image, strlen(curr_image) + 1);
        final[strlen(curr_image) + 1] = '\0';

        //imgs[i] = mk_le_img(curr_image, 0, 0, w, h);
        gsdl_load_tex(&imgs[i], curr_image, x, y, w, h, renderer, texture_storage, surf_storage);
        memset(curr_image, 0, strlen(curr_image)); 
    }
}


void gsdl_create_surf(gsdl_img_t * img, u32 w, u32 h, u08 r, u08 g, u08 b, u08 a, list_t * surf_storage) {
    SDL_Surface * surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    SDL_LockSurface(surface);

    u08 * px = (u08 *) surface -> pixels;

    for (i32 y = 0; y < h; y++) {
        for (i32 x = 0; x < w; x++) {
            px[y * surface -> pitch + x * surface -> format -> BytesPerPixel + 2] = r;
            px[y * surface -> pitch + x * surface -> format -> BytesPerPixel + 0] = g;
            px[y * surface -> pitch + x * surface -> format -> BytesPerPixel + 1] = b;             
        }
    }
    
    SDL_UnlockSurface(surface);

    if (surface == NULL) {
        log_warn("Failed to make surface: %s", SDL_GetError());
    }

    img -> surf = surface;
    list_insert(surf_storage, surface);
}

void gsdl_create_tex(gsdl_img_t * img, u32 w, u32 h, u08 r, u08 g, u08 b, u08 a, SDL_Renderer * renderer, list_t * texture_storage, list_t * surf_storage) {
    gsdl_create_surf(img, w, h, r, g, b, a, surf_storage);
    img -> tex = SDL_CreateTextureFromSurface(renderer, img -> surf);

    if (img -> tex == NULL) {
        log_warn("Failed to make texture: %s", SDL_GetError());
    }

    img -> src = (SDL_Rect) { 0, 0, w, h };
    img -> alpha = a;         
    SDL_SetTextureBlendMode(img -> tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(img -> tex, a);
    list_insert(texture_storage, img -> tex);
}

void gsdl_create_render_tex(gsdl_img_t * img, u32 w, u32 h, u08 a, SDL_Renderer * renderer, list_t * texture_storage) {
    img -> tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    img -> src = (SDL_Rect) { 0, 0, w, h };
    SDL_SetTextureBlendMode(img -> tex, SDL_BLENDMODE_BLEND);
    img -> alpha = a;
    img -> angle = 0;
    img -> center = NULL;
    img -> flip = 0;
    img -> serialized = 0;
    list_insert(texture_storage, img -> tex);
    SDL_SetRenderTarget(renderer, img -> tex);
}

void gsdl_load_music(gsdl_music_t * music, const char * path, list_t * music_storage) {
    music -> music = Mix_LoadMUS(path);
    list_insert(music_storage, music -> music);
}

void gsdl_load_sfx(gsdl_sfx_t * sfx, const char * path, list_t * sfx_storage) {
    sfx -> sfx = Mix_LoadWAV(path);
    list_insert(sfx_storage, sfx -> sfx);
}

#define gsdl_load_font(path, size) \
    TTF_OpenFont(path, size) 

void gsdl_create_ui_label(gsdl_ui_label_t * label, i32 x, i32 y, char * text, char * font, u08 r, u08 g, u08 b, u08 a, u32 size, SDL_Renderer * renderer, list_t * texture_storage, list_t * font_storage) {
    label -> pos.x = x;
    label -> pos.y = y;
    label -> label_text = text;
    label -> label_font = gsdl_load_font(font, size);
    label -> text_color.r = r;
    label -> text_color.g = g;
    label -> text_color.b = b;
    label -> text_color.a = a;
    label -> label_tex = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(label -> label_font,
                                                                                       label -> label_text,
                                                                                       label -> text_color));
    SDL_QueryTexture(label -> label_tex, NULL, NULL, &label -> pos.w, &label -> pos.h);
    list_insert(texture_storage, label -> label_tex);
    list_insert(font_storage, label -> label_font);
}

void gsdl_center_ui_label(gsdl_ui_label_t * label) {
    label -> pos.x -= label -> pos.w / 2;
    label -> pos.y -= label -> pos.h / 2;
}

void gsdl_create_phys_obj(gsdl_phys_obj_t * obj, v2_t pos, v2_t acc, u16 w, u16 h) {
    memset(obj, 0, sizeof(gsdl_phys_obj_t));
    obj -> pos = pos;
    obj -> vel = mk_v2(0, 0);
    obj -> acc = acc;
    obj -> w = w;
    obj -> h = h;
}

void gsdl_update_phys_obj_vel(gsdl_phys_obj_t * obj) {
    obj -> vel = mk_v2_zero();
    if (obj -> move_r) {
        obj -> vel.x += obj -> acc.x;
    }
    if (obj -> move_l) {
        obj -> vel.x -= obj -> acc.x;
    }
    if (obj -> move_t) {
        obj -> vel.y += obj -> acc.y;
    }
    if (obj -> move_b) {
        obj -> vel.y -= obj -> acc.y;
    }

    if (obj -> move_r + obj -> move_l + obj -> move_t + obj -> move_b < 1) {
        obj -> no_movement = 1;
    } 
}

void gsdl_update_phys_obj_pos(gsdl_phys_obj_t * o1, const gsdl_phys_obj_t * o2, const u32 o2_len) {
    o1 -> vel.y += o1 -> y_momentum;
    o1 -> pos.x += o1 -> vel.x;
    o1 -> coll_r = 0;
    o1 -> coll_l = 0;

    if (o2) {
        for (u32 i = 0; i < o2_len; i++) {
            if (gsdl_phys_obj_coll_detect((*o1), o2[i])) {
                if (o1 -> vel.x > 0) {
                    o1 -> pos.x -= o1 -> acc.x;
                    /* if collision breaks, do while loop and subtract */
                    o1 -> coll_r = 1;
                    o1 -> coll_l = 0;
                }

                else if (o1 -> vel.x < 0) {
                    o1 -> pos.x += o1 -> acc.x;
                    o1 -> coll_r = 0;
                    o1 -> coll_l = 1;
                } 
            }
        }
    }

    o1 -> pos.y += o1 -> vel.y;
    o1 -> coll_b = 0;
    o1 -> coll_t = 0;

    if (o2) {
        for (u32 i = 0; i < o2_len; i++) {
            if (gsdl_phys_obj_coll_detect((*o1), o2[i])) {
                if (o1 -> vel.y > 0) {
                    o1 -> pos.y -= o1 -> y_momentum;
                    //o1 -> pos.y += o1 -> acc.y;
                    o1 -> coll_b = 1;
                    o1 -> coll_t = 0;
                }

                else if (o1 -> vel.y < 0) {
                    o1 -> pos.y += fabsf(o1 -> y_momentum);
                    //o1 -> pos.y -= o1 -> acc.y;
                    o1 -> coll_b = 0;
                    o1 -> coll_t = 1;
                }               
            
            }
        }
    }

}

void gsdl_create_cam(gsdl_cam_t * cam) {
    memset(cam, 0, sizeof(gsdl_cam_t));
}

void gsdl_calc_cam_pos(gsdl_cam_t * cam, gsdl_props_t * props, gsdl_phys_obj_t * o, u16 tsx, u16 tsy) {
    SDL_Texture * target = SDL_GetRenderTarget(props -> renderer);
    i32 w;
    i32 h;
    if (!target) {
        SDL_GetWindowSize(props -> win, &w, &h);
    } else {
        u32 format;
        i32 access;
        SDL_QueryTexture(target, &format, &access, &w, &h);
    }

    cam -> x += (i32) round((o -> pos.x - cam -> x - ((w * 0.5) - (tsx * 0.5))) / 10);
    cam -> y += (i32) round((o -> pos.y - cam -> y - ((h * 0.5) - (tsy * 0.5))) / 10); 
}

void gsdl_create_circle(gsdl_circle_t * circle, i32 cx, i32 cy, i32 rad, u08 r, u08 g, u08 b, u08 a, u08 fill) {
    circle -> cx = cx;
    circle -> cy = cy;
    circle -> rad = rad;
    circle -> color[0] = r; 
    circle -> color[1] = g; 
    circle -> color[2] = b; 
    circle -> color[3] = a; 
    circle -> fill = fill;
}

void gsdl_start_render(gsdl_props_t * props, u08 r, u08 g, u08 b, u08 a, SDL_Texture * render_texture) {
    SDL_SetRenderTarget(props -> renderer, render_texture);
    SDL_SetRenderDrawColor(props -> renderer, r, g, b, a);        
    SDL_RenderClear(props -> renderer);
}

void gsdl_draw_rect_outline(const SDL_Rect * rect, u08 r, u08 g, u08 b, u08 a, SDL_Renderer * renderer) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawRect(renderer, rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
}

void gsdl_draw_rect(const SDL_Rect * rect, u08 r, u08 g, u08 b, u08 a, SDL_Renderer * renderer) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
}

void gsdl_draw_ui_label(const gsdl_ui_label_t * label, SDL_Renderer * renderer) {
    SDL_RenderCopy(renderer, label -> label_tex, NULL, &label -> pos);
}

void _gsdl_draw_circle(i32 cx, i32 cy, u32 rad, u08 r, u08 g, u08 b, u08 a, u08 fill, SDL_Renderer * renderer) {
    // https://gist.github.com/Gumichan01/332c26f6197a432db91cc4327fcabb1c
    i32 x = cx;
    i32 y = cy;

    i32 x_offset = 0;
    i32 y_offset = rad; 
    i32 d = rad - 1;

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    if (!fill) {    

        while (y_offset >= x_offset) {
            SDL_RenderDrawPoint(renderer, x + x_offset, y + y_offset);
            SDL_RenderDrawPoint(renderer, x + y_offset, y + x_offset);
            SDL_RenderDrawPoint(renderer, x - x_offset, y + y_offset);
            SDL_RenderDrawPoint(renderer, x - y_offset, y + x_offset);
            SDL_RenderDrawPoint(renderer, x + x_offset, y - y_offset);
            SDL_RenderDrawPoint(renderer, x + y_offset, y - x_offset);
            SDL_RenderDrawPoint(renderer, x - x_offset, y - y_offset);
            SDL_RenderDrawPoint(renderer, x - y_offset, y - x_offset);

            if (d >= 2 * x_offset) {
                d -= 2 * x_offset + 1;
                x_offset += 1;
            }
            else if (d < 2 * (rad - y_offset)) {
                d += 2 * y_offset - 1;
                y_offset -= 1;
            }
            else {
                d += 2 * (y_offset - x_offset - 1);
                y_offset -= 1;
                x_offset += 1;
            }
        }

        return;
    }


    while (y_offset >= x_offset) {

        SDL_RenderDrawLine(renderer, x - y_offset, y + x_offset,
                                     x + y_offset, y + x_offset);
        SDL_RenderDrawLine(renderer, x - x_offset, y + y_offset,
                                     x + x_offset, y + y_offset);
        SDL_RenderDrawLine(renderer, x - x_offset, y - y_offset,
                                     x + x_offset, y - y_offset);
        SDL_RenderDrawLine(renderer, x - y_offset, y - x_offset,
                                     x + y_offset, y - x_offset);

        if (d >= 2 * x_offset) {
            d -= 2 * x_offset + 1;
            x_offset += 1;
        }
        else if (d < 2 * (rad - y_offset)) {
            d += 2 * y_offset - 1;
            y_offset -= 1;
        }
        else {
            d += 2 * (y_offset - x_offset - 1);
            y_offset -= 1;
            x_offset += 1;
        }
    }
    return;


}

void gsdl_draw_circle(gsdl_circle_t * circle, SDL_Renderer * renderer) {
    _gsdl_draw_circle(circle -> cx, circle -> cy, circle -> rad, circle -> color[0], circle -> color[1], circle -> color[2], circle -> color[3], circle -> fill, renderer);
}


// XOR the flip for the image
void gsdl_draw_img(const gsdl_img_t * img, i32 x, i32 y, gsdl_cam_t * cam, SDL_Renderer * renderer) {
    SDL_Rect rect;

    if (cam == NULL) {
        rect.x = x;
        rect.y = y;
    } 
    else {
        rect.x = x - cam -> x;
        rect.y = y - cam -> y;
    }

    rect.w = img -> src.w;
    rect.h = img -> src.h;

    SDL_RenderCopyEx(renderer, img -> tex, &img -> src, &rect, img -> angle, img -> center, img -> flip);
}

void gsdl_draw_phys_obj(const gsdl_img_t * img, gsdl_phys_obj_t * obj, gsdl_cam_t * cam, SDL_Renderer * renderer) {
    SDL_Rect rect;

    if (cam == NULL) {
        rect.x = obj -> pos.x;
        rect.y = obj -> pos.y;
    } 
    else {
        rect.x = obj -> pos.x - cam -> x;
        rect.y = obj -> pos.y - cam -> y;
    }

    rect.w = obj -> w;
    rect.h = obj -> h;

    SDL_RendererFlip f = SDL_FLIP_NONE;
    if (img -> flip) {
        f = SDL_FLIP_HORIZONTAL;
    }

    SDL_RenderCopyEx(renderer, img -> tex, &img -> src, &rect, img -> angle, img -> center, f);
    //SDL_RenderCopy(renderer, img -> tex, &img -> src, &rect);
}

void gsdl_draw_phys_obj_rect(gsdl_phys_obj_t * obj, u08 r, u08 g, u08 b, u08 a, gsdl_cam_t * cam, SDL_Renderer * renderer) {
    SDL_Rect rect;

    if (cam == NULL) {
        rect.x = obj -> pos.x;
        rect.y = obj -> pos.y; 
    }
    else {
        rect.x = obj -> pos.x - cam -> x;
        rect.y = obj -> pos.y - cam -> y; 
    }

    rect.w = obj -> w;
    rect.h = obj -> h;

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
}

void gsdl_draw_line(i32 x1, i32 y1, i32 x2, i32 y2, u08 r, u08 g, u08 b, u08 a, SDL_Renderer * renderer) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);        
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
}
void gsdl_draw_point(i32 x, i32 y, u08 r, u08 g, u08 b, u08 a, SDL_Renderer * renderer) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);        
    SDL_RenderDrawPoint(renderer, x, y); 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
}

void gsdl_render_scale(f32 sx, f32 sy, SDL_Renderer * renderer) { 
    SDL_RenderSetScale(renderer, sx, sy);
}

void gsdl_end_render(gsdl_props_t * props) {
    SDL_Texture * render_target = SDL_GetRenderTarget(props -> renderer);
    if (!render_target) {
        SDL_RenderPresent(props -> renderer);
        if (!props -> vsync) {
            SDL_Delay(props -> ms_delay);  
        }
    }
}

void gsdl_render_all(gsdl_props_t * props, u08 r, u08 g, u08 b, u08 a, const gsdl_img_t * img, gsdl_phys_obj_t * obj, gsdl_cam_t * cam, u08 handle_resize, i32 o_w, i32 o_h) {
    i32 w;
    i32 h;
    SDL_GetWindowSize(props -> win, &w, &h);

    obj -> pos.x = w/2;
    obj -> pos.y = h/2;

    obj -> pos.x -= obj -> w / 2;
    obj -> pos.y -= obj -> h / 2;

    
    f32 x_scale = (f32) w/o_w;
    f32 y_scale = (f32) h/o_h;

    f32 min = 0.0f;

    if (x_scale < y_scale) {
        min = x_scale;
    } else {
        min = y_scale;
    }

    u16 n_w = o_w * min;
    u16 n_h = o_h * min;

    if (obj -> w != n_w) {
        obj -> w = n_w;
    }

    if (obj -> h != n_h) {
        obj -> h = n_h;
    }

    gsdl_start_render(props, r, g, b, a, NULL);
        gsdl_draw_phys_obj(img, obj, cam, props -> renderer);
    gsdl_end_render(props);
}

void gsdl_serialize_bin(void * ptr, u64 size, const char * path) {
    FILE * fp = fopen(path, "wb");
    if (fwrite(ptr, size, 1, fp) == 0) {
        log_warn("Failed to serialize");
    } 
    fclose(fp);    
}

void * gsdl_deserialize_bin(u64 size, const char * path) {
    void * s = calloc(1, size);
    FILE * sp = fopen(path, "rb");
    fread(s, size, 1, sp);
    fclose(sp);
    return s;
}

void gsdl_serialize_img(gsdl_img_t * img, char * file_name, SDL_Renderer * renderer) {
    SDL_PixelFormatEnum format;
    i32 access;

    /* Get information about texture we want to save */
    u08 texture_query = SDL_QueryTexture(img -> tex, &format, &access, NULL, NULL) + 1;
    if (!texture_query) {
        log_warn("Texture query failed, %s", SDL_GetError());
    }
    SDL_Texture * render_texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_TARGET, img -> src.w, img -> src.h);
    if (!render_texture) {
        log_warn("Failed to create a render texture: %s\n", SDL_GetError());
    } 

    u08 render_target_set = SDL_SetRenderTarget(renderer, render_texture) + 1;
    if (!render_target_set) {
        log_warn("Failed to set render target, %s", SDL_GetError());
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    u08 render = SDL_RenderCopy(renderer, img -> tex, NULL, NULL) + 1;
    if (!render) {
        log_warn("Failed rendering texture to file, %s", SDL_GetError());
    }

    void * pixels = malloc(img -> src.w * img -> src.h * SDL_BYTESPERPIXEL(format));
    if (!pixels) {
        log_warn("Failed allocating memory for saving texture");
    }

    u08 read_pixels = SDL_RenderReadPixels(renderer, NULL, format, pixels, img -> src.w * SDL_BYTESPERPIXEL(format));
    if (read_pixels) {
        log_warn("Failed reading pixel data, %s", SDL_GetError());
    }

    SDL_Surface * surf = SDL_CreateRGBSurfaceWithFormatFrom(pixels, img -> src.w, img -> src.h, SDL_BITSPERPIXEL(format), img -> src.w * SDL_BYTESPERPIXEL(format), format);
    if (!surf) {
        log_warn("Failed to create a surface for saving %s", SDL_GetError());
    }

    u08 saved = IMG_SavePNG(surf, file_name) + 1;    
    if (!saved) {
        log_warn("Failed to save %s", file_name);
    } 

    img -> serialized = 1;
}


void gsdl_deserialize_img(gsdl_img_t * img, const char * path, SDL_Renderer * renderer, list_t * texture_storage, list_t * surf_storage) {
    gsdl_load_tex(img, path, img -> src.x, img -> src.y, img -> src.w, img -> src.h, renderer, texture_storage, surf_storage);    
    img -> serialized = 0;
}
