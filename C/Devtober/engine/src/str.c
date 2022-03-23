#include <types.h>
#include <str.h>
#include <stdlib.h>
#include <stdio.h>


i32 le_str_length(const le_str le_str_one) {
    i32 len;
    for (len = 0; le_str_one[len] != '\0'; len++);
    return len;
}

i32 le_str_compare(const le_str le_str_one, const le_str le_str_two) {
    if (le_str_length(le_str_one) != le_str_length(le_str_two)) {
        return 0;
    }
    
    i32 i = 0;
    i32 flag = 0;

    while (le_str_one[i] != '\0' && le_str_two[i] != '\0') {
        if (le_str_one[i] != le_str_two[i]) {
            flag = 1;
            break;
        }

        i++;
    }

    if (flag == 0 && le_str_one[i] == '\0' && le_str_two[i] == '\0') {
        return 1;
    }

    else {
        return 0;
    }
} 

le_str le_str_concat (const le_str le_str_one, const le_str le_str_two) {    
    i32 size_le_str_one = le_str_length(le_str_one);
    i32 size_le_str_two = le_str_length(le_str_two);
    le_str new_char = (le_str) malloc(size_le_str_one + size_le_str_two + 1);
    i32 i; 
    for (i = 0; i < size_le_str_one; i++) {
        new_char[i] = le_str_one[i];
    }

    for (i = 0; i < size_le_str_two; i++) {
        new_char[i + size_le_str_one] = le_str_two[i];
    }

    new_char[size_le_str_two + size_le_str_one] = '\0';

    return new_char;
}

i32 le_str_starts_with(const le_str le_str_one, const le_str le_str_two) {
    i32 len = le_str_length(le_str_two);
    i32 flag = 0;
    i32 i;
    for (i = 0; i < len; i++) {
        if (le_str_one[i] != le_str_two[i]) {
            flag = 0;
            break;
        }
    }

    if (flag == 0) return 1;
    return 0;
}

le_str le_str_reverse(le_str le_str_one) {
    i32 len = le_str_length(le_str_one) - 1;
    le_str new_char = (le_str) malloc(len + 1); 
    i32 i; 
    for (i = 0; i < le_str_length(le_str_one); i++) {
        new_char[i] = le_str_one[len - i];
    }

    return new_char;
}

i32 le_str_ends_with(le_str le_str_one, le_str le_str_two) {
    le_str new_le_str = le_str_reverse(le_str_one);
    le_str new_le_str_o = le_str_reverse(le_str_two);

    return le_str_starts_with(new_le_str, new_le_str_o);
}

i32 le_str_sub_str(const le_str le_str_one, const le_str le_str_two) {
    i32 i = 0;
    i32 j = 0;

    while (le_str_one[j] != '\0' && le_str_two[i] != '\0') {
        if (le_str_two[i] != le_str_one[j]) {
            j++;
            i = 0;
        }

        else {
            i++;
            j++;
        }
    }

    if (le_str_two[i] == '\0') {
        return 1;
    }

    else {
        return 0;
    }
}

le_str le_str_split(const le_str le_str_one, chr c) {
    le_str new_char = (le_str) malloc(le_str_length(le_str_one));

    i32 i;
    for (i = 0; i < le_str_length(le_str_one); i++) {
        if (le_str_one[i] == c) {
            break;
        }

        new_char[i] = le_str_one[i];
    }

    return new_char;
}

le_str i32_to_le_str(i32 num) {
    le_str le_string = (le_str) malloc(40); 
    snprintf(le_string, 39, "%d", num); 
    le_string[40] = '\0';
    return le_string;
}

/* From: https://github.com/TheAlgorithms/C */
i32 le_str_to_i32(char * le_string) {
    int i;
    int sign;
    long value;
    long prev;

    i = 0;
    sign = 1;
    value = 0;

    while (((le_string[i] <= 13 && le_string[i] >= 9) || le_string[i] == 32) && le_string[i] != '\0')
        i++;

    if (le_string[i] == '-') {
        sign = -1;
        i++;
    }

    else if (le_string[i] == '+') {
        sign = 1;
        i++;
    }

    while (le_string[i] >= 48 && le_string[i] <= 57 && le_string[i] != '\0') {
        prev = value;
        value = value * 10 + sign * (le_string[i] - '0');

        if (sign == 1 && prev > value)
            return (-1);
        else if (sign == -1 && prev < value)
            return (0);
        i++;
    }
    return (value); 
}


le_str f64_to_le_str(f64 num) {
    le_str le_string = (le_str) malloc(40); 
    snprintf(le_string, 39, "%f", num); 
    le_string[40] = '\0';
    return le_string;   
}

