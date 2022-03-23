#ifndef _light_engine_le_string 

    #define _light_engine_le_string
    
    #include "types.h"

    le_str le_str_st         (const le_str le_str_one, chr c);
    i32    le_str_length     (const le_str le_str_one);
    le_str le_str_concat     (const le_str le_str_one, const le_str le_str_two);
    i32    le_str_compare    (const le_str le_str_one, const le_str le_str_two);
    le_str le_str_reverse    (le_str le_str_one);
    i32    le_str_subchar    (le_str le_str_one, chr letter);
    i32    le_str_ends_with  (le_str le_str_one, le_str le_str_two);
    i32    le_str_sub_str    (const le_str le_str_one, const le_str le_str_two);
    i32    le_str_starts_with(const le_str le_str_one, const le_str le_str_two);
    le_str i32_to_le_str     (i32 num);
    i32    le_str_to_i32     (le_str le_string);


#endif 

