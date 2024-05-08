#pragma once

/// @file Vector.h
/// @brief Vector class used in MCC
///
/// Layout
/// +----------+----------+----------+------------------+------------+
/// | 4 bytes  | 4 bytes  | 4 bytes  | 8 bytes          | N Elements |
/// +----------+----------+----------+------------------+------------+
/// ^ size     ^ capacity ^elem size ^ destructor ptr   ^ Vector pointer returned
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
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/// @brief Vector is an address to stored elements
typedef const void* Vector;

/// @brief Create a new Vector
/// @param reserve initial amount of elements to reserve
/// @param elem_size size of element
/// @return returns NULL on error, otherwise points to allocated block
inline Vector VectorCreate(uint32_t reserve, uint32_t elem_size, void (*destructor)(void*));

/// @brief Destroy Vector, dellocate memory reserved and calls destructor
/// @param self
inline void VectorDestroy(Vector self);

/// @brief Reserve `size` elements, this function WILL allocate memory, unless size == capacity
/// @param self
/// @param size number of elements to allocate, CANNOT be less than size
/// @return returns NULL on error, otherwise points to allocated block
inline Vector VectorReserve(Vector self, uint32_t size);

/// @brief Push an element via memcpy, this function MAY allocate memory
/// @param self
/// @param elem element to copy
/// @return returns NULL on error, otherwise points to allocated block
inline Vector VectorPush(Vector self, void* elem);

/// @brief Pop an element, calling it's destructor, this function does not allocate
/// @param self
/// @return returns NULL on error, otherwise points to allocated block
inline Vector VectorPop(Vector self);

/// @brief Length of Vector
/// @param self
/// @return Length of Vector in terms of elements
inline uint32_t VectorLength(Vector self);

/// @brief Capacity of Vector
/// @param self
/// @return Capacity of Vector in terms of elements
inline uint32_t VectorCapacity(Vector self);

/// @brief Element size of vector
/// @param self
/// @return Element size of Vector
inline uint32_t VectorElementSize(Vector self);

struct __VectorImpl {
    uint32_t size;
    uint32_t capacity;
    uint32_t elem_size;
    void (*destructor)(void*);
};

#define __VECTOR_HEADER_SIZE (sizeof(struct __VectorImpl))
#define __VECTOR_GET_IMPL(_V) ((struct __VectorImpl*)(((uint8_t*)_V) - __VECTOR_HEADER_SIZE))
#define __VECTOR_GET_VECTOR(_Impl) ((void*)(((uint8_t*)_Impl) + __VECTOR_HEADER_SIZE))

struct __VectorImpl* __VectorRealloc(struct __VectorImpl* impl, uint32_t n) {
    assert(impl != NULL);

    if (n == impl->capacity)
        return impl;

    struct __VectorImpl* new_impl;

    if ((new_impl = realloc(impl, n * impl->elem_size + __VECTOR_HEADER_SIZE)) == NULL) {
        if ((new_impl = malloc(n * impl->elem_size + __VECTOR_HEADER_SIZE)) == NULL)
            return NULL;

        memcpy(new_impl, impl, impl->size * impl->elem_size + __VECTOR_HEADER_SIZE);
        free(impl);
    }

    new_impl->capacity = n;

    return new_impl;
}

Vector VectorCreate(uint32_t reserve, uint32_t elem_size, void (*destructor)(void*)) {
    assert(elem_size > 0);

    struct __VectorImpl* impl;

    if ((impl = malloc((reserve * elem_size) + __VECTOR_HEADER_SIZE)) == NULL)
        return NULL;

    impl->size       = 0;
    impl->capacity   = reserve;
    impl->elem_size  = elem_size;
    impl->destructor = destructor;

    return __VECTOR_GET_VECTOR(impl);
}

void VectorDestroy(Vector self) {
    assert(self != NULL);

    struct __VectorImpl* impl = __VECTOR_GET_IMPL(self);

    for (uint32_t i = 0; i < impl->size; i++)
        if (impl->destructor != NULL)
            impl->destructor(((uint8_t*)self) + (i * impl->elem_size));

    free(impl);
}

Vector VectorReserve(Vector self, uint32_t size) {
    assert(self != NULL);

    struct __VectorImpl* impl = __VECTOR_GET_IMPL(self);

    if ((impl = __VectorRealloc(impl, size)) == NULL)
        return NULL;

    if (size < impl->size)
        impl->size = size;

    return __VECTOR_GET_VECTOR(impl);
}

Vector VectorPush(Vector self, void* elem) {
    assert(self != NULL && elem != NULL);

    struct __VectorImpl* impl = __VECTOR_GET_IMPL(self);

    if (impl->size == impl->capacity)
        if ((impl = __VectorRealloc(impl, impl->capacity ? impl->capacity * 2 : 1)) == NULL)
            return NULL;

    uint8_t* vector = __VECTOR_GET_VECTOR(impl);

    memcpy(&vector[impl->size * impl->elem_size], elem, impl->elem_size);
    impl->size++;

    return vector;
}

Vector VectorPop(Vector self) {
    assert(self != NULL);

    struct __VectorImpl* impl = __VECTOR_GET_IMPL(self);

    if (impl->destructor != NULL)
        impl->destructor(((uint8_t*)self) + (impl->size * impl->elem_size));

    impl->size--;

    return self;
}

uint32_t VectorLength(Vector self) {
    assert(self != NULL);

    return __VECTOR_GET_IMPL(self)->size;
}

uint32_t VectorCapacity(Vector self) {
    assert(self != NULL);

    return __VECTOR_GET_IMPL(self)->capacity;
}

uint32_t VectorElementSize(Vector self) {
    assert(self != NULL);

    return __VECTOR_GET_IMPL(self)->elem_size;
}
