#ifndef _light_engine_prof
    #define _light_engine_prof
    
    #include "types.h"

    typedef struct {
        le_str name;
        f64 elapsed;
    } le_prof_entry_t;

    emp    le_start_prof_func(le_prof_entry_t * prof, le_str name);
    le_str le_stop_prof_func(le_prof_entry_t * prof);

#endif
