#include "utils.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char* read_file(const char* path, size_t* bytes_read) {
    FILE* file   = NULL;
    char* buffer = NULL;

    file = fopen(path, "rb");
    if (!file) { // "rb" for binary mode
        perror("fopen");
        goto l_abort;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size;
    {
        long ret = ftell(file);
        if (ret < 0) {
            perror("ftell");
            goto l_abort;
        }
        file_size = (size_t)ret;
    }
    fseek(file, 0, SEEK_SET);

    buffer = malloc(file_size + 1);
    if (!buffer) {
        perror("malloc");
        goto l_abort;
    }

    size_t read_size = fread(buffer, 1, file_size, file);
    if (read_size != file_size) {
        perror("fread");
        goto l_abort;
    }
    buffer[file_size] = '\0';

    fclose(file);

    *bytes_read = file_size;
    return buffer;

l_abort:
    if (file) {
        fclose(file);
    }
    free(buffer);
    return NULL;
}
