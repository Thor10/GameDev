#ifndef _light_engine_timer
    
    #define _light_engine_timer

    #include "types.h"
    #include <time.h>

    typedef struct {
        f64     sec_passed;  // 64-bit float for seconds passed
        clock_t last_time; // clock_t for last_time
    } le_timer_t;
    
    emp init_le_timer_t (le_timer_t * timer);
    emp store_curr_time (le_timer_t * timer);
    emp store_sec_passed(le_timer_t * timer);

#endif

