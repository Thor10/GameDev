#include <types.h>
#include <time.h>
#include <timer.h>

emp init_le_timer_t(le_timer_t * timer) {
    timer -> sec_passed  = 0;
}

emp store_curr_time(le_timer_t * timer) {
    timer -> last_time = clock();
}


emp store_sec_passed(le_timer_t * timer) {
    timer -> sec_passed =  (f64)(clock() - timer -> last_time) / CLOCKS_PER_SEC; 
}


