#pragma once

/// @file Scan.h
/// @brief Scan the input files, generate tokens and AST

#include "TokenVector.h"

/// @brief Tokenize a C file
/// @param filepath path to file
/// @return returns false on error, true on success
bool Tokenize(const char* filepath, TokenVector* token_vector);