#pragma once

/// @file String.h
/// @brief String class used in MCC
///
/// Layout
/// +--------------+------------------+------------------+-----------+
/// | 4 bytes size | 4 bytes capacity | N bytes of chars | null byte |
/// +--------------+------------------+------------------+-----------+
///                                   ^ String pointer returned from member functions
///
/// NOTE: if a member function returns `String`, the return value MUST be checked.
///       If the return value is NULL an error has occured, otherwise it is a pointer
///       to the String. `self` MAY be freed in that case and should be assigned to
///       the return value of the function.

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/// @brief String is simply an address to the stored chars
typedef const char* String;

/// @brief Create a new String
/// @param str initial string value, can be NULL
/// @param reserve initial amount of space to reserve NOT including NULL byte
/// @return returns NULL on error, otherwise points to allocated string
inline String StringCreate(const char* str, uint32_t reserve);

/// @brief Destroy String, deallocate memory reserved
/// @param self
inline void StringDestroy(String self);

/// @brief Reserve `size` bytes, this function WILL allocate memory, unless size == capacity
/// @param self
/// @param size number of bytes to allocate, equivalent to number of chars CAN be less than size
/// @return returns NULL on error, otherwise is a pointer to String
inline String StringReserve(String self, uint32_t size);

/// @brief Clear all chars in String
/// @param self
/// @return returns NULL on error, otherwise is a pointer to String
inline String StringClear(String self);

/// @brief Push a char to String, this function MAY allocate memory
/// @param self
/// @param c char to append
/// @return returns NULL on error, otherwise is a pointer to String
inline String StringPush(String self, char c);

/// @brief Pop a char to String, this function does not allocate memory
/// @param self
/// @return returns a pointer to String
inline String StringPop(String self);

/// @brief Append another String to String, this function MAY allocate memory
/// @param self
/// @param other String to append, this String is preserved as-is
/// @return returns NULL on error, otherwise is a pointer to String
inline String StringAppend(String self, const char* other);

/// @brief Copy String, this function WILL allocate memory
/// @param self
/// @return returns NULL on error, otherwise is a pointer to a new String
inline String StringCopy(String self);

/// @brief Trim String to specified size DOES NOT deallocate
/// @param self
/// @param n number of elements to trim
/// @return returns a pointer to String
inline String StringTrim(String self, uint32_t n);

/// @brief Strink String to scecified size DOES deallocate
/// @param self
/// @param n number of elements to shrink
/// @return returns a pointer to String
inline String StringShrink(String self, uint32_t n);

/// @brief Length of String
/// @param self
/// @return returns length of chars NOT including header or null byte
inline uint32_t StringLength(String self);

/// @brief Capacity of String
/// @param self
/// @return returns capacity allocated memory NOT including header
inline uint32_t StringCapacity(String self);

struct __StringImpl {
    uint32_t size;
    uint32_t capacity;
};

#define __STRING_HEADER_SIZE (sizeof(struct __StringImpl))
#define __STRING_PADDING (__STRING_HEADER_SIZE + sizeof(char))

#define __STRING_GET_IMPL(_S) ((struct __StringImpl*)(((char*)_S) - __STRING_HEADER_SIZE))
#define __STRING_GET_STRING(_Impl) (((char*)_Impl) + __STRING_HEADER_SIZE)

struct __StringImpl* __StringRealloc(struct __StringImpl* impl, uint32_t n) {
    assert(impl != NULL);

    if (n + 1 == impl->capacity)
        return impl;

    struct __StringImpl* new_impl;

    if ((new_impl = realloc(impl, n + __STRING_PADDING)) == NULL) {
        if ((new_impl = malloc(n + __STRING_PADDING)) == NULL)
            return NULL;

        memcpy(new_impl, impl, impl->size + __STRING_PADDING);
        free(impl);
    }

    new_impl->capacity = n + 1;

    return new_impl;
}

String StringCreate(const char* str, uint32_t reserve) {
    struct __StringImpl* impl;

    if ((impl = malloc(reserve + __STRING_PADDING)) == NULL)
        return NULL;

    impl->capacity = reserve + 1;

    char* string = __STRING_GET_STRING(impl);

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
    assert(self != NULL);

    free(__STRING_GET_IMPL(self));
}

String StringReserve(String self, uint32_t size) {
    assert(self != NULL);

    struct __StringImpl* impl;

    if ((impl = __StringRealloc(__STRING_GET_IMPL(self), size)) == NULL)
        return NULL;

    if (size < impl->size)
        impl->size = size;

    char* string = __STRING_GET_STRING(impl);

    string[impl->size] = '\0';
    return string;
}

String StringClear(String self) {
    assert(self != NULL);

    struct __StringImpl* impl = __STRING_GET_IMPL(self);
    char* string              = __STRING_GET_STRING(impl);

    impl->size = 0;

    string[impl->size] = '\0';
    return string;
}

String StringPush(String self, char c) {
    assert(self != NULL);

    struct __StringImpl* impl = __STRING_GET_IMPL(self);

    if (impl->size + 1 == impl->capacity)
        if ((impl = __StringRealloc(impl, impl->capacity * 2)) == NULL)
            return NULL;

    char* string = __STRING_GET_STRING(impl);

    string[impl->size] = c;
    impl->size++;

    string[impl->size] = '\0';
    return string;
}

String StringPop(String self) {
    assert(self != NULL);

    struct __StringImpl* impl = __STRING_GET_IMPL(self);
    char* string              = __STRING_GET_STRING(impl);

    impl->size--;

    string[impl->size] = '\0';
    return string;
}

String StringAppend(String self, const char* other) {
    assert(self != NULL && other != NULL);

    struct __StringImpl* impl = __STRING_GET_IMPL(self);

    uint32_t n = strlen(other);

    if (impl->size + n >= impl->capacity) {
        uint32_t new_capacity = impl->capacity;

        do {
            new_capacity *= 2;
        } while (new_capacity <= impl->size + n);

        if ((impl = __StringRealloc(impl, new_capacity)) == NULL)
            return NULL;
    }

    char* string = __STRING_GET_STRING(impl);

    strcpy(string + impl->size, other);
    impl->size += n;

    assert(string[impl->size] == '\0');
    return string;
}

String StringCopy(String self) {
    assert(self != NULL);

    return StringCreate(self, __STRING_GET_IMPL(self)->size);
}

String StringTrim(String self, uint32_t n) {
    assert(self != NULL);

    struct __StringImpl* impl = __STRING_GET_IMPL(self);

    assert(n <= impl->size);

    char* string = __STRING_GET_STRING(impl);

    impl->size -= n;

    string[impl->size] = '\0';
    return string;
}

String StringShrink(String self, uint32_t n) {
    assert(self != NULL);

    struct __StringImpl* impl = __STRING_GET_IMPL(self);

    assert(n <= impl->size);

    impl->size -= n;
    return StringReserve(self, impl->size);
}

uint32_t StringLength(String self) {
    assert(self != NULL);

    return __STRING_GET_IMPL(self)->size;
}

uint32_t StringCapacity(String self) {
    assert(self != NULL);

    return __STRING_GET_IMPL(self)->capacity;
}
