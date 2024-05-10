#pragma once

/// @file Scan.h
/// @brief Scan the input files, generate tokens and AST

#include "TokenVector.h"

/// @brief Tokenize a C file
/// @param filepath path to file
/// @return returns NULL on error, otherwise a Vector of Tokens
TokenVector Tokenize(const char* filepath);
