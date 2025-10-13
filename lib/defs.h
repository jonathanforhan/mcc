/// @file lib/defs.h

#pragma once

#include <stddef.h>

struct mcc_string_view {
    char* data;
    size_t size;
};

struct mcc_wstring_view {
    wchar_t* data;
    size_t size;
};