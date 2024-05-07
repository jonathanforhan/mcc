#include "str.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static Result _StringRealloc(String* self) {
    uint32_t new_capacity = self->capacity == 0 ? 2 : self->capacity * 2; // need 2 to start for NULL byte
    char* new_data        = realloc(self->data, sizeof(char) * new_capacity);

    if (new_data == NULL) {
        new_data = malloc(sizeof(char) * new_capacity);

        if (new_data == NULL) {
            return OUT_OF_MEMORY;
        } else {
            if (self->data != NULL)
                memcpy(new_data, self->data, self->size);

            free(self->data);
        }
    }

    self->capacity = new_capacity;
    self->data     = new_data;
    return SUCCESS;
}

String* StringCreate(void) {
    return calloc(1, sizeof(String));
}

void StringDestroy(String* self) {
    free(self->data);
    free(self);
}

Result StringAppend(String* self, char c) {
    if (self->size >= self->capacity - 1 || self->capacity == 0) { // account for NULL byte and unsigned capacity
        Result result = _StringRealloc(self);

        if (result != SUCCESS)
            return result;
    }

    assert(self->data != NULL);

    self->data[self->size]     = c;
    self->data[self->size + 1] = '\0';
    self->size++;

    return SUCCESS;
}
