#ifndef _light_engine_audio

    #define _light_engine_audio

    #include <SDL2/SDL_mixer.h>

    typedef struct {
        Mix_Chunk * sfx;
    } le_sfx_t;

    typedef struct {
        Mix_Music * music;   
    } le_music_t;

    #define mk_le_music_t(path) (le_music_t) { .music = Mix_LoadMUS(path) };
    #define   mk_le_sfx_t(path) (le_sfx_t) { .sfx   = Mix_LoadWAV(path) };
    #define le_music_t_play_loop(music)   Mix_PlayMusic(music.music, -1);
    #define le_sfx_t_play(_sfx)      Mix_PlayChannel(-1, _sfx.sfx, 0);


#endif

