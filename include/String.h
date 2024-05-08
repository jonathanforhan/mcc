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

#include <stdint.h>

/// @brief String is simply an address to the stored chars
typedef const char* String;

/// @brief Create a new String
/// @param str initial string value, can be NULL
/// @param reserve initial amount of space to reserve NOT including NULL byte
/// @return returns NULL on error, otherwise points to allocated string
String StringCreate(const char* str, uint32_t reserve);

/// @brief Destroy String, deallocate memory reserved
/// @param self
void StringDestroy(String self);

/// @brief Reserve `size` bytes, this function WILL allocate memory, unless size == capacity
/// @param self
/// @param size number of bytes to allocate, equivalent to number of chars CAN be less than size
/// @return returns NULL on error, otherwise is a pointer to String
String StringReserve(String self, uint32_t size);

/// @brief Push a char to String, this function MAY allocate memory
/// @param self
/// @param c char to append
/// @return returns NULL on error, otherwise is a pointer to String
String StringPush(String self, char c);

/// @brief Pop a char to String, this function does not allocate memory
/// @param self
/// @return returns a pointer to String
String StringPop(String self);

/// @brief Append another String to String, this function MAY allocate memory
/// @param self
/// @param other String to append, this String is preserved as-is
/// @return returns NULL on error, otherwise is a pointer to String
String StringAppend(String self, const char* other);

/// @brief Copy String, this function WILL allocate memory
/// @param self
/// @return returns NULL on error, otherwise is a pointer to a new String
String StringCopy(String self);

/// @brief Trim String to specified size DOES NOT deallocate
/// @param self
/// @param n number of elements to trim
/// @return returns a pointer to String
String StringTrim(String self, uint32_t n);

/// @brief Strink String to scecified size DOES deallocate
/// @param self
/// @param n number of elements to shrink
/// @return returns a pointer to String
String StringShrink(String self, uint32_t n);

/// @brief Length of String
/// @param self
/// @return returns length of chars NOT including header or null byte
uint32_t StringLength(String self);

/// @brief Capacity of String
/// @param self
/// @return returns capacity allocated memory NOT including header
uint32_t StringCapacity(String self);
