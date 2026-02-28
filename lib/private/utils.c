#include "utils.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char* read_file(const char* path, size_t* bytes_read) {
    FILE* file   = NULL;
    char* buffer = NULL;

    if (!(file = fopen(path, "rb"))) { // "rb" for binary mode

        goto l_abort;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (file_size <= 0) {
        fprintf(stderr, "Error: File '%s' is empty or error occurred\n", path);
        goto l_abort;
    }
    fseek(file, 0, SEEK_SET);

    if (!(buffer = (char*)malloc(file_size + 1))) {
        fprintf(stderr, "Error: Could not allocate memory for file '%s'\n", path);
        goto l_abort;
    }

    size_t read_size = fread(buffer, 1, file_size, file);
    if (read_size != (size_t)file_size) {
        fprintf(stderr, "Error: Could not read file '%s' (read %zu of %ld bytes)\n", path, read_size, file_size);
        goto l_abort;
    }
    buffer[file_size] = '\0';

    fclose(file);

    *bytes_read = file_size;
    return buffer;

l_abort:
    if (file != NULL) {
        fclose(file);
    }
    free(buffer);
    return NULL;
}
