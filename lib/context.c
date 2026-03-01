#include "context.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct string_storage {
    char** strings; // list of null-terimated strings
    size_t size;
    size_t used;
};

struct mcc_context {
    struct string_storage store; // owns all allocated string/wstring data
};

struct mcc_context* mcc_context_create(void) {
    struct mcc_context* ctx = malloc(sizeof(*ctx));
    if (!ctx) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    ctx->store.strings = malloc(sizeof(char*));
    if (!ctx->store.strings) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    ctx->store.size = 1;
    ctx->store.used = 0;

    return ctx;
}

void mcc_context_destroy(struct mcc_context* ctx) {
    assert(ctx);
    for (size_t i = 0; i < ctx->store.used; i++) {
        free(ctx->store.strings[i]);
    }
    free(ctx->store.strings);
    free(ctx);
}

void mcc_context_store_string(struct mcc_context* ctx, char* str) {
    assert(ctx && str);
    if (ctx->store.used == ctx->store.size) {
        char** new_store = malloc(sizeof(char*) * (ctx->store.size *= 2));
        if (!new_store) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        memcpy(new_store, ctx->store.strings, sizeof(char*) * ctx->store.used);
        free(ctx->store.strings);
        ctx->store.strings = new_store;
    }
    ctx->store.strings[ctx->store.used++] = str;
}
