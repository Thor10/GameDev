#include <file.h>
#include <stdio.h>
#include <stdlib.h>

le_str le_load_file(const le_str path) {
    FILE * f = fopen(path, "r");
    
    if (f) {
        fseek(f, 0, SEEK_END);
        i64 length = ftell(f);

        le_str buffer = (le_str) malloc(length * sizeof(char *));
        rewind(f);
        fread(buffer, sizeof(char), length, f);
        buffer[length] = '\0';
        fclose(f);

        return buffer;
    }

    else {
        le_str buffer = "File not found";
        return buffer;
    }
}


