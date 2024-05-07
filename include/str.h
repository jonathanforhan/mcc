#pragma once

#include <stdint.h>
#include "mcc.h"

typedef struct String {
    uint32_t size;
    uint32_t capacity;
    char* data;
} String;

String* StringCreate(void);

void StringDestroy(String* self);

Result StringAppend(String* self, char c);
