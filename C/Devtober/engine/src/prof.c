
#include <prof.h>
#include <stdio.h>
#include <str.h>
#include <stdlib.h>
#include <time.h>

clock_t time_s;
clock_t time_e;

emp le_start_prof_func(le_prof_entry_t * prof, le_str name) {
    prof -> name = name;
    prof -> elapsed = 0;
    time_s = clock();
    
}

le_str le_stop_prof_func(le_prof_entry_t * prof) {
    time_e = clock();
    prof -> elapsed = (f64)(time_e - time_s) / CLOCKS_PER_SEC;
    le_str number = f64_to_le_str(prof -> elapsed); 
    le_str stats = "TIME TOOK FOR ";
    le_str concat_stats = le_str_concat(stats, prof -> name);
    le_str concat_stats_sp = le_str_concat(concat_stats, ": ");
    le_str concatted_final = le_str_concat(concat_stats_sp, number);
    le_str concatted_finale = le_str_concat(concatted_final, "s");

    return concatted_finale;
}




