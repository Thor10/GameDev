#include <stdio.h>
#include <stdlib.h>

char * load_text_file(const char * path) {
    FILE * f = fopen(path, "r");
    
    if (f) {
        fseek(f, 0, SEEK_END);
        long length = ftell(f);

        char * buffer = (char *) malloc(length);
        rewind(f);
        fread(buffer, sizeof(char), length, f);
        buffer[length] = '\0';
        fclose(f);

        return buffer;
    }

    else {
        char * buffer = "File not found";
        return buffer;
    }
}


