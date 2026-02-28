/// @file lib/lexer.h

#pragma once

#include "defs.h"

enum mcc_keyword {
    MCC_KEYWORD_AUTO,      // "auto"
    MCC_KEYWORD_BREAK,     // "break"
    MCC_KEYWORD_CASE,      // "case"
    MCC_KEYWORD_CHAR,      // "char"
    MCC_KEYWORD_CONST,     // "const"
    MCC_KEYWORD_CONTINUE,  // "continue"
    MCC_KEYWORD_DEFAULT,   // "default"
    MCC_KEYWORD_DO,        // "do"
    MCC_KEYWORD_DOUBLE,    // "double"
    MCC_KEYWORD_ELSE,      // "else"
    MCC_KEYWORD_ENUM,      // "enum"
    MCC_KEYWORD_EXTERN,    // "extern"
    MCC_KEYWORD_FLOAT,     // "float"
    MCC_KEYWORD_FOR,       // "for"
    MCC_KEYWORD_GOTO,      // "goto"
    MCC_KEYWORD_IF,        // "if"
    MCC_KEYWORD_INLINE,    // "inline"
    MCC_KEYWORD_INT,       // "int"
    MCC_KEYWORD_LONG,      // "long"
    MCC_KEYWORD_REGISTER,  // "register"
    MCC_KEYWORD_RESTRICT,  // "restrict"
    MCC_KEYWORD_RETURN,    // "return"
    MCC_KEYWORD_SHORT,     // "short"
    MCC_KEYWORD_SIGNED,    // "signed"
    MCC_KEYWORD_SIZEOF,    // "sizeof"
    MCC_KEYWORD_STATIC,    // "static"
    MCC_KEYWORD_STRUCT,    // "struct"
    MCC_KEYWORD_SWITCH,    // "switch"
    MCC_KEYWORD_TYPEDEF,   // "typedef"
    MCC_KEYWORD_UNION,     // "union"
    MCC_KEYWORD_UNSIGNED,  // "unsigned"
    MCC_KEYWORD_VOID,      // "void"
    MCC_KEYWORD_VOLATILE,  // "volatile"
    MCC_KEYWORD_WHILE,     // "while"
    MCC_KEYWORD_BOOL,      // "_Bool"
    MCC_KEYWORD_COMPLEX,   // "_Complex"
    MCC_KEYWORD_IMAGINARY, // "_Imaginary"
    MCC_KEYWORD_NOT_FOUND = -1,
};

enum mcc_constant_type {
    MCC_CONSTANT_TYPE_ENUM,
    MCC_CONSTANT_TYPE_CHAR,
    MCC_CONSTANT_TYPE_SIGNED_CHAR,
    MCC_CONSTANT_TYPE_UNSIGNED_CHAR,
    MCC_CONSTANT_TYPE_WIDE_CHAR,
    MCC_CONSTANT_TYPE_INT,
    MCC_CONSTANT_TYPE_LONG_INT,
    MCC_CONSTANT_TYPE_LONG_LONG_INT,
    MCC_CONSTANT_TYPE_UNSIGNED_INT,
    MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT,
    MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT,
    MCC_CONSTANT_TYPE_FLOAT,
    MCC_CONSTANT_TYPE_DOUBLE,
    MCC_CONSTANT_TYPE_LONG_DOUBLE,
    MCC_CONSTANT_TYPE_INVALID  = -1,
    MCC_CONSTANT_TYPE_OVERFLOW = -2,
};

union mcc_constant_value {
    char c;
    signed char sc;
    unsigned char uc;
    wchar_t wc;
    int i;
    long int l;
    long long int ll;
    unsigned int u;
    unsigned long int ul;
    unsigned long long int ull;
    float f;
    double d;
    long double ld;
};

struct mcc_constant {
    enum mcc_constant_type type;
    union mcc_constant_value value;
};

enum mcc_string_literal_type {
    MCC_STRING_LITERAL_TYPE_STRING,
    MCC_STRING_LITERAL_TYPE_WIDE_STRING,
};

union mcc_string_literal_value {
    struct mcc_string_view string;
    struct mcc_wstring_view wstring;
};

struct mcc_string_literal {
    enum mcc_string_literal_type type;
    union mcc_string_literal_value value;
};

enum mcc_punctuator {
    MCC_PUNCTUATOR_LEFT_BRACKET,               // [
    MCC_PUNCTUATOR_RIGHT_BRACKET,              // ]
    MCC_PUNCTUATOR_LEFT_PARENTHESIS,           // (
    MCC_PUNCTUATOR_RIGHT_PARENTHESIS,          // )
    MCC_PUNCTUATOR_LEFT_BRACE,                 // {
    MCC_PUNCTUATOR_RIGHT_BRACE,                // }
    MCC_PUNCTUATOR_DOT,                        // .
    MCC_PUNCTUATOR_ARROW,                      // ->
    MCC_PUNCTUATOR_PLUS_PLUS,                  // ++
    MCC_PUNCTUATOR_MINUS_MINUS,                // --
    MCC_PUNCTUATOR_AMPERSAND,                  // &
    MCC_PUNCTUATOR_ASTERISK,                   // *
    MCC_PUNCTUATOR_PLUS,                       // +
    MCC_PUNCTUATOR_MINUS,                      // -
    MCC_PUNCTUATOR_TILDE,                      // ~
    MCC_PUNCTUATOR_BANG,                       // !
    MCC_PUNCTUATOR_SLASH,                      // /
    MCC_PUNCTUATOR_PERCENT,                    // %
    MCC_PUNCTUATOR_DOUBLE_LEFT_CHEVRON,        // <<
    MCC_PUNCTUATOR_DOUBLE_RIGHT_CHEVRON,       // >>
    MCC_PUNCTUATOR_LEFT_CHEVRON,               // <
    MCC_PUNCTUATOR_RIGHT_CHEVRON,              // >
    MCC_PUNCTUATOR_LEFT_CHEVRON_EQUAL,         // <=
    MCC_PUNCTUATOR_RIGHT_CHEVRON_EQUAL,        // >=
    MCC_PUNCTUATOR_EQUAL_EQUAL,                // ==
    MCC_PUNCTUATOR_BANG_EQUAL,                 // !=
    MCC_PUNCTUATOR_CARET,                      // ^
    MCC_PUNCTUATOR_PIPE,                       // |
    MCC_PUNCTUATOR_AMPERSAND_AMPERSAND,        // &&
    MCC_PUNCTUATOR_PIPE_PIPE,                  // ||
    MCC_PUNCTUATOR_QUESTION_MARK,              // ?
    MCC_PUNCTUATOR_COLON,                      // :
    MCC_PUNCTUATOR_SEMICOLON,                  // ;
    MCC_PUNCTUATOR_ELLIPSIS,                   // ...
    MCC_PUNCTUATOR_EQUAL,                      // =
    MCC_PUNCTUATOR_ASTERISK_EQUAL,             // *=
    MCC_PUNCTUATOR_SLASH_EQUAL,                // /=
    MCC_PUNCTUATOR_PERCENT_EQUAL,              // %=
    MCC_PUNCTUATOR_PLUS_EQUAL,                 // +=
    MCC_PUNCTUATOR_MINUS_EQUAL,                // -=
    MCC_PUNCTUATOR_DOUBLE_LEFT_CHEVRON_EQUAL,  // <<=
    MCC_PUNCTUATOR_DOUBLE_RIGHT_CHEVRON_EQUAL, // >>=
    MCC_PUNCTUATOR_AMPERSAND_EQUAL,            // &=
    MCC_PUNCTUATOR_CARET_EQUAL,                // ^=
    MCC_PUNCTUATOR_PIPE_EQUAL,                 // |=
    MCC_PUNCTUATOR_COMMA,                      // ,
    MCC_PUNCTUATOR_HASH,                       // #
    MCC_PUNCTUATOR_HASH_HASH,                  // ##
};

enum mcc_token_type {
    MCC_TOKEN_TYPE_EOF,
    MCC_TOKEN_TYPE_KEYWORD,
    MCC_TOKEN_TYPE_IDENTIFIER,
    MCC_TOKEN_TYPE_CONSTANT,
    MCC_TOKEN_TYPE_STRING_LITERAL,
    MCC_TOKEN_TYPE_PUNCTUATOR,
    MCC_TOKEN_TYPE_INVALID = -1,
};

union mcc_token_value {
    enum mcc_keyword keyword;
    struct mcc_string_view identifier;
    struct mcc_constant constant;
    struct mcc_string_literal string_literal;
    enum mcc_punctuator punctuator;
    const char* error_message;
};

struct mcc_token {
    enum mcc_token_type type;
    union mcc_token_value value;
    struct mcc_string_view lexeme;
    size_t line;
    size_t column;
};

struct mcc_lexer {
    char* source;
    char* current;
    size_t line;
    size_t column;
};

/// @brief Initializes a lexer with the given source text and its length.
/// @param lexer Pointer to the lexer structure to initialize.
/// @param source Pointer to the source text to be lexed.
/// @param length Length of the source text in bytes (excluding NULL terminator).
void mcc_lexer_create(struct mcc_lexer* lexer, const char* source, size_t length);

/// @brief Destroys a lexer object and releases any resources associated with it.
/// @param lexer Pointer to the lexer object to be destroyed.
void mcc_lexer_destroy(struct mcc_lexer* lexer);

/// @brief Retrieves the next token from the lexer.
/// @param lexer Pointer to the lexer from which to retrieve the next token.
/// @return The next token from the lexer. If the end of the input is reached, MCC_TOKEN_TYPE_EOF is returned.
struct mcc_token mcc_lexer_next_token(struct mcc_lexer* lexer);
