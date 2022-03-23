#ifndef _light_engine_grid
    
    #define _light_engine_grid
    
    #include "types.h"
    #include "entity.h"
    
    typedef struct {
        i32 bounds[2];
        i32 dimensions[2];
        le_entity_t * ent;
    } grid_t;

#endif
