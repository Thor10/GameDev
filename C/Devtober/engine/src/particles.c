#include <particles.h>
#include <engine.h>

// Particle image must be of the color values 
// 255, 255, 255, 255 -> RGBA
//
le_img_t global_particle_image;
i32 particles_dead = 0;

le_particles_t create_le_particles_t(
            le_str particle_img, i32 min_rad, 
            i32 max_rad, le_vec2_t pos, i32 min_accx,
            i32 max_accx, i32 min_accy, i32 max_accy, i32 r, 
            i32 g, i32 b) {
    le_particles_t particles = {0};

    global_particle_image = mk_le_img_t(particle_img, 0, 0, 32, 32);

    for (i32 i = 0; i < LE_MAX_PARTICLE_NUMBER; i++) {
        i32 diam = randnum(min_rad, max_rad) * 2;
        particles.physics_comp[i] = mk_le_entity_t(le_v2(pos.x, 
                                                         pos.y), 
                                    le_v2(0, 0), le_v2(
                                                      randnum(
                                                        min_accx,
                                                        max_accx
                                                             ),
                                                      randnum(
                                                        min_accy,
                                                        max_accy
                                                             )), 
                                    diam, diam);



        particles.animation_comp[i] = global_particle_image; 
        SDL_SetTextureColorMod(particles.animation_comp[i].tex,
                               r, g, b);
    }
    return particles;
}

i32 update_le_particles_t(le_particles_t * particles, f32 decrement) {
    i32 dead_particles_num = 0;
    for (i32 i = 0; i < LE_MAX_PARTICLE_NUMBER; i++) {
        if (particles -> physics_comp[i].w > 2 
         && particles -> physics_comp[i].h > 2) {

            particles -> physics_comp[i].pos.x += particles -> physics_comp[i].acc.x;
            particles -> physics_comp[i].pos.y += particles -> physics_comp[i].acc.y;
            particles -> physics_comp[i].w -= decrement;
            particles -> physics_comp[i].h -= decrement;


        }
        else {
            dead_particles_num += 1; 
        }
    }

    if (dead_particles_num == le_get_max_particles_num()) {
        particles_dead = 1;   
    }
    else {
        particles_dead = 0;
    }
    return dead_particles_num;
}

emp draw_le_particles_t(le_particles_t * particles) {
    for (i32 i = 0; i < LE_MAX_PARTICLE_NUMBER; i++) {
        if (particles -> physics_comp[i].w > 2 
         && particles -> physics_comp[i].h > 2) {
            le_draw_le_entity_t(&particles -> physics_comp[i], 
                                &particles -> animation_comp[i]);
        }

    }
}
emp repopulate_le_particles_t(le_particles_t * particles, 
                               i32 min_rad, i32 max_rad, 
                               i32 x_min, i32 y_min, i32 x_max,
                               i32 y_max,
                               i32 min_accx,
                               i32 max_accx, i32 min_accy, 
                               i32 max_accy, i32 r, 
                               i32 g, i32 b, i32 n) {      

    for (i32 i = 0; i < n; i++) {
        i32 diam = randnum(min_rad, max_rad) * 2;
        particles -> physics_comp[i] = mk_le_entity_t(le_v2(
                                            randnum(x_min, x_max),
                                            randnum(y_min, y_max)), 
                                    le_v2(0, 0), le_v2(
                                                      randnum(
                                                        min_accx,
                                                        max_accx
                                                             ),
                                                      randnum(
                                                        min_accy,
                                                        max_accy
                                                             )), 
                                    diam, diam);



        particles -> animation_comp[i] = global_particle_image; 
        SDL_SetTextureColorMod(particles -> animation_comp[i].tex,
                               r, g, b);
    }       
}

i32 all_particles_dead() { return particles_dead; }
i32 le_get_max_particles_num() { return LE_MAX_PARTICLE_NUMBER; }

