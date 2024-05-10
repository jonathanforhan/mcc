#pragma once

/// @file TokenVector.h
/// @brief TokenVector to make Vector more ergonomic

#include "Token.h"
#include "Vector.h"

typedef const Token* TokenVector;

void __TokenVectorDestructor(void* vp) {
    if (vp != NULL) {
        Token* token = vp;
        if (token->type & (TOKEN_IDENTIFIER_BIT | TOKEN_STRING_LITERAL_BIT))
            StringDestroy(token->data.str);
    }
}

/// @brief Create a new TokenTokenVector
/// @param reserve initial amount of elements to reserve
/// @return returns NULL on error, otherwise points to allocated block
static inline TokenVector TokenVectorCreate(size_t reserve) {
    return VectorCreate(reserve, sizeof(Token), __TokenVectorDestructor);
}

/// @brief Destroy TokenVector, dellocate memory reserved and calls destructor
/// @param self
static inline void TokenVectorDestroy(TokenVector self) {
    VectorDestroy(self);
}

/// @brief Reserve `size` elements, this function WILL allocate memory, unless size == capacity
/// @param self
/// @param size number of elements to allocate, CANNOT be less than size
/// @return returns false on error
static inline bool TokenVectorReserve(TokenVector* self, size_t size) {
    return VectorReserve((Vector*)self, size);
}

/// @brief Clear all elements in TokenVector
/// @param self
static inline void TokenVectorClear(TokenVector* self) {
    VectorClear((Vector*)self);
}

/// @brief Push an element via memcpy, this function MAY allocate memory
/// @param self
/// @param elem element to copy
/// @return returns false on error
static inline bool TokenVectorPush(TokenVector* self, Token* elem) {
    return VectorPush((Vector*)self, elem);
}

/// @brief Pop an element, calling it's destructor, this function does not allocate
/// @param self
/// @return returns NULL on error, otherwise points to allocated block
static inline void TokenVectorPop(TokenVector* self) {
    VectorPop((Vector*)self);
}

/// @brief Length of TokenVector
/// @param self
/// @return Length of TokenVector in terms of elements
static inline size_t TokenVectorLength(TokenVector self) {
    return VectorLength(self);
}

/// @brief Capacity of TokenVector
/// @param self
/// @return Capacity of TokenVector in terms of elements
static inline size_t TokenVectorCapacity(TokenVector self) {
    return VectorCapacity(self);
}

/// @brief Element size of vector
/// @return Element size of TokenVector
static inline size_t TokenVectorElementSize(void) {
    return sizeof(TokenVector);
}
