#include <entity.h>
#include <stdlib.h>
#include <engine.h>

le_entity_t * le_create_entities(i32 entity_num, i32 w, i32 h) {
    le_entity_t * ents = calloc(entity_num, sizeof(le_entity_t)); 

    for (int i = 0; i < entity_num; i++) {
        ents[i] = mk_le_entity_t(le_v2(0, 0), le_v2(0, 0), le_v2(0, 0), w, h);
    }
    return ents;
}

