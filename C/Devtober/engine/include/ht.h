#ifndef _light_engine_ht

    #define _light_engine_ht
    #include "types.h" 
    #include "str.h"

    typedef struct _le_ht_item {
        le_str id;   // id = key, type = le_ent (example)
        i32    hash;
        ret    val;
        le_str type;
        struct _le_ht_item * next;
    } le_ht_item;

    emp le_init_table();
    i32 le_get_tablesize();
    i32 le_gen_hash(le_str id);
    emp le_add_to_table(le_ht_item * item);
    ret le_get_value(le_str id);

    #define mk_le_ht_item(_id, _val, _type) \
        (le_ht_item) { _id, le_gen_hash(_id), (ret)_val, _type, NULL }

    
#endif
