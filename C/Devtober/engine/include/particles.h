#ifndef _light_engine_particles
    #define _light_engine_particles
    
    #ifndef LE_MAX_PARTICLE_NUMBER
        #define LE_MAX_PARTICLE_NUMBER 100
    #endif

    #include "entity.h"
    #include "img.h"
    #include "util.h"
    #include "types.h"

    typedef struct {
        le_entity_t physics_comp[LE_MAX_PARTICLE_NUMBER];
        le_img_t    animation_comp[LE_MAX_PARTICLE_NUMBER];
    } le_particles_t;

    le_particles_t create_le_particles_t(
            le_str particle_img, i32 min_rad, 
            i32 max_rad, le_vec2_t pos, i32 min_accx,
            i32 max_accx, i32 min_accy, i32 max_accy, i32 r, 
            i32 g, i32 b);

    i32 update_le_particles_t(
            le_particles_t * particles, 
            f32 decrement);

    emp draw_le_particles_t(
            le_particles_t * particles);
    emp repopulate_le_particles_t(le_particles_t * particles, 
                               i32 min_rad, i32 max_rad, 
                               i32 x_min, i32 y_min, i32 x_max,
                               i32 y_max,
                               i32 min_accx,
                               i32 max_accx, i32 min_accy, 
                               i32 max_accy, i32 r, 
                               i32 g, i32 b, i32 n);

    i32 all_particles_dead();
    i32 le_get_max_particles_num(); 

#endif
