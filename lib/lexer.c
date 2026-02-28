#include "lexer.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "./private/utils.h"
#include "defs.h"

#ifdef _MSC_VER
#define strcasecmp  _stricmp
#define strncasecmp _strnicmp
#endif

struct table_entry {
    const char* key;
    int value;
};

static int table_lookup(const struct table_entry* table, const char* str, size_t len) {
    for (; table->key != NULL; table++) {
        if ((strncmp(table->key, str, len) == 0) && (strlen(table->key) == len)) {
            break;
        }
    }
    return table->value;
}

static int table_caseless_lookup(const struct table_entry* table, const char* str, size_t len) {
    for (; table->key != NULL; table++) {
        if ((strncasecmp(table->key, str, len) == 0) && (strlen(table->key) == len)) {
            break;
        }
    }
    return table->value;
}

static const struct table_entry keyword_table[] = {
    {"auto",       MCC_KEYWORD_AUTO     },
    {"break",      MCC_KEYWORD_BREAK    },
    {"case",       MCC_KEYWORD_CASE     },
    {"char",       MCC_KEYWORD_CHAR     },
    {"const",      MCC_KEYWORD_CONST    },
    {"continue",   MCC_KEYWORD_CONTINUE },
    {"default",    MCC_KEYWORD_DEFAULT  },
    {"do",         MCC_KEYWORD_DO       },
    {"double",     MCC_KEYWORD_DOUBLE   },
    {"else",       MCC_KEYWORD_ELSE     },
    {"enum",       MCC_KEYWORD_ENUM     },
    {"extern",     MCC_KEYWORD_EXTERN   },
    {"float",      MCC_KEYWORD_FLOAT    },
    {"for",        MCC_KEYWORD_FOR      },
    {"goto",       MCC_KEYWORD_GOTO     },
    {"if",         MCC_KEYWORD_IF       },
    {"inline",     MCC_KEYWORD_INLINE   },
    {"int",        MCC_KEYWORD_INT      },
    {"long",       MCC_KEYWORD_LONG     },
    {"register",   MCC_KEYWORD_REGISTER },
    {"restrict",   MCC_KEYWORD_RESTRICT },
    {"return",     MCC_KEYWORD_RETURN   },
    {"short",      MCC_KEYWORD_SHORT    },
    {"signed",     MCC_KEYWORD_SIGNED   },
    {"sizeof",     MCC_KEYWORD_SIZEOF   },
    {"static",     MCC_KEYWORD_STATIC   },
    {"struct",     MCC_KEYWORD_STRUCT   },
    {"switch",     MCC_KEYWORD_SWITCH   },
    {"typedef",    MCC_KEYWORD_TYPEDEF  },
    {"union",      MCC_KEYWORD_UNION    },
    {"unsigned",   MCC_KEYWORD_UNSIGNED },
    {"void",       MCC_KEYWORD_VOID     },
    {"volatile",   MCC_KEYWORD_VOLATILE },
    {"while",      MCC_KEYWORD_WHILE    },
    {"_Bool",      MCC_KEYWORD_BOOL     },
    {"_Complex",   MCC_KEYWORD_COMPLEX  },
    {"_Imaginary", MCC_KEYWORD_IMAGINARY},
    {NULL,         MCC_KEYWORD_NOT_FOUND},
};

static const struct table_entry integer_suffix_table[] = {
    {"U",   MCC_CONSTANT_TYPE_UNSIGNED_INT          },
    {"L",   MCC_CONSTANT_TYPE_LONG_INT              },
    {"UL",  MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT     },
    {"LU",  MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT     },
    {"LL",  MCC_CONSTANT_TYPE_LONG_LONG_INT         },
    {"ULL", MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT},
    {"LLU", MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT},
    {NULL,  MCC_CONSTANT_TYPE_INVALID               },
};

static const struct table_entry float_suffix_table[] = {
    {"F",  MCC_CONSTANT_TYPE_FLOAT      },
    {"L",  MCC_CONSTANT_TYPE_LONG_DOUBLE},
    {NULL, MCC_CONSTANT_TYPE_INVALID    },
};

static enum mcc_keyword keyword_lookup(const char* str, size_t len) {
    return table_lookup(keyword_table, str, len);
}

static enum mcc_constant_type integer_suffix_lookup(const char* str, size_t len) {
    return table_caseless_lookup(integer_suffix_table, str, len);
}

static enum mcc_constant_type float_suffix_lookup(const char* str, size_t len) {
    return table_caseless_lookup(float_suffix_table, str, len);
}

static const char escape_sequences[256] = {
    ['\''] = '\'',
    ['\"'] = '\"',
    ['\?'] = '\?',
    ['\\'] = '\\',
    ['a']  = '\a',
    ['b']  = '\b',
    ['f']  = '\f',
    ['n']  = '\n',
    ['r']  = '\r',
    ['t']  = '\t',
    ['v']  = '\v',
};

static char curr(struct mcc_lexer* lexer) {
    return *lexer->current;
}

static char next(struct mcc_lexer* lexer) {
    if (*lexer->current == '\n') {
        ++lexer->line;
        lexer->column = 0;
    } else {
        ++lexer->column;
    }
    return *(++lexer->current);
}

static char next_n(struct mcc_lexer* lexer, size_t n) {
    char c = curr(lexer);
    for (size_t i = 0; i < n; i++) {
        c = next(lexer);
    }
    return c;
}

static char peek(struct mcc_lexer* lexer) {
    return lexer->current[1];
}

static char peek_n(struct mcc_lexer* lexer, size_t n) {
    return lexer->current[n];
}

static void skip_whitespace(struct mcc_lexer* lexer) {
    while (isspace(curr(lexer))) {
        next(lexer);
    }
}

static struct mcc_token scan_keyword_or_identifier(struct mcc_lexer* lexer) {
    assert(isident_start(curr(lexer)) && "ensure a valid starting character in identifier or keyword");

    const struct mcc_lexer state = *lexer;

    while (isident(curr(lexer))) {
        next(lexer);
    }

    const struct mcc_string_view lexeme = mcc_string_view_from_ptrs(state.current, lexer->current);
    const enum mcc_keyword keyword      = keyword_lookup(lexeme.data, lexeme.size);

    if (keyword == MCC_KEYWORD_NOT_FOUND) {
        return (struct mcc_token){
            .type   = MCC_TOKEN_TYPE_IDENTIFIER,
            .value  = {.identifier = lexeme},
            .lexeme = lexeme,
            .line   = state.line,
            .column = state.column,
        };
    }

    return (struct mcc_token){
        .type   = MCC_TOKEN_TYPE_KEYWORD,
        .value  = {.keyword = keyword},
        .lexeme = lexeme,
        .line   = state.line,
        .column = state.column,
    };
}

static enum mcc_constant_type parse_suffix(struct mcc_string_view lexeme, bool is_float) {
    if (is_float) {
        return float_suffix_lookup(lexeme.data, lexeme.size);
    } else {
        return integer_suffix_lookup(lexeme.data, lexeme.size);
    }
}

// see ISO C99 6.4.4.1 pg 56 for promotion chain.
// the promotion chain differs for decimal vs hex and octal
static struct mcc_constant parse_number(struct mcc_string_view lexeme, enum mcc_constant_type type, int radix) {
    assert((radix == 8 || radix == 10 || radix == 16) && "valid radix");

    struct mcc_constant constant = {.type = type};

    char* lexeme_end = lexeme.data + lexeme.size;

    const char tmp = *lexeme_end;
    *lexeme_end    = '\0'; // temporarily null-terminate the string for strto* family functions

    char* number_end;

l_retry:
    errno = 0; // strto* will set errno on overflow

    switch (constant.type) {
        case MCC_CONSTANT_TYPE_INT:
            // constant.value.i will have correct binary representation if constant.value.l <= INT_MAX
            constant.value.l = strtol(lexeme.data, &number_end, radix);
            if (errno == ERANGE || (constant.value.l > INT_MAX) || (constant.value.l < INT_MIN)) {
                constant.type = radix == 10 ? MCC_CONSTANT_TYPE_LONG_INT : MCC_CONSTANT_TYPE_UNSIGNED_INT;
                goto l_retry;
            }
            break;
        case MCC_CONSTANT_TYPE_LONG_INT:
            constant.value.l = strtol(lexeme.data, &number_end, radix);
            if (errno == ERANGE) {
                constant.type = radix == 10 ? MCC_CONSTANT_TYPE_LONG_LONG_INT : MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT;
                goto l_retry;
            }
            break;
        case MCC_CONSTANT_TYPE_LONG_LONG_INT:
            constant.value.ll = strtoll(lexeme.data, &number_end, radix);
            if (errno == ERANGE) {
                constant.type = radix == 10 ? MCC_CONSTANT_TYPE_OVERFLOW : MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT;
            }
            break;
        case MCC_CONSTANT_TYPE_UNSIGNED_INT:
            // constant.value.u will have correct binary representation if constant.value.ul <= UINT_MAX
            constant.value.ul = strtoul(lexeme.data, &number_end, radix);
            if (errno == ERANGE || constant.value.ul > UINT_MAX) {
                constant.type = radix == 10 ? MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT : MCC_CONSTANT_TYPE_LONG_INT;
                goto l_retry;
            }
            break;
        case MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT:
            constant.value.ul = strtoul(lexeme.data, &number_end, radix);
            if (errno == ERANGE) {
                constant.type =
                    radix == 10 ? MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT : MCC_CONSTANT_TYPE_LONG_LONG_INT;
                goto l_retry;
            }
            break;
        case MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT:
            constant.value.ull = strtoull(lexeme.data, &number_end, radix);
            if (errno == ERANGE) {
                constant.type = MCC_CONSTANT_TYPE_OVERFLOW;
            }
            break;
        case MCC_CONSTANT_TYPE_FLOAT:
            constant.value.f = strtof(lexeme.data, &number_end);
            if (errno == ERANGE) {
                constant.type = MCC_CONSTANT_TYPE_OVERFLOW;
            }
            break;
        case MCC_CONSTANT_TYPE_DOUBLE:
            constant.value.d = strtod(lexeme.data, &number_end);
            if (errno == ERANGE) {
                constant.type = MCC_CONSTANT_TYPE_OVERFLOW;
            }
            break;
        case MCC_CONSTANT_TYPE_LONG_DOUBLE:
            constant.value.ld = strtold(lexeme.data, &number_end);
            if (errno == ERANGE) {
                constant.type = MCC_CONSTANT_TYPE_OVERFLOW;
            }
            break;
        default:
            assert(false);
    }

    *lexeme_end = tmp; // restore original character

    return constant;
}

static struct mcc_token scan_number(struct mcc_lexer* lexer) {
    const struct mcc_lexer state = *lexer;

    const char* error_message = NULL; // setting this to non-NULL indicates an error / invalid token

    bool is_float           = false;
    bool seen_decimal_point = false;
    bool seen_significand   = false;

    bool is_hex        = false;
    bool maybe_octal   = false;
    bool invalid_octal = false;

    int radix = 10;

    enum mcc_constant_type number_type = MCC_CONSTANT_TYPE_INT;

    char c = curr(lexer);

    if (c == '0') {
        c = next(lexer);
        if (c == 'x' || c == 'X') {
            c = next(lexer);
            if (!isxdigit(c)) {
                error_message = "invalid character sequence in number";
            }
            radix  = 16;
            is_hex = true;
        } else {
            maybe_octal = true; // could be octal (01) or decimal float (0.1)
        }
    }

    // maximal munch
    while (isalnum(c) || c == '.') {
        if (c == '.') {
            if (seen_decimal_point) {
                error_message = "multiple decimal points in number";
            } else if (seen_significand) {
                error_message = "decimal point in exponent";
            }

            is_float           = true;
            seen_decimal_point = true;
            number_type        = MCC_CONSTANT_TYPE_DOUBLE;
        } else if ((!is_hex && (c == 'e' || c == 'E')) || (is_hex && (c == 'p' || c == 'P'))) {
            if (seen_significand) {
                error_message = "invalid character sequence in number";
            }

            is_float         = true;
            seen_significand = true;
            number_type      = MCC_CONSTANT_TYPE_DOUBLE;

            if (peek(lexer) == '+' || peek(lexer) == '-') {
                c = next(lexer);
            }

            if (!isdigit(peek(lexer))) {
                error_message = "invalid character sequence in exponent";
            }
        } else if (((!is_hex || seen_significand) && !isdigit(c)) || (is_hex && !isxdigit(c))) {
            char* suffix_begin = lexer->current;
            do {
                c = next(lexer);
            } while (isalnum(c) || c == '.');
            const char* suffix_end = lexer->current;

            const struct mcc_string_view suffix = mcc_string_view_from_ptrs(suffix_begin, suffix_end);

            number_type = parse_suffix(suffix, is_float);
            if (number_type == MCC_CONSTANT_TYPE_INVALID) {
                error_message = is_float ? "invalid float literal suffix" : "invalid integer literal suffix";
            }
            break; // finding a suffix ends the number (and we already are past the lexeme)
        } else if (maybe_octal && !isodigit(c)) {
            invalid_octal = true;
        }

        c = next(lexer);
    }

    if (is_hex && seen_decimal_point && !seen_significand) {
        error_message = "hexadecimal floating point is not valid outside of binary exponentials";
    } else if (maybe_octal && !is_float) {
        radix = 8;
        if (invalid_octal) {
            error_message = "octal integer literal contains non-octal digits";
        }
    }

    const struct mcc_string_view lexeme = mcc_string_view_from_ptrs(state.current, lexer->current);

    if (error_message) {
        goto l_abort;
    }

    const struct mcc_constant constant = parse_number(lexeme, number_type, radix);

    if (constant.type < 0) {
        switch (constant.type) {
            case MCC_CONSTANT_TYPE_OVERFLOW:
                error_message = "integer overflow/underflow";
                goto l_abort;
            default:
                assert(false);
        }
    }

    return (struct mcc_token){
        .type   = MCC_TOKEN_TYPE_CONSTANT,
        .value  = {.constant = constant},
        .lexeme = lexeme,
        .line   = state.line,
        .column = state.column,
    };

l_abort:
    return (struct mcc_token){
        .type   = MCC_TOKEN_TYPE_INVALID,
        .value  = {.error_message = error_message},
        .lexeme = lexeme,
        .line   = state.line,
        .column = state.column,
    };
}

static struct mcc_token scan_char(struct mcc_lexer* lexer) {
    const struct mcc_lexer state = *lexer;

    const char* error_message = NULL; // setting this to non-NULL indicates an error / invalid token

    bool is_wide = false;
    int ret      = 0;

    char c = curr(lexer);

    if (c == 'L') {
        c       = next(lexer);
        is_wide = true;
    }
    assert(c == '\'' && "must start with single quote");
    c = next(lexer);

    if (c == '\\') {
        // escape sequence
        c = next(lexer);
        if (escape_sequences[(unsigned char)c] != 0) {
            ret = escape_sequences[(unsigned char)c];
            c   = next(lexer);
        } else if (isodigit(c)) {
            // octal escape sequence
            int digits = 0;
            while (isodigit(c)) {
                ret = (ret << 3) | (c - '0');
                c   = next(lexer);
                digits++;
            }
            if (digits > 6) {
                error_message = "octal escape sequence out of range";
            }
        } else if (c == 'u' || c == 'U') {
            // universal escape sequence
            c          = next(lexer);
            int digits = 0;
            while (isxdigit(c)) {
                ret = (ret << 4) | (isdigit(c) ? (c - '0') : (toupper(c) - 'A' + 10));
                c   = next(lexer);
                digits++;
            }
            if (digits != 4) {
                error_message = "invalid universal escape sequence";
            }
        } else if (c == 'x' || c == 'X') {
            // hexadecimal escape sequence
            c          = next(lexer);
            int digits = 0;
            while (isxdigit(c)) {
                ret = (ret << 4) | (isdigit(c) ? (c - '0') : (toupper(c) - 'A' + 10));
                c   = next(lexer);
                digits++;
            }
            if (digits == 0) {
                error_message = "invalid hexadecimal escape sequence";
            }
        } else {
            // invalid escape sequence
            error_message = "invalid escape sequence";
        }
    } else if (c >= 32 && c != 127) {
        // regular character
        ret = c;
        c   = next(lexer);
    } else {
        // control character
        error_message = "invalid character in character literal";
    }

    if (c != '\'') {
        error_message = "unterminated character literal";
        do {
            c = next(lexer);
        } while (c != '\'' && c != '\0');
    } else {
        c = next(lexer);
    }

    const struct mcc_string_view lexeme = mcc_string_view_from_ptrs(state.current, lexer->current);

    if (error_message) {
        return (struct mcc_token){
            .type   = MCC_TOKEN_TYPE_INVALID,
            .value  = {.error_message = error_message},
            .lexeme = lexeme,
            .line   = state.line,
            .column = state.column,
        };
    }

    struct mcc_constant constant;
    if (is_wide) {
        constant = (struct mcc_constant){
            .type  = MCC_CONSTANT_TYPE_WIDE_CHAR,
            .value = {.wc = (wchar_t)ret},
        };
    } else {
        constant = (struct mcc_constant){
            .type  = MCC_CONSTANT_TYPE_CHAR,
            .value = {.c = (char)ret},
        };
    }

    return (struct mcc_token){
        .type   = MCC_TOKEN_TYPE_CONSTANT,
        .value  = {.constant = constant},
        .lexeme = lexeme,
        .line   = state.line,
        .column = state.column,
    };
}

static struct mcc_token scan_string(struct mcc_lexer* lexer) {
    const struct mcc_lexer state = *lexer;

    next(lexer); // TODO

    return (struct mcc_token){
        .type   = MCC_TOKEN_TYPE_INVALID,
        .value  = {.error_message = "TODO"},
        .lexeme = mcc_string_view_from_ptrs(state.current, lexer->current),
        .line   = state.line,
        .column = state.column,
    };
}

static struct mcc_token scan_punctuator(struct mcc_lexer* lexer) {
    const struct mcc_lexer state = *lexer;

    enum mcc_punctuator punctuator;
    char c = curr(lexer);

    switch (c) {
        case '!':
            c = next(lexer);
            switch (c) {
                case '=':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_BANG_EQUAL;
                    break;
                default:
                    punctuator = MCC_PUNCTUATOR_BANG;
                    break;
            }
            break;
        case '#':
            c = next(lexer);
            switch (c) {
                case '#':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_HASH_HASH;
                    break;
                default:
                    punctuator = MCC_PUNCTUATOR_HASH;
                    break;
            }
            break;
        case '%':
            c = next(lexer);
            switch (c) {
                case '=':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_PERCENT_EQUAL;
                    break;
                default:
                    punctuator = MCC_PUNCTUATOR_PERCENT;
                    break;
            }
            break;
        case '&':
            c = next(lexer);
            switch (c) {
                case '&':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_AMPERSAND_AMPERSAND;
                    break;
                case '=':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_AMPERSAND_EQUAL;
                    break;
                default:
                    punctuator = MCC_PUNCTUATOR_AMPERSAND;
                    break;
            }
            break;
        case '(':
            c          = next(lexer);
            punctuator = MCC_PUNCTUATOR_LEFT_PARENTHESIS;
            break;
        case ')':
            c          = next(lexer);
            punctuator = MCC_PUNCTUATOR_RIGHT_PARENTHESIS;
            break;
        case '*':
            c = next(lexer);
            switch (c) {
                case '=':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_ASTERISK_EQUAL;
                    break;
                default:
                    punctuator = MCC_PUNCTUATOR_ASTERISK;
                    break;
            }
            break;
        case '+':
            c = next(lexer);
            switch (c) {
                case '+':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_PLUS_PLUS;
                    break;
                case '=':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_PLUS_EQUAL;
                    break;
                default:
                    punctuator = MCC_PUNCTUATOR_PLUS;
                    break;
            }
            break;
        case ',':
            c          = next(lexer);
            punctuator = MCC_PUNCTUATOR_COMMA;
            break;
        case '-':
            c = next(lexer);
            switch (c) {
                case '-':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_MINUS_MINUS;
                    break;
                case '=':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_MINUS_EQUAL;
                    break;
                case '>':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_ARROW;
                    break;
                default:
                    punctuator = MCC_PUNCTUATOR_MINUS;
                    break;
            }
            break;
        case '.':
            c = next(lexer);
            if (c == '.' && peek(lexer) == '.') {
                c          = next_n(lexer, 2);
                punctuator = MCC_PUNCTUATOR_ELLIPSIS;
            } else {
                punctuator = MCC_PUNCTUATOR_DOT;
            }
            break;
        case '/':
            c = next(lexer);
            switch (c) {
                case '=':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_SLASH_EQUAL;
                    break;
                default:
                    punctuator = MCC_PUNCTUATOR_SLASH;
                    break;
            }
            break;
        case ':':
            c          = next(lexer);
            punctuator = MCC_PUNCTUATOR_COLON;
            break;
        case ';':
            c          = next(lexer);
            punctuator = MCC_PUNCTUATOR_SEMICOLON;
            break;
        case '<':
            c = next(lexer);
            switch (c) {
                case '<':
                    c = next(lexer);
                    switch (c) {
                        case '=':
                            c          = next(lexer);
                            punctuator = MCC_PUNCTUATOR_DOUBLE_LEFT_CHEVRON_EQUAL;
                            break;
                        default:
                            punctuator = MCC_PUNCTUATOR_DOUBLE_LEFT_CHEVRON;
                            break;
                    }
                    break;
                case '=':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_LEFT_CHEVRON_EQUAL;
                    break;
                default:
                    punctuator = MCC_PUNCTUATOR_LEFT_CHEVRON;
                    break;
            }
            break;
        case '=':
            c = next(lexer);
            switch (c) {
                case '=':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_EQUAL_EQUAL;
                    break;
                default:
                    punctuator = MCC_PUNCTUATOR_EQUAL;
                    break;
            }
            break;
        case '>':
            c = next(lexer);
            switch (c) {
                case '>':
                    c = next(lexer);
                    switch (c) {
                        case '=':
                            c          = next(lexer);
                            punctuator = MCC_PUNCTUATOR_DOUBLE_RIGHT_CHEVRON_EQUAL;
                            break;
                        default:
                            punctuator = MCC_PUNCTUATOR_DOUBLE_RIGHT_CHEVRON;
                            break;
                    }
                    break;
                case '=':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_RIGHT_CHEVRON_EQUAL;
                    break;
                default:
                    punctuator = MCC_PUNCTUATOR_RIGHT_CHEVRON;
                    break;
            }
            break;
        case '?':
            c          = next(lexer);
            punctuator = MCC_PUNCTUATOR_QUESTION_MARK;
            break;
        case '[':
            c          = next(lexer);
            punctuator = MCC_PUNCTUATOR_LEFT_BRACKET;
            break;
        case ']':
            c          = next(lexer);
            punctuator = MCC_PUNCTUATOR_RIGHT_BRACKET;
            break;
        case '^':
            c = next(lexer);
            switch (c) {
                case '=':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_CARET_EQUAL;
                    break;
                default:
                    punctuator = MCC_PUNCTUATOR_CARET;
                    break;
            }
            break;
        case '{':
            c          = next(lexer);
            punctuator = MCC_PUNCTUATOR_LEFT_BRACE;
            break;
        case '|':
            c = next(lexer);
            switch (c) {
                case '|':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_PIPE_PIPE;
                    break;
                case '=':
                    c          = next(lexer);
                    punctuator = MCC_PUNCTUATOR_PIPE_EQUAL;
                    break;
                default:
                    punctuator = MCC_PUNCTUATOR_PIPE;
                    break;
            }
            break;
        case '}':
            c          = next(lexer);
            punctuator = MCC_PUNCTUATOR_RIGHT_BRACE;
            break;
        case '~':
            c          = next(lexer);
            punctuator = MCC_PUNCTUATOR_TILDE;
            break;
        default:
            return (struct mcc_token){
                .type   = MCC_TOKEN_TYPE_INVALID,
                .value  = {.error_message = "invalid character sequence"},
                .lexeme = mcc_string_view_from_ptrs(state.current, lexer->current),
                .line   = state.line,
                .column = state.column,
            };
    }

    return (struct mcc_token){
        .type   = MCC_TOKEN_TYPE_PUNCTUATOR,
        .value  = {.punctuator = punctuator},
        .lexeme = mcc_string_view_from_ptrs(state.current, lexer->current),
        .line   = state.line,
        .column = state.column,
    };
}

static struct mcc_token scan_eof(struct mcc_lexer* lexer) {
    return (struct mcc_token){
        .type   = MCC_TOKEN_TYPE_EOF,
        .lexeme = {lexer->current, 0},
        .line   = lexer->line,
        .column = lexer->column,
    };
}

void mcc_lexer_create(struct mcc_lexer* lexer, const char* source, size_t length) {
    assert(lexer && source);
    memset(lexer, 0, sizeof(*lexer));
    lexer->source = malloc(length + 1);
    if (!lexer->source) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memcpy(lexer->source, source, length);
    lexer->source[length] = '\0';
    lexer->current        = lexer->source;
}

void mcc_lexer_destroy(struct mcc_lexer* lexer) {
    assert(lexer);
    free(lexer->source);
    memset(lexer, 0, sizeof(*lexer));
}

struct mcc_token mcc_lexer_next_token(struct mcc_lexer* lexer) {
    assert(lexer && lexer->source && lexer->current);

#ifdef MCC_DEBUG
    skip_whitespace(lexer);

    // Skip single-line comments in debug builds for easier testing
    // Should be handled in the preprocessor in release builds
    while (curr(lexer) == '/' && peek(lexer) == '/') {
        while (curr(lexer) != '\n' && curr(lexer) != '\0') {
            next(lexer);
        }
        skip_whitespace(lexer);
    }
#endif

    skip_whitespace(lexer);

    char c = curr(lexer);

    if (c == '\0') {
        return scan_eof(lexer);
    }

    if (isident_start(c)) {
        return scan_keyword_or_identifier(lexer);
    }

    if (isdigit(c) || (c == '.' && isdigit(peek(lexer)))) {
        return scan_number(lexer);
    }

    if (c == '\'' || (c == 'L' && peek(lexer) == '\'')) {
        return scan_char(lexer);
    }

    if (c == '\"' || (c == 'L' && peek(lexer) == '\"')) {
        return scan_string(lexer);
    }

    return scan_punctuator(lexer);
}
