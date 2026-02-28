/// @file lib/defs.h

#pragma once

#include <stddef.h>

struct mcc_string_view {
    char* data;
    size_t size;
};

static inline struct mcc_string_view mcc_string_view_from_ptrs(char* base_ptr, const char* end_ptr) {
    return (struct mcc_string_view){.data = base_ptr, .size = (size_t)(end_ptr - base_ptr)};
}

struct mcc_wstring_view {
    wchar_t* data;
    size_t size;
};

static inline struct mcc_wstring_view mcc_wstring_view_from_ptrs(wchar_t* base_ptr, const wchar_t* end_ptr) {
    return (struct mcc_wstring_view){.data = base_ptr, .size = (size_t)(end_ptr - base_ptr)};
}
