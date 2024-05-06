#pragma once

typedef enum MCC_Token {
    // SYMBOLS
    MCC_TOKEN_UNDEFINED         = 0x00, // unknown
    MCC_TOKEN_TILDE             = 0x01, // ~
    MCC_TOKEN_TICK              = 0x02, // `
    MCC_TOKEN_BANG              = 0x03, // !
    MCC_TOKEN_AT                = 0x04, // @
    MCC_TOKEN_HASH              = 0x05, // #
    MCC_TOKEN_DOLLAR            = 0x06, // $
    MCC_TOKEN_PERCENT           = 0x07, // %
    MCC_TOKEN_CARET             = 0x08, // ^
    MCC_TOKEN_AMPERSAND         = 0x09, // &
    MCC_TOKEN_STAR              = 0x0a, // *
    MCC_TOKEN_PARENTHESIS_LEFT  = 0x0b, // (
    MCC_TOKEN_PARENTHESIS_RIGHT = 0x0c, // )
    MCC_TOKEN_MINUS             = 0x0d, // -
    MCC_TOKEN_PLUS              = 0x0e, // +
    MCC_TOKEN_EQUAL             = 0x0f, // =
    MCC_TOKEN_BRACE_LEFT        = 0x10, // {
    MCC_TOKEN_BRACE_RIGHT       = 0x11, // }
    MCC_TOKEN_BRACKET_LEFT      = 0x12, // [
    MCC_TOKEN_BRACKET_RIGHT     = 0x13, // ]
    MCC_TOKEN_PIPE              = 0x14, // |
    MCC_TOKEN_BACKSLASH         = 0x15, // '\'
    MCC_TOKEN_COLON             = 0x16, // :
    MCC_TOKEN_SEMICOLON         = 0x17, // ;
    MCC_TOKEN_DOUBLE_QUOTE      = 0x18, // "
    MCC_TOKEN_SINGLE_QUOTE      = 0x19, // '
    MCC_TOKEN_CHEVRON_LEFT      = 0x1a, // <
    MCC_TOKEN_CHEVRON_RIGHT     = 0x1b, // >
    MCC_TOKEN_COMMA             = 0x1c, // ,
    MCC_TOKEN_DOT               = 0x1d, // .
    MCC_TOKEN_QUESTION_MARK     = 0x1e, // ?
    MCC_TOKEN_FORWARD_SLASH     = 0x1f, // /

    // LITERALS
    MCC_TOKEN_NUMERIC_LITERAL   = 0x20, // 42
    MCC_TOKEN_CHARACTER_LITERAL = 0x21, // "Hello World\n"
    MCC_TOKEN_STRING_LITERAL    = 0x22, // 'x'

    // IDENTIFIER
    MCC_TOKEN_IDENTIFIER = 0x23,

    // KEYWORDS
    MCC_TOKEN_KEYWORD_BIT = 0x4000,
    // C90
    MCC_TOKEN_AUTO     = 0x4001,
    MCC_TOKEN_BREAK    = 0x4002,
    MCC_TOKEN_CASE     = 0x4003,
    MCC_TOKEN_CHAR     = 0x4004,
    MCC_TOKEN_CONST    = 0x4005,
    MCC_TOKEN_CONTINUE = 0x4006,
    MCC_TOKEN_DEFAULT  = 0x4007,
    MCC_TOKEN_DO       = 0x4008,
    MCC_TOKEN_DOUBLE   = 0x4009,
    MCC_TOKEN_ELSE     = 0x400a,
    MCC_TOKEN_ENUM     = 0x400b,
    MCC_TOKEN_EXTERN   = 0x400c,
    MCC_TOKEN_FLOAT    = 0x400d,
    MCC_TOKEN_FOR      = 0x400e,
    MCC_TOKEN_GOTO     = 0x400f,
    MCC_TOKEN_IF       = 0x4010,
    MCC_TOKEN_INT      = 0x4011,
    MCC_TOKEN_LONG     = 0x4012,
    MCC_TOKEN_REGISTER = 0x4013,
    MCC_TOKEN_RETURN   = 0x4014,
    MCC_TOKEN_SHORT    = 0x4015,
    MCC_TOKEN_SIGNED   = 0x4016,
    MCC_TOKEN_SIZEOF   = 0x4017,
    MCC_TOKEN_STATIC   = 0x4018,
    MCC_TOKEN_STRUCT   = 0x4019,
    MCC_TOKEN_SWITCH   = 0x401a,
    MCC_TOKEN_TYPEDEF  = 0x401b,
    MCC_TOKEN_UNION    = 0x401c,
    MCC_TOKEN_UNSIGNED = 0x401c,
    MCC_TOKEN_VOID     = 0x401d,
    MCC_TOKEN_VOLATILE = 0x401e,
    MCC_TOKEN_WHILE    = 0x401f,
    // C99
    MCC_TOKEN__Bool      = 0x4020,
    MCC_TOKEN__Complex   = 0x4021,
    MCC_TOKEN__Imaginary = 0x4022,
    MCC_TOKEN_INLINE     = 0x4023,
    MCC_TOKEN_RESTRICT   = 0x4024,
} MCC_Token;
