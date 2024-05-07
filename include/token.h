#pragma once

#include <stdint.h>
#include "mcc.h"

#define TOKEN_KEYWORD_BIT 0x0400
#define TOKEN_IDENTIFIER_BIT 0x0800
#define TOKEN_CONSTANT_BIT 0x1000
#define TOKEN_STRING_LITERAL_BIT 0x2000
#define TOKEN_PUNCTUATOR_BIT 0x4000
#define TOKEN_MALFORMED_BIT 0x8000

typedef enum TokenType {
    TOKEN_UNKNOWN = 0,

    //=== KEYWORDS ===//

    TOKEN_AUTO       = TOKEN_KEYWORD_BIT | 0x01,
    TOKEN_BREAK      = TOKEN_KEYWORD_BIT | 0x02,
    TOKEN_CASE       = TOKEN_KEYWORD_BIT | 0x03,
    TOKEN_CHAR       = TOKEN_KEYWORD_BIT | 0x04,
    TOKEN_CONST      = TOKEN_KEYWORD_BIT | 0x05,
    TOKEN_CONTINUE   = TOKEN_KEYWORD_BIT | 0x06,
    TOKEN_DEFAULT    = TOKEN_KEYWORD_BIT | 0x07,
    TOKEN_DO         = TOKEN_KEYWORD_BIT | 0x08,
    TOKEN_DOUBLE     = TOKEN_KEYWORD_BIT | 0x09,
    TOKEN_ELSE       = TOKEN_KEYWORD_BIT | 0x0a,
    TOKEN_ENUM       = TOKEN_KEYWORD_BIT | 0x0b,
    TOKEN_EXTERN     = TOKEN_KEYWORD_BIT | 0x0c,
    TOKEN_FLOAT      = TOKEN_KEYWORD_BIT | 0x0d,
    TOKEN_FOR        = TOKEN_KEYWORD_BIT | 0x0e,
    TOKEN_GOTO       = TOKEN_KEYWORD_BIT | 0x0f,
    TOKEN_IF         = TOKEN_KEYWORD_BIT | 0x10,
    TOKEN_INT        = TOKEN_KEYWORD_BIT | 0x11,
    TOKEN_LONG       = TOKEN_KEYWORD_BIT | 0x12,
    TOKEN_REGISTER   = TOKEN_KEYWORD_BIT | 0x13,
    TOKEN_RETURN     = TOKEN_KEYWORD_BIT | 0x14,
    TOKEN_SHORT      = TOKEN_KEYWORD_BIT | 0x15,
    TOKEN_SIGNED     = TOKEN_KEYWORD_BIT | 0x16,
    TOKEN_SIZEOF     = TOKEN_KEYWORD_BIT | 0x17,
    TOKEN_STATIC     = TOKEN_KEYWORD_BIT | 0x18,
    TOKEN_STRUCT     = TOKEN_KEYWORD_BIT | 0x19,
    TOKEN_SWITCH     = TOKEN_KEYWORD_BIT | 0x1a,
    TOKEN_TYPEDEF    = TOKEN_KEYWORD_BIT | 0x1b,
    TOKEN_UNION      = TOKEN_KEYWORD_BIT | 0x1c,
    TOKEN_UNSIGNED   = TOKEN_KEYWORD_BIT | 0x1d,
    TOKEN_VOID       = TOKEN_KEYWORD_BIT | 0x1e,
    TOKEN_VOLATILE   = TOKEN_KEYWORD_BIT | 0x1f,
    TOKEN_WHILE      = TOKEN_KEYWORD_BIT | 0x20,
    TOKEN__Bool      = TOKEN_KEYWORD_BIT | 0x21,
    TOKEN__Complex   = TOKEN_KEYWORD_BIT | 0x22,
    TOKEN__Imaginary = TOKEN_KEYWORD_BIT | 0x23,
    TOKEN_INLINE     = TOKEN_KEYWORD_BIT | 0x24,
    TOKEN_RESTRICT   = TOKEN_KEYWORD_BIT | 0x25,

    //=== INDENTIFIERS ===//

    TOKEN_INDENTIFIER = 0x0801,

    //=== CONSTANTS ===//

    TOKEN_INTEGER_CONSTANT        = TOKEN_CONSTANT_BIT | 0x01,
    TOKEN_FLOATING_CONSTANT       = TOKEN_CONSTANT_BIT | 0x02,
    TOKEN_ENUMERATION_CONSTANT    = TOKEN_CONSTANT_BIT | 0x03,
    TOKEN_CHARACTER_CONSTANT      = TOKEN_CONSTANT_BIT | 0x04,
    TOKEN_WIDE_CHARACTER_CONSTANT = TOKEN_CONSTANT_BIT | 0x05,

    //=== STRING-LITERALS ===//

    TOKEN_STRING_LITERAL      = TOKEN_STRING_LITERAL_BIT | 0x01,
    TOKEN_WIDE_STRING_LITERAL = TOKEN_STRING_LITERAL_BIT | 0x02,

    //=== PUNCTUATORS ===//

    TOKEN_BRACKET_LEFT               = TOKEN_PUNCTUATOR_BIT | 0x01, // [
    TOKEN_BRACKET_RIGHT              = TOKEN_PUNCTUATOR_BIT | 0x02, // ]
    TOKEN_BRACE_LEFT                 = TOKEN_PUNCTUATOR_BIT | 0x03, // {
    TOKEN_BRACE_RIGHT                = TOKEN_PUNCTUATOR_BIT | 0x04, // }
    TOKEN_PARENTHESIS_LEFT           = TOKEN_PUNCTUATOR_BIT | 0x05, // (
    TOKEN_PARENTHESIS_RIGHT          = TOKEN_PUNCTUATOR_BIT | 0x06, // )
    TOKEN_DOT                        = TOKEN_PUNCTUATOR_BIT | 0x07, // .
    TOKEN_ARROW                      = TOKEN_PUNCTUATOR_BIT | 0x08, // ->
    TOKEN_DOUBLE_PLUS                = TOKEN_PUNCTUATOR_BIT | 0x09, // ++
    TOKEN_DOUBLE_MINUS               = TOKEN_PUNCTUATOR_BIT | 0x0a, // --
    TOKEN_AMPERSAND                  = TOKEN_PUNCTUATOR_BIT | 0x0b, // &
    TOKEN_STAR                       = TOKEN_PUNCTUATOR_BIT | 0x0c, // *
    TOKEN_PLUS                       = TOKEN_PUNCTUATOR_BIT | 0x0d, // +
    TOKEN_MINUS                      = TOKEN_PUNCTUATOR_BIT | 0x0e, // -
    TOKEN_TILDE                      = TOKEN_PUNCTUATOR_BIT | 0x0f, // ~
    TOKEN_BANG                       = TOKEN_PUNCTUATOR_BIT | 0x10, // !
    TOKEN_SLASH                      = TOKEN_PUNCTUATOR_BIT | 0x11, // /
    TOKEN_PERCENT                    = TOKEN_PUNCTUATOR_BIT | 0x12, // %
    TOKEN_DOUBLE_CHEVRON_LEFT        = TOKEN_PUNCTUATOR_BIT | 0x13, // <<
    TOKEN_DOUBLE_CHEVRON_RIGHT       = TOKEN_PUNCTUATOR_BIT | 0x14, // >>
    TOKEN_CHEVRON_LEFT               = TOKEN_PUNCTUATOR_BIT | 0x15, // <
    TOKEN_CHEVRON_RIGHT              = TOKEN_PUNCTUATOR_BIT | 0x16, // >
    TOKEN_CHEVRON_LEFT_EQUAL         = TOKEN_PUNCTUATOR_BIT | 0x17, // <=
    TOKEN_CHEVRON_RIGHT_EQUAL        = TOKEN_PUNCTUATOR_BIT | 0x18, // >=
    TOKEN_EQUAL_EQUAL                = TOKEN_PUNCTUATOR_BIT | 0x19, // ==
    TOKEN_BANG_EQUAL                 = TOKEN_PUNCTUATOR_BIT | 0x1a, // !=
    TOKEN_CARET                      = TOKEN_PUNCTUATOR_BIT | 0x1b, // ^
    TOKEN_PIPE                       = TOKEN_PUNCTUATOR_BIT | 0x1c, // |
    TOKEN_DOUBLE_AMPERSAND           = TOKEN_PUNCTUATOR_BIT | 0x1d, // &&
    TOKEN_DOUBLE_PIPE                = TOKEN_PUNCTUATOR_BIT | 0x1e, // ||
    TOKEN_QUESTION_MARK              = TOKEN_PUNCTUATOR_BIT | 0x1f, // ?
    TOKEN_COLON                      = TOKEN_PUNCTUATOR_BIT | 0x20, // :
    TOKEN_SEMICOLON                  = TOKEN_PUNCTUATOR_BIT | 0x21, // ;
    TOKEN_TRIPLE_DOT                 = TOKEN_PUNCTUATOR_BIT | 0x22, // ...
    TOKEN_EQUAL                      = TOKEN_PUNCTUATOR_BIT | 0x23, // =
    TOKEN_STAR_EQUAL                 = TOKEN_PUNCTUATOR_BIT | 0x24, // *=
    TOKEN_SLASH_EQUAL                = TOKEN_PUNCTUATOR_BIT | 0x25, // /=
    TOKEN_PERCENT_EQUAL              = TOKEN_PUNCTUATOR_BIT | 0x26, // %=
    TOKEN_PLUS_EQUAL                 = TOKEN_PUNCTUATOR_BIT | 0x27, // +=
    TOKEN_MINUS_EQUAL                = TOKEN_PUNCTUATOR_BIT | 0x28, // -=
    TOKEN_DOUBLE_CHEVRON_LEFT_EQUAL  = TOKEN_PUNCTUATOR_BIT | 0x29, // <<=
    TOKEN_DOUBLE_CHEVRON_RIGHT_EQUAL = TOKEN_PUNCTUATOR_BIT | 0x2a, // >>=
    TOKEN_AMPERSAND_EQUAL            = TOKEN_PUNCTUATOR_BIT | 0x2b, // &=
    TOKEN_CARET_EQUAL                = TOKEN_PUNCTUATOR_BIT | 0x2c, // ^=
    TOKEN_PIPE_EQUAL                 = TOKEN_PUNCTUATOR_BIT | 0x2d, // |=
    TOKEN_COMMA                      = TOKEN_PUNCTUATOR_BIT | 0x2e, // ,
    TOKEN_HASH                       = TOKEN_PUNCTUATOR_BIT | 0x2f, // #
    TOKEN_DOUBLE_HASH                = TOKEN_PUNCTUATOR_BIT | 0x30, // ##
    TOKEN_ALT_BRACKET_LEFT           = TOKEN_PUNCTUATOR_BIT | 0x31, // <:
    TOKEN_ALT_BRACKET_RIGHT          = TOKEN_PUNCTUATOR_BIT | 0x32, // :>
    TOKEN_ALT_BRACE_LEFT             = TOKEN_PUNCTUATOR_BIT | 0x33, // <%
    TOKEN_ALT_BRACE_RIGHT            = TOKEN_PUNCTUATOR_BIT | 0x34, // %>
    TOKEN_ALT_HASH                   = TOKEN_PUNCTUATOR_BIT | 0x35, // %:
    TOKEN_ALT_DOUBLE_HASH            = TOKEN_PUNCTUATOR_BIT | 0x36, // %:%:
    TOKEN_SINGLE_LINE_COMMENT        = TOKEN_PUNCTUATOR_BIT | 0x37, // //
    TOKEN_MULTI_LINE_COMMENT_BEGIN   = TOKEN_PUNCTUATOR_BIT | 0x38, // /*
    TOKEN_MULTI_LINE_COMMENT_END     = TOKEN_PUNCTUATOR_BIT | 0x39, // */

    //=== MALFORMED TOKENS ===//

    TOKEN_MALFORMED_KEYWORD        = TOKEN_MALFORMED_BIT | TOKEN_KEYWORD_BIT,
    TOKEN_MALFORMED_IDENTIFIER     = TOKEN_MALFORMED_BIT | TOKEN_IDENTIFIER_BIT,
    TOKEN_MALFORMED_CONSTANT       = TOKEN_MALFORMED_BIT | TOKEN_CONSTANT_BIT,
    TOKEN_MALFORMED_STRING_LITERAL = TOKEN_MALFORMED_BIT | TOKEN_STRING_LITERAL_BIT,
    TOKEN_MALFORMED_PUNCTUATOR     = TOKEN_MALFORMED_BIT | TOKEN_PUNCTUATOR_BIT,
} TokenType;

typedef struct Token {
    TokenType type;

    // keyword          Token.data : NULL
    // identifier       Token.data : String*
    // constant         Token.data : uint64_t*
    // string literal   Token.data : String*
    // punctuator       Token.data : NULL
    void* data;
} Token;

typedef struct TokenString {
    uint32_t size;
    uint32_t capacity;
    Token* tokens;
} TokenString;

TokenString TokenStringCreate(void);

void TokenStringDestroy(TokenString* self);

Result TokenStringAppend(TokenString* self, Token token);

Result TokenStringEmplace(TokenString* self, TokenType type, void* data);
