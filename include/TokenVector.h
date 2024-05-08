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
inline TokenVector TokenVectorCreate(uint32_t reserve) {
    return VectorCreate(reserve, sizeof(Token), __TokenVectorDestructor);
}

/// @brief Destroy TokenVector, dellocate memory reserved and calls destructor
/// @param self
inline void TokenVectorDestroy(TokenVector self) {
    VectorDestroy(self);
}

/// @brief Reserve `size` elements, this function WILL allocate memory, unless size == capacity
/// @param self
/// @param size number of elements to allocate, CANNOT be less than size
/// @return returns NULL on error, otherwise points to allocated block
inline TokenVector TokenVectorReserve(TokenVector self, uint32_t size) {
    return VectorReserve(self, size);
}

/// @brief Push an element via memcpy, this function MAY allocate memory
/// @param self
/// @param elem element to copy
/// @return returns NULL on error, otherwise points to allocated block
inline TokenVector TokenVectorPush(TokenVector self, Token* elem) {
    return VectorPush(self, elem);
}

/// @brief Pop an element, calling it's destructor, this function does not allocate
/// @param self
/// @return returns NULL on error, otherwise points to allocated block
inline TokenVector TokenVectorPop(TokenVector self) {
    return VectorPop(self);
}

/// @brief Length of TokenVector
/// @param self
/// @return Length of TokenVector in terms of elements
inline uint32_t TokenVectorLength(TokenVector self) {
    return VectorLength(self);
}

/// @brief Capacity of TokenVector
/// @param self
/// @return Capacity of TokenVector in terms of elements
inline uint32_t TokenVectorCapacity(TokenVector self) {
    return VectorCapacity(self);
}

/// @brief Element size of vector
/// @return Element size of TokenVector
inline uint32_t TokenVectorElementSize(void) {
    return sizeof(TokenVector);
}
