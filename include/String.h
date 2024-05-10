#pragma once

/// @file String.h
/// @brief String class used in MCC
///
/// Layout (assuming 64-bit system, will be 4 bytes on a 32-bit)
/// +--------------+------------------+------------------+-----------+
/// | 8 bytes size | 8 bytes capacity | N bytes of chars | null byte |
/// +--------------+------------------+------------------+-----------+
///                                   ^ String pointer returned from member functions
///
/// NOTE: if a member function returns `String`, the return value MUST be checked.
///       If the return value is NULL an error has occured, otherwise it is a pointer
///       to the String. `self` MAY be freed in that case and should be assigned to
///       the return value of the function.

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "Log.h"

/// @brief String is simply an address to the stored chars
typedef const char* String;

/// @brief Create a new String
/// @param str initial string value, can be NULL
/// @param reserve initial amount of space to reserve NOT including NULL byte
/// @return returns NULL on error, otherwise points to allocated string
static inline String StringCreate(const char* str, size_t reserve);

/// @brief Destroy String, deallocate memory reserved
/// @param self
static inline void StringDestroy(String self);

/// @brief Reserve `size` bytes, this function WILL allocate memory, unless size == capacity
/// @param self
/// @param size number of bytes to allocate, equivalent to number of chars CAN be less than size
/// @return returns false on error
static inline bool StringReserve(String* self, size_t size);

/// @brief Clear all chars in String
/// @param self
static inline void StringClear(String* self);

/// @brief Push a char to String, this function MAY allocate memory
/// @param self
/// @param c char to append
/// @return returns false on error
static inline bool StringPush(String* self, char c);

/// @brief Pop a char to String, this function does not allocate memory
/// @param self
/// @return returns false on error
static inline void StringPop(String* self);

/// @brief Append another String to String, this function MAY allocate memory
/// @param self
/// @param other String to append, this String is preserved as-is
/// @return returns false on error
static inline bool StringAppend(String* self, const char* other);

/// @brief Copy String, this function WILL allocate memory
/// @param self
/// @param dest destination to copy self to
/// @return returns false on error
static inline bool StringCopy(String self, String* dest);

/// @brief Trim String to specified size DOES NOT deallocate
/// @param self
/// @param n number of elements to trim
/// @return returns false on error
static inline void StringTrim(String* self, size_t n);

/// @brief Strink String to scecified size DOES deallocate
/// @param self
/// @param n number of elements to shrink
/// @return returns false on error
static inline bool StringShrink(String* self, size_t n);

/// @brief Does String contain substring
/// @param self
/// @param substring string to check for
/// @param substring_size size of substring
/// @param case_sensitive is comparision case sensitive
/// @return returns true if contains substring
static inline bool StringContains(String self, const char* substring, size_t substring_size, bool case_sensitive);

/// @brief Length of String
/// @param self
/// @return returns length of chars NOT including header or null byte
static inline size_t StringLength(String self);

/// @brief Capacity of String
/// @param self
/// @return returns capacity allocated memory NOT including header
static inline size_t StringCapacity(String self);

struct __StringImpl {
    size_t size;
    size_t capacity;
};

#define __STRING_HEADER_SIZE (sizeof(struct __StringImpl))
#define __STRING_PADDING (__STRING_HEADER_SIZE + sizeof(char))

#define __STRING_GET_IMPL(_S) ((struct __StringImpl*)(((char*)_S) - __STRING_HEADER_SIZE))
#define __STRING_GET_STRING(_Impl) (((char*)_Impl) + __STRING_HEADER_SIZE)

static inline struct __StringImpl* __StringRealloc(struct __StringImpl* impl, size_t n) {
    assert(impl != NULL);

    if (n + 1 == impl->capacity)
        return impl;

    struct __StringImpl* new_impl;

    if (!(new_impl = realloc(impl, n + __STRING_PADDING))) {
        if (!(new_impl = malloc(n + __STRING_PADDING))) {
            LOG_FATAL("%s", "failed malloc, possible heap corruption");
            return NULL;
        }

        memcpy(new_impl, impl, impl->size + __STRING_PADDING);
        free(impl);
    }

    new_impl->capacity = n + 1;

    return new_impl;
}

String StringCreate(const char* str, size_t reserve) {
    struct __StringImpl* impl;

    if (!(impl = malloc(reserve + __STRING_PADDING)))
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
    assert(self);

    free(__STRING_GET_IMPL(self));
}

bool StringReserve(String* self, size_t size) {
    assert(self && *self);

    struct __StringImpl* impl;

    if (!(impl = __StringRealloc(__STRING_GET_IMPL(*self), size)))
        return false;

    if (size < impl->size)
        impl->size = size;

    char* string = __STRING_GET_STRING(impl);

    string[impl->size] = '\0';
    *self              = string;

    return true;
}

void StringClear(String* self) {
    assert(self && *self);

    struct __StringImpl* impl = __STRING_GET_IMPL(*self);
    impl->size                = 0;

    char* string = __STRING_GET_STRING(impl);

    string[impl->size] = '\0';
    *self              = string;
}

bool StringPush(String* self, char c) {
    assert(self && *self);

    struct __StringImpl* impl = __STRING_GET_IMPL(*self);

    if (impl->size + 1 == impl->capacity)
        if (!(impl = __StringRealloc(impl, impl->capacity * 2)))
            return false;

    char* string = __STRING_GET_STRING(impl);

    string[impl->size] = c;
    impl->size++;

    string[impl->size] = '\0';
    *self              = string;

    return true;
}

void StringPop(String* self) {
    assert(self && *self);

    struct __StringImpl* impl = __STRING_GET_IMPL(*self);
    char* string              = __STRING_GET_STRING(impl);

    impl->size--;

    string[impl->size] = '\0';
    *self              = string;
}

bool StringAppend(String* self, const char* other) {
    assert(self && *self && other);

    struct __StringImpl* impl = __STRING_GET_IMPL(*self);

    size_t n = strlen(other);

    if (impl->size + n >= impl->capacity) {
        size_t new_capacity = impl->capacity;

        do {
            new_capacity *= 2;
        } while (new_capacity <= impl->size + n);

        if (!(impl = __StringRealloc(impl, new_capacity)))
            return false;
    }

    char* string = __STRING_GET_STRING(impl);

    strcpy(string + impl->size, other);
    impl->size += n;

    assert(string[impl->size] == '\0');
    *self = string;

    return true;
}

bool StringCopy(String self, String* dest) {
    assert(self);

    return (*dest = StringCreate(self, __STRING_GET_IMPL(self)->size));
}

void StringTrim(String* self, size_t n) {
    assert(self && *self);

    struct __StringImpl* impl = __STRING_GET_IMPL(*self);

    assert(n <= impl->size);

    char* string = __STRING_GET_STRING(impl);

    impl->size -= n;

    string[impl->size] = '\0';
    *self              = string;
}

bool StringShrink(String* self, size_t n) {
    assert(self && *self);

    struct __StringImpl* impl = __STRING_GET_IMPL(*self);

    assert(n <= impl->size);

    impl->size -= n;
    return StringReserve(self, impl->size);
}

bool StringContains(String self, const char* substring, size_t substring_size, bool case_sensitive) {
    assert(self && substring && substring_size);

    size_t i, match;

    if (case_sensitive) {
        for (i = 0; i < 1 + StringLength(self) - substring_size; i++)
            if ((match = strncmp(&self[i], substring, substring_size)) == 0)
                return true;
    } else {
        for (i = 0; i < 1 + StringLength(self) - substring_size; i++)
            if ((match = strncasecmp(&self[i], substring, substring_size)) == 0)
                return true;
    }

    return false;
}

size_t StringLength(String self) {
    assert(self);

    return __STRING_GET_IMPL(self)->size;
}

size_t StringCapacity(String self) {
    assert(self != NULL);

    return __STRING_GET_IMPL(self)->capacity;
}
