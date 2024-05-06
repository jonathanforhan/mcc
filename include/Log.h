#pragma once

#include <stdio.h>

#define MCC_LOG_ERROR(_MSG) fprintf(stderr, "MCC ERROR: " _MSG __LINE__ "\n")
#define MCC_LOG_WARNING(_MSG) fprintf(stderr, "MCC WARNING: " _MSG __LINE__ "\n")
#define MCC_LOG_INFO(_MSG) fprintf(stdout, "MCC INFO: " _MSG "\n")
