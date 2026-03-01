/// @file lib/context.h
/// @brief MCC compiler context — owns all compiler-wide resources and lifetime-managed allocations.
///
/// The context is the root object of the MCC compiler. It must be created before any other
/// compiler object and destroyed last. All compiler objects that allocate memory (e.g. the
/// lexer's processed string literals) store their allocations here so that token data remains
/// valid for the lifetime of the context, independent of the lifetime of the object that
/// produced it.

#pragma once

/// @brief Opaque compiler context.
/// @note Create with mcc_context_create(), destroy with mcc_context_destroy().
struct mcc_context;

/// @brief Creates a new compiler context.
/// @return A pointer to the newly created context. Never returns NULL; exits on allocation failure.
struct mcc_context* mcc_context_create(void);

/// @brief Destroys a compiler context and frees all resources owned by it.
/// @param ctx The context to destroy. Must not be NULL.
/// @note All pointers into context-owned memory (e.g. string literal data) become invalid after this call.
void mcc_context_destroy(struct mcc_context* ctx);

/// @brief Transfers ownership of a heap-allocated string to the context.
/// @param ctx The context to store the string in. Must not be NULL.
/// @param str A heap-allocated, null-terminated string. Must not be NULL.
///            The context takes ownership and will free it on mcc_context_destroy().
void mcc_context_store_string(struct mcc_context* ctx, char* str);
