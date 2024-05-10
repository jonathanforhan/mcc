#pragma once

#include <assert.h>
#include <stdbool.h>
#include "String.h"

bool GetTestPath(const char* test_paths[], size_t n, String* filepath) {
    assert(StringLength(*filepath) == 0);
    assert(StringCapacity(*filepath) >= 128);
    assert(test_paths && n > 0);

    void* fptr;

    for (int i = 0; i < n; i++) {
        if ((fptr = fopen(test_paths[i], "r"))) {
            fclose(fptr);
            assert(StringAppend(filepath, test_paths[i]));
            return true;
        }
    }

    return false;
}
