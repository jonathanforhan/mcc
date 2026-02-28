/// @file lib/private/utils.h

#pragma once

#include <ctype.h>
#include <stddef.h>

/// @brief Calculates the number of elements in a static array.
#define ARRAY_SIZE(_Array) (sizeof(_Array) / sizeof(*_Array))

/// @brief Checks if a character is an octal digit ('0' to '7').
/// @param c The character to check, represented as an integer.
/// @return Nonzero if the character is an octal digit; otherwise, zero.
static inline int isodigit(int c) {
    return (c >= '0' && c <= '7');
}

/// @brief Checks if a character is a valid starting character for an identifier.
/// @param c The character to check, represented as an integer.
/// @return Nonzero if the character is an underscore or an English letter (uppercase or lowercase); zero otherwise.
static inline int isident_start(int c) {
    return (isalpha(c) || c == '_');
}

/// @brief Checks if a character is a valid C identifier character (letter, digit, or underscore).
/// @param c The character to check, represented as an integer.
/// @return Nonzero if the character is a letter, digit, or underscore; zero otherwise.
static inline int isident(int c) {
    return (isalnum(c) || c == '_');
}

/// @brief Reads the contents of a file into a dynamically allocated buffer.
/// @param path The path to the file to be read.
/// @param bytes_read Pointer to a variable where the number of bytes read will be stored.
/// @return A pointer to a buffer containing the file's contents, or NULL if the file could not be read. The caller is
/// responsible for freeing the buffer.
char* read_file(const char* path, size_t* bytes_read);
