#include "String.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct _StringImpl {
    uint32_t size;
    uint32_t capacity;
} _StringImpl;

#define HEADER_SIZE (sizeof(_StringImpl))
#define PADDING (HEADER_SIZE + sizeof(char))

#define GET_IMPL(_S) ((_StringImpl*)(((char*)_S) - HEADER_SIZE))
#define GET_STRING(_Impl) (((char*)_Impl) + HEADER_SIZE)

static _StringImpl* _Realloc(_StringImpl* impl, uint32_t n) {
    assert(impl != NULL);

    if (n + 1 == impl->capacity)
        return impl;

    _StringImpl* new_impl;

    if ((new_impl = realloc(impl, n + PADDING)) == NULL) {
        if ((new_impl = malloc(n + PADDING)) == NULL)
            return NULL;

        memcpy(new_impl, impl, impl->size + PADDING);
        free(impl);
    }

    new_impl->capacity = n + 1;

    return new_impl;
}

String StringCreate(const char* str, uint32_t reserve) {
    _StringImpl* impl;

    if ((impl = malloc(reserve + PADDING)) == NULL)
        return NULL;

    impl->capacity = reserve + 1;

    char* string = GET_STRING(impl);

    if (str != NULL) {
        impl->size = strnlen(str, reserve);
        strncpy(string, str, impl->size);
    } else {
        impl->size = 0;
    }

    string[impl->size] = '\0';
    return string;
}

void StringDestroy(String self) {
    free(GET_IMPL(self));
}

String StringReserve(String self, uint32_t size) {
    _StringImpl* impl;

    if ((impl = _Realloc(GET_IMPL(self), size)) == NULL)
        return NULL;

    if (size < impl->size)
        impl->size = size;

    char* string = GET_STRING(impl);

    string[impl->size] = '\0';
    return string;
}

String StringPush(String self, char c) {
    _StringImpl* impl = GET_IMPL(self);

    if (impl->size + 1 == impl->capacity) {
        if ((impl = _Realloc(impl, impl->capacity * 2)) == NULL)
            return NULL;
    }

    char* string = GET_STRING(impl);

    string[impl->size] = c;
    impl->size++;

    string[impl->size] = '\0';
    return string;
}

String StringPop(String self) {
    _StringImpl* impl = GET_IMPL(self);
    char* string      = GET_STRING(impl);

    impl->size--;

    string[impl->size] = '\0';
    return string;
}

String StringAppend(String self, const char* other) {
    _StringImpl* impl = GET_IMPL(self);

    uint32_t n = strlen(other);

    if (impl->size + n >= impl->capacity) {
        uint32_t new_capacity = impl->capacity;

        do {
            new_capacity *= 2;
        } while (new_capacity <= impl->size + n);

        if ((impl = _Realloc(impl, new_capacity)) == NULL)
            return NULL;
    }

    char* string = GET_STRING(impl);

    strcpy(string + impl->size, other);
    impl->size += n;

    assert(string[impl->size] == '\0');
    return string;
}

String StringCopy(String self) {
    _StringImpl* impl = GET_IMPL(self);
    return StringCreate(self, impl->size);
}

String StringTrim(String self, uint32_t n) {
    _StringImpl* impl = GET_IMPL(self);

    assert(n <= impl->size);

    char* string = GET_STRING(impl);

    impl->size -= n;

    string[impl->size] = '\0';
    return string;
}

String StringShrink(String self, uint32_t n) {
    _StringImpl* impl = GET_IMPL(self);

    assert(n <= impl->size);

    impl->size -= n;
    return StringReserve(self, impl->size);
}

uint32_t StringLength(String self) {
    return GET_IMPL(self)->size;
}

uint32_t StringCapacity(String self) {
    return GET_IMPL(self)->capacity;
}
