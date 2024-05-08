#pragma once

#include <stdio.h>

#define LOG_ERROR(_FMT, ...) fprintf(stderr, "MCC Error: " _FMT "\n", __VA_ARGS__)
#define LOG_WARNING(_FMT, ...) fprintf(stderr, "MCC Warning: " _FMT "\n", __VA_ARGS__)
#define LOG_INFO(_FMT, ...) fprintf(stderr, "MCC Info: " _FMT "\n", __VA_ARGS__)
