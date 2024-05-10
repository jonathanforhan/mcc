#pragma once

/// @file Vector.h
/// @brief Vector class used in MCC
///
/// Layout (assuming 64-bit system, will be 4 bytes on a 32-bit)
/// +------------+------------+------------+------------+------------+
/// | 8 bytes    | 8 bytes    | 8 bytes    | 8 bytes    | N Elements |
/// +------------+------------+------------+------------+------------+
/// ^ size       ^ capacity   ^elem size   ^ destructor ^ Vector pointer returned
///
/// NOTE: if a member function returns `Vector`, the return value MUST be checked.
///       If the return value is NULL an error has occured, otherwise it is a pointer
///       to the Vector. `self` MAY be freed in that case and should be assigned to
///       the return value of the function.
///
/// NOTE:
///       the compiler most likely pads this data struct, ONLY use the member functions
///
/// Vector is not meant to be used by itself, it is instead meant to be used
/// to create inline functions for known types

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "Log.h"

/// @brief Vector is an address to stored elements
typedef const void* Vector;

/// @brief Create a new Vector
/// @param reserve initial amount of elements to reserve
/// @param elem_size size of element
/// @return returns NULL on error, otherwise points to allocated block
static inline Vector VectorCreate(size_t reserve, size_t elem_size, void (*destructor)(void*));

/// @brief Destroy Vector, dellocate memory reserved and calls destructor
/// @param self
static inline void VectorDestroy(Vector self);

/// @brief Reserve `size` elements, this function WILL allocate memory, unless size == capacity
/// @param self
/// @param size number of elements to allocate, CANNOT be less than size
/// @return returns false on error
static inline bool VectorReserve(Vector* self, size_t size);

/// @brief Clear all elements in Vector
/// @param self
static inline void VectorClear(Vector* self);

/// @brief Push an element via memcpy, this function MAY allocate memory
/// @param self
/// @param elem element to copy
/// @return returns false on error
static inline bool VectorPush(Vector* self, void* elem);

/// @brief Pop an element, calling it's destructor, this function does not allocate
/// @param self
static inline void VectorPop(Vector* self);

/// @brief Length of Vector
/// @param self
/// @return Length of Vector in terms of elements
static inline size_t VectorLength(Vector self);

/// @brief Capacity of Vector
/// @param self
/// @return Capacity of Vector in terms of elements
static inline size_t VectorCapacity(Vector self);

/// @brief Element size of vector
/// @param self
/// @return Element size of Vector
static inline size_t VectorElementSize(Vector self);

struct __VectorImpl {
    size_t size;
    size_t capacity;
    size_t elem_size;
    void (*destructor)(void*);
};

#define __VECTOR_HEADER_SIZE (sizeof(struct __VectorImpl))
#define __VECTOR_GET_IMPL(_V) ((struct __VectorImpl*)(((unsigned char*)_V) - __VECTOR_HEADER_SIZE))
#define __VECTOR_GET_VECTOR(_Impl) ((void*)(((unsigned char*)_Impl) + __VECTOR_HEADER_SIZE))

static inline struct __VectorImpl* __VectorRealloc(struct __VectorImpl* impl, size_t n) {
    assert(impl);

    if (n == impl->capacity)
        return impl;

    if (n < impl->size) {
        for (size_t i = n; i < impl->size; i++) {
            if (impl->destructor)
                impl->destructor(((unsigned char*)__VECTOR_GET_VECTOR(impl)) + (i * impl->elem_size));
        }
    }

    struct __VectorImpl* new_impl;

    if (!(new_impl = realloc(impl, n * impl->elem_size + __VECTOR_HEADER_SIZE))) {
        if (!(new_impl = malloc(n * impl->elem_size + __VECTOR_HEADER_SIZE))) {
            LOG_FATAL("%s", "failed malloc, possible heap corruption");
            return NULL;
        }

        memcpy(new_impl, impl, impl->size * impl->elem_size + __VECTOR_HEADER_SIZE);
        free(impl);
    }

    new_impl->capacity = n;

    return new_impl;
}

Vector VectorCreate(size_t reserve, size_t elem_size, void (*destructor)(void*)) {
    assert(elem_size > 0);

    struct __VectorImpl* impl;

    if (!(impl = malloc((reserve * elem_size) + __VECTOR_HEADER_SIZE)))
        return NULL;

    impl->size       = 0;
    impl->capacity   = reserve;
    impl->elem_size  = elem_size;
    impl->destructor = destructor;

    return __VECTOR_GET_VECTOR(impl);
}

void VectorDestroy(Vector self) {
    assert(self);

    struct __VectorImpl* impl = __VECTOR_GET_IMPL(self);

    for (size_t i = 0; i < impl->size; i++) {
        if (impl->destructor)
            impl->destructor(((unsigned char*)self) + (i * impl->elem_size));
    }

    free(impl);
}

bool VectorReserve(Vector* self, size_t size) {
    assert(self && *self);

    struct __VectorImpl* impl = __VECTOR_GET_IMPL(*self);

    if (!(impl = __VectorRealloc(impl, size)))
        return false;

    if (size < impl->size)
        impl->size = size;

    *self = __VECTOR_GET_VECTOR(impl);
    return true;
}

void VectorClear(Vector* self) {
    assert(self && *self);

    struct __VectorImpl* impl = __VECTOR_GET_IMPL(*self);
    unsigned char* vector     = __VECTOR_GET_VECTOR(impl);

    for (size_t i = 0; i < impl->size; i++)
        if (impl->destructor)
            impl->destructor(vector + (i * impl->elem_size));

    impl->size = 0;

    *self = vector;
}

bool VectorPush(Vector* self, void* elem) {
    assert(self && *self && elem);

    struct __VectorImpl* impl = __VECTOR_GET_IMPL(*self);

    if (impl->size == impl->capacity)
        if (!(impl = __VectorRealloc(impl, impl->capacity ? impl->capacity * 2 : 1)))
            return false;

    unsigned char* vector = __VECTOR_GET_VECTOR(impl);

    memcpy(&vector[impl->size * impl->elem_size], elem, impl->elem_size);
    impl->size++;

    return (*self = vector);
}

void VectorPop(Vector* self) {
    assert(self && *self);

    struct __VectorImpl* impl = __VECTOR_GET_IMPL(*self);

    if (impl->destructor)
        impl->destructor(((unsigned char*)*self) + ((impl->size - 1) * impl->elem_size));

    impl->size--;
}

size_t VectorLength(Vector self) {
    assert(self);

    return __VECTOR_GET_IMPL(self)->size;
}

size_t VectorCapacity(Vector self) {
    assert(self);

    return __VECTOR_GET_IMPL(self)->capacity;
}

size_t VectorElementSize(Vector self) {
    assert(self);

    return __VECTOR_GET_IMPL(self)->elem_size;
}
