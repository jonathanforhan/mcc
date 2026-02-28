/// @file lib/defs.h
/// @brief Common type definitions for MCC compiler.

#pragma once

#include <stddef.h>

/// @brief A non-owning view into a string.
/// @note The data is not guarenteed to be null-terminated.
struct mcc_string_view {
    char* data;  ///< Pointer to the first character.
    size_t size; ///< Number of characters in the view.
};

/// @brief Constructs a string view from a base and end pointer.
/// @param begin Pointer to the first character.
/// @param end  Pointer one past the last character.
/// @return A string view spanning [base_ptr, end_ptr).
static inline struct mcc_string_view mcc_string_view_from_ptrs(char* begin, const char* end) {
    return (struct mcc_string_view){
        .data = begin,
        .size = (size_t)(end - begin),
    };
}

/// @brief A non-owning view into a wide string.
/// @note The data is not guarenteed to be null-terminated.
struct mcc_wstring_view {
    wchar_t* data; ///< Pointer to the first character.
    size_t size;   ///< Number of characters in the view.
};

/// @brief Constructs a wide string view from a base and end pointer.
/// @param begin Pointer to the first character.
/// @param end  Pointer one past the last character.
/// @return A wide string view spanning [base_ptr, end_ptr).
static inline struct mcc_wstring_view mcc_wstring_view_from_ptrs(wchar_t* begin, const wchar_t* end) {
    return (struct mcc_wstring_view){
        .data = begin,
        .size = (size_t)(end - begin),
    };
}
