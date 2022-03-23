#ifndef _light_engine_entity
    #define _light_engine_entity
    #include "maths.h"

    typedef struct {
        le_vec2_t pos; // Position
        le_vec2_t vel; // Velocity 
        le_vec2_t acc; // Acceleration
        f32 w;         // Width
        f32 h;         // Height
        u08 coll_l;    // Left Collision
        u08 coll_r;    // Right Collision
        u08 coll_b;    // Bottom Collision
        u08 coll_t;    // Top Collision
        // add an id?
    } le_entity_t;

    #define mk_le_entity_t(_pos, _vel, _acc, _w, _h) (le_entity_t)\
        { .pos = _pos, .vel = _vel, .acc = _acc, .w = _w  - 2, .h = _h - 2, \
          .coll_l = 0, .coll_r = 0, .coll_b = 0, .coll_t = 0       \
        };
    
    le_entity_t * le_create_entities(i32 entity_num, i32 w, i32 h);
    /* 
     * Best detection using SAT 
     * Separating Axis Theorem
     * Add x velocity then check 
     * horizontal collisions. Then
     * do the same thing for vertical
     * collisions
    */ 
    #define collision_detect(m, l) \
        ((((m.pos.x + m.w >= l.pos.x) &&  \
        (l.pos.x + l.w >= m.pos.x)) &&  \
        (m.pos.y + m.h >= l.pos.y)) &&  \
        (l.pos.y + l.h >= m.pos.y))     \

#endif

