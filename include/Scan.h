#pragma once

/// @file Scan.h
/// @brief Scan the input files, generate tokens and AST

#include "TokenVector.h"

/// @brief Tokenize a C file
/// @param filepath path to file
/// @return returns false on error, true on success
bool Tokenize(const char* filepath, TokenVector* token_vector);

ssize_t ParseConstantSuffix(String str, Token* token);

ssize_t ParseConstantSuffixFP(String str, Token* token);

ssize_t ParseConstantOct(String str, Token* token);

ssize_t ParseConstantDec(String str, Token* token);

ssize_t ParseConstantDecFP(String str, Token* token);

ssize_t ParseConstantHex(String str, Token* token);

ssize_t ParseConstantHexFP(String str, Token* token);

ssize_t ParseConstant(String str, Token* token);

ssize_t ParseChar(String str, Token* token);

ssize_t ParseLongChar(String str, Token* token);

ssize_t ParseStr(void);

ssize_t ParseLongStr(void);

ssize_t ParseKeywordOrIdentifier(void);

ssize_t ParsePunctuator(void);
