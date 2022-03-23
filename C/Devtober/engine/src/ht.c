#include <ht.h>
#include <stdlib.h>

#define SIZE 10000
le_ht_item * table[SIZE];

emp le_init_table() {
    //table = calloc(SIZE, sizeof(le_ht_item));
    for (int i = 0; i < SIZE; i++) {
        table[i] = NULL;
    }
}

i32 le_get_tablesize() {
    return SIZE;
}

i32 le_gen_hash(le_str id) {
    i32 hash = 0;
    for (int i = 0; i < le_str_length(id); i++) {
        hash += (i^id[i]);
    }

    return hash % le_get_tablesize();
}

emp le_add_to_table(le_ht_item * item) {
    if (table[item -> hash] == NULL) {
        table[item -> hash] = item;        
        return;
    }
    if (table[item -> hash] != NULL) {
        le_ht_item * curr_node = table[item -> hash] -> next;
        while (curr_node != NULL) {
            curr_node = curr_node -> next; 
        }
        curr_node = item;
        return;
    }
}


ret le_get_value(le_str id) {
    i32 hash = le_gen_hash(id);
    if (table[hash] -> val == NULL) {
        return NULL;
    }

    if (table[hash] -> id == id) {
        return table[hash] -> val;
    }
    
    if (table[hash] -> id != id) {
        le_ht_item * curr_node = table[hash] -> next;
        while (curr_node -> id != id) {
            curr_node = curr_node -> next;
        }
        return curr_node -> val;
    }

}
