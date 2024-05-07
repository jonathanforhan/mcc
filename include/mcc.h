#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Result {
    SUCCESS       = 0,
    IO_FAILURE    = 1,
    OUT_OF_MEMORY = 2,
    FAILURE       = 0xffff,
} Result;

void MCC_Initialize(void);

#ifdef __cplusplus
}
#endif
