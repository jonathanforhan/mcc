#pragma once

#include <stdint.h>
#include "mcc.h"
#include "token.h"

Result Tokenize(const char* filepath, TokenString* token_string);

Result GenerateAST(const TokenString* token_string);
