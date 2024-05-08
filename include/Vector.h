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

#include <stdint.h>

/// @brief Vector is an address to stored elements
typedef const void* Vector;

/// @brief Create a new Vector
/// @param reserve initial amount of bytes to reserve
/// @param elem_size size of element
/// @return returns NULL on error, otherwise points to allocated block
Vector VectorCreate(uint32_t reserve, uint32_t elem_size, void (*destructor)(void*));

/// @brief Destroy Vector, dellocate memory reserved and calls destructor
/// @param self
void VectorDestroy(Vector self);

/// @brief Reserve `size` bytes, this function WILL allocate memory, unless size == capacity
/// @param self
/// @param size number of elements to allocate, CANNOT be less than size
/// @return returns NULL on error, otherwise points to allocated block
Vector VectorReserve(Vector self, uint32_t size);

/// @brief Push an element via memcpy, this function MAY allocate memory
/// @param self
/// @param elem element to copy
/// @return returns NULL on error, otherwise points to allocated block
Vector VectorPush(Vector self, void* elem);

/// @brief Pop an element, calling it's destructor, this function does not allocate
/// @param self
/// @return returns NULL on error, otherwise points to allocated block
Vector VectorPop(Vector self);

/// @brief Length of Vector
/// @param self
/// @return Length of Vector in terms of elements
uint32_t VectorLength(Vector self);

/// @brief Capacity of Vector
/// @param self
/// @return Capacity of Vector in terms of elements
uint32_t VectorCapacity(Vector self);

/// @brief Element size of vector
/// @param self
/// @return Element size of Vector
uint32_t VectorElementSize(Vector self);
