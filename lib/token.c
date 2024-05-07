#include "token.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "mcc.h"

static Result _TokenStringRealloc(TokenString* self) {
    uint32_t new_capacity = self->capacity == 0 ? 1 : self->capacity * 2;
    Token* new_tokens     = realloc(self->tokens, sizeof(Token) * new_capacity);

    if (new_tokens == NULL) {
        new_tokens = malloc(sizeof(Token) * new_capacity);

        if (new_tokens == NULL) {
            return OUT_OF_MEMORY;
        } else {
            if (self->tokens != NULL)
                memcpy(new_tokens, self->tokens, self->size);
            free(self->tokens);
        }
    }

    self->capacity = new_capacity;
    self->tokens   = new_tokens;
    return SUCCESS;
}

TokenString TokenStringCreate(void) {
    return (TokenString){
        .size     = 0,
        .capacity = 0,
        .tokens   = NULL,
    };
}

void TokenStringDestroy(TokenString* self) {
    for (uint32_t i = 0; i < self->size; i++) {
        free(self->tokens[i].data);
    }

    free(self->tokens);
}

Result TokenStringAppend(TokenString* self, Token token) {
    if (self->size == self->capacity) {
        Result result = _TokenStringRealloc(self);

        if (result != SUCCESS)
            return result;
    }

    assert(self->tokens != NULL);

    self->tokens[self->size] = token;
    self->size++;

    return SUCCESS;
}

Result TokenStringEmplace(TokenString* self, TokenType type, void* data) {
    if (self->size == self->capacity) {
        Result result = _TokenStringRealloc(self);

        if (result != SUCCESS)
            return result;
    }

    assert(self->tokens != NULL);

    self->tokens[self->size] = (Token){
        .type = type,
        .data = data,
    };
    self->size++;

    return SUCCESS;
}
