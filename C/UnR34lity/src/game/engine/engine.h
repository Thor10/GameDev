#ifndef ENGINE_H
    #include <extd_cstd/lib.h>

    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>
    //#include "sdl_gfx/.h"

    i32 main(i32 argv, char ** args);
    #define SDL_main main

    #define cWHITE 255, 255, 255, 255
    #define cBLACK 000, 000, 000, 255

    /*
    ////////////////////////////////
    ////////////////////////////////
    //// Game Initialization & Props
    ////////////////////////////////
    ////////////////////////////////
    */

    typedef struct {
        char * title;
        i32 w;
        i32 h;
        i32 renderer_accel;
        i32 fps;
        i32 vsync;
        i32 resizable;
    } gsdl_init_info_t;

    typedef struct {
        SDL_Window * win;
        SDL_Renderer * renderer;
        u32 fps;
        u16 ms_delay; 
        u32 last_step;
        i32 dt;
        i32 avg_dt[200];
        i32 mb_used;
        i32 curr_dt;
        u08 running;
        u08 vsync;
        SDL_Event event;
        const u08 * keys_pressed;
    } gsdl_props_t;

    void gsdl_init(const gsdl_init_info_t init, gsdl_props_t * props);
    void gsdl_destroy(gsdl_props_t * props, list_t * texture_storage, list_t * ptr_storage);
    u08 gsdl_check_key(SDL_Event e, SDL_Scancode key);

    void gsdl_start_frame_time_rec(gsdl_props_t * props);
    void gsdl_stop_frame_time_rec(gsdl_props_t * props);
    i32 gsdl_get_avg_frame_time(gsdl_props_t * props);
    void gsdl_get_frame_time_str(const char * frame_time_about, char * int_dest, char * dest, gsdl_props_t * props);

    i32 gsdl_get_mem_used(gsdl_props_t * props);
    void gsdl_get_mem_used_str(char * int_dest, char * dest, gsdl_props_t * props);


    /*
    ////////////////////////////////
    ////////////////////////////////
    //// Game Image 
    ////////////////////////////////
    ////////////////////////////////
    */
    typedef struct {
        SDL_Surface * surf;
        SDL_Texture * tex;
        SDL_Rect src;
        u08 alpha;
        f64 angle;
        SDL_Point * center;
        u08 flip; // XOR: 0 = none, 1 = horizontal and 2 = vertical
        u08 serialized;
    } gsdl_img_t;

    #define gsdl_set_img_colorkey(img, val) SDL_SetColorKey(img -> surf, SDL_TRUE, val);
    #define gsdl_map_img_rgba(surf_format, r, g, b, a) SDL_MapRGBA(surf_format, r, g, b, a);
    #define gsdl_set_img_alpha(img, val) img -> alpha = val; SDL_SetTextureAlphaMod(img -> tex, val);

    void gsdl_load_tex(gsdl_img_t * img, const char * file, u32 x, u32 y, u32 w, u32 h, SDL_Renderer * renderer, list_t * texture_storage, list_t * surf_storage);
    void gsdl_clip_tex(gsdl_img_t * img, u32 x, u32 y, u32 w, u32 h);
    void gsdl_load_textures(gsdl_img_t * imgs, u16 frames, char * prefix, u32 x, u32 y, u32 w, u32 h, SDL_Renderer * renderer, list_t * texture_storage, list_t * surf_storage);
    void gsdl_create_surf(gsdl_img_t * img, u32 w, u32 h, u08 r, u08 g, u08 b, u08 a, list_t * surf_storage);
    void gsdl_create_tex(gsdl_img_t * img, u32 w, u32 h, u08 r, u08 g, u08 b, u08 a, SDL_Renderer * renderer, list_t * texture_storage, list_t * surf_storage);
    void gsdl_create_render_tex(gsdl_img_t * img, u32 w, u32 h, u08 a, SDL_Renderer * renderer, list_t * texture_storage);
    /*
    ////////////////////////////////
    ////////////////////////////////
    //// Game Audio 
    ////////////////////////////////
    ////////////////////////////////
    */
    typedef struct {
        Mix_Music * music;
    } gsdl_music_t;

    typedef struct {
        Mix_Chunk * sfx;
    } gsdl_sfx_t;

    void gsdl_load_music(gsdl_music_t * music, const char * path, list_t * music_storage);
    void gsdl_load_sfx(gsdl_sfx_t * sfx, const char * path, list_t * sfx_storage);
    #define gsdl_play_music_looped(music_m) Mix_PlayMusic(*music_m -> music, -1);
    #define gsdl_play_sfx(sfx_s) Mix_PlayChannel(-1, *sfx_s -> sfx, 0);
    /*
    ////////////////////////////////
    ////////////////////////////////
    //// Game Fonts 
    //// TODO: make it so that u dont have to load the font everytime and just do
    //// a hash table lookup for the font using an id
    ////////////////////////////////
    ////////////////////////////////
    */
    typedef struct {
        SDL_Rect pos;        
        char * label_text; 
        TTF_Font * label_font; 
        SDL_Color text_color; 
        SDL_Texture * label_tex;  
    } gsdl_ui_label_t;

    void gsdl_create_ui_label(gsdl_ui_label_t * label, i32 x, i32 y, char * text, char * font, u08 r, u08 g, u08 b, u08 a, u32 size, SDL_Renderer * renderer, list_t * texture_storage, list_t * font_storage); 
    void gsdl_center_ui_label(gsdl_ui_label_t * label);
    /*
    ////////////////////////////////
    ////////////////////////////////
    //// Game Entity
    ////////////////////////////////
    ////////////////////////////////
    */
    typedef struct {
        v2_t pos; // Position
        v2_t vel; // Velocity 
        v2_t acc; // Acceleration
        f32 y_momentum;
        u16 w;         // Width
        u16 h;         // Height
        u08 coll_l;    // Left Collision
        u08 coll_r;    // Right Collision
        u08 coll_b;    // Bottom Collision
        u08 coll_t;    // Top Collision
        u08 move_l;
        u08 move_r;
        u08 move_b;
        u08 move_t;
        u08 no_movement;
        // add an id?
    } gsdl_phys_obj_t;

    /* 
     * Best detection using SAT 
     * Separating Axis Theorem
     * Add x velocity then check 
     * horizontal collisions. Then
     * do the same thing for vertical
     * collisions
    */ 
    #define gsdl_phys_obj_coll_detect(m, l) \
        (m.pos.x + m.w >= l.pos.x) &&  \
        (l.pos.x + l.w >= m.pos.x) &&  \
        (m.pos.y + m.h >= l.pos.y) &&  \
        (l.pos.y + l.h >= m.pos.y)     \




    // Make sure the acceleration is in negative for the y
    void gsdl_create_phys_obj(gsdl_phys_obj_t * obj, v2_t pos, v2_t acc, u16 w, u16 h);
    // Static object list collisions
    void gsdl_update_phys_obj_vel(gsdl_phys_obj_t * obj);
    void gsdl_update_phys_obj_pos(gsdl_phys_obj_t * o1, const gsdl_phys_obj_t * o2, const u32 o2_len);
    /*
    ////////////////////////////////
    ////////////////////////////////
    //// Game Camera 
    ////////////////////////////////
    ////////////////////////////////
    */
    typedef struct {
        f32 x;
        f32 y;
    } gsdl_cam_t;

    void gsdl_create_cam(gsdl_cam_t * cam);
    // tsx = tile size
    // tsy = tile size
    void gsdl_calc_cam_pos(gsdl_cam_t * cam, gsdl_props_t * props, gsdl_phys_obj_t * o, u16 tsx, u16 tsy);
    /*
    ////////////////////////////////
    ////////////////////////////////
    //// Game Camera 
    ////////////////////////////////
    ////////////////////////////////
    */
    typedef struct { 
        i32 cx;
        i32 cy; 
        i32 rad;
        u08 color[4];
        u08 fill;
    } gsdl_circle_t;
    void gsdl_create_circle(gsdl_circle_t * circle, i32 cx, i32 cy, i32 rad, u08 r, u08 g, u08 b, u08 a, u08 fill);

    /*
    ////////////////////////////////
    ////////////////////////////////
    //// Game Drawing & Rendering
    ////////////////////////////////
    ////////////////////////////////
    */
    void gsdl_start_render(gsdl_props_t * props, u08 r, u08 g, u08 b, u08 a, SDL_Texture * render_texture);
    void gsdl_draw_rect(const SDL_Rect * rect, u08 r, u08 g, u08 b, u08 a, SDL_Renderer * renderer);
    void gsdl_draw_rect_outline(const SDL_Rect * rect, u08 r, u08 g, u08 b, u08 a, SDL_Renderer * renderer);
    void gsdl_draw_ui_label(const gsdl_ui_label_t * label, SDL_Renderer * renderer);
    void gsdl_draw_circle(gsdl_circle_t * circle, SDL_Renderer * renderer);
    void gsdl_draw_img(const gsdl_img_t * img, i32 x, i32 y, gsdl_cam_t * cam, SDL_Renderer * renderer);
    void gsdl_draw_phys_obj(const gsdl_img_t * img, gsdl_phys_obj_t * obj, gsdl_cam_t * cam, SDL_Renderer * renderer);
    void gsdl_draw_phys_obj_rect(gsdl_phys_obj_t * obj, u08 r, u08 g, u08 b, u08 a, gsdl_cam_t * cam, SDL_Renderer * renderer);
    void gsdl_draw_line(i32 x1, i32 y1, i32 x2, i32 y2, u08 r, u08 g, u08 b, u08 a, SDL_Renderer * renderer);
    void gsdl_draw_point(i32 x, i32 y, u08 r, u08 g, u08 b, u08 a, SDL_Renderer * renderer);
    void gsdl_render_scale(f32 sx, f32 sy, SDL_Renderer * renderer); 
    void gsdl_end_render(gsdl_props_t * props);
    void gsdl_render_all(gsdl_props_t * props, u08 r, u08 g, u08 b, u08 a, const gsdl_img_t * img, gsdl_phys_obj_t * obj, gsdl_cam_t * cam, u08 handle_resize, i32 o_w, i32 o_h);

    /*
    ////////////////////////////////
    ////////////////////////////////
    //// Game Save/Serialize 
    ////////////////////////////////
    ////////////////////////////////
    */

    void gsdl_serialize_bin(void * ptr, u64 size, const char * path);
    void * gsdl_deserialize_bin(u64 size, const char * path);
    void gsdl_serialize_img(gsdl_img_t * img, char * file_name, SDL_Renderer * renderer);
    void gsdl_deserialize_img(gsdl_img_t * img, const char * path, SDL_Renderer * renderer, list_t * texture_storage, list_t * surf_storage);


    /*
    ////////////////////////////////
    ////////////////////////////////
    //// Utility 
    ////////////////////////////////
    ////////////////////////////////
    */


#endif