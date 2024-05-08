#include "Vector.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct _VectorImpl {
    uint32_t size;
    uint32_t capacity;
    uint32_t elem_size;
    void (*destructor)(void*);
} _VectorImpl;

#define HEADER_SIZE (sizeof(_VectorImpl))

#define GET_IMPL(_V) ((_VectorImpl*)(((unsigned char*)_V) - HEADER_SIZE))
#define GET_VECTOR(_Impl) ((void*)(((unsigned char*)_Impl) + HEADER_SIZE))

static _VectorImpl* _Realloc(_VectorImpl* impl, uint32_t n) {
    assert(impl != NULL);

    if (n == impl->capacity)
        return impl;

    _VectorImpl* new_impl;

    if ((new_impl = realloc(impl, n * impl->elem_size + HEADER_SIZE)) == NULL) {
        if ((new_impl = malloc(n * impl->elem_size + HEADER_SIZE)) == NULL)
            return NULL;

        memcpy(new_impl, impl, impl->size * impl->elem_size + HEADER_SIZE);
        free(impl);
    }

    new_impl->capacity = n;

    return new_impl;
}

Vector VectorCreate(uint32_t reserve, uint32_t elem_size, void (*destructor)(void*)) {
    _VectorImpl* impl;

    if ((impl = malloc((reserve * elem_size) + HEADER_SIZE)) == NULL)
        return NULL;

    impl->size       = 0;
    impl->capacity   = reserve;
    impl->destructor = destructor;
    impl->elem_size  = elem_size;

    return GET_VECTOR(impl);
}

void VectorDestroy(Vector self) {
    _VectorImpl* impl = GET_IMPL(self);

    for (uint32_t i = 0; i < impl->size; i++)
        if (impl->destructor != NULL)
            impl->destructor(((unsigned char*)self) + (i * impl->elem_size));

    free(impl);
}

Vector VectorReserve(Vector self, uint32_t size) {
    _VectorImpl* impl = GET_IMPL(self);

    if ((impl = _Realloc(impl, size)) == NULL)
        return NULL;

    if (size < impl->size)
        impl->size = size;

    return GET_VECTOR(impl);
}

Vector VectorPush(Vector self, void* elem) {
    _VectorImpl* impl = GET_IMPL(self);

    if (impl->size == impl->capacity) {
        if ((impl = _Realloc(impl, impl->capacity == 0 ? 1 : impl->capacity * 2)) == NULL)
            return NULL;
    }

    unsigned char* vector = GET_VECTOR(impl);

    memcpy(&vector[impl->size * impl->elem_size], elem, impl->elem_size);
    impl->size++;

    return vector;
}

Vector VectorPop(Vector self) {
    _VectorImpl* impl = GET_IMPL(self);

    if (impl->destructor != NULL)
        impl->destructor(((unsigned char*)self) + (impl->size * impl->elem_size));

    impl->size--;

    return self;
}

uint32_t VectorLength(Vector self) {
    return GET_IMPL(self)->size;
}

uint32_t VectorCapacity(Vector self) {
    _VectorImpl* impl = GET_IMPL(self);

    return impl->capacity;
}

uint32_t VectorElementSize(Vector self) {
    return GET_IMPL(self)->elem_size;
}
