#include "lexer.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "private/utils.h"

struct keyword_entry {
    const char* text;
    enum mcc_keyword keyword;
};

static const struct keyword_entry keyword_table[] = {
    {"auto", MCC_KEYWORD_AUTO},
    {"break", MCC_KEYWORD_BREAK},
    {"case", MCC_KEYWORD_CASE},
    {"char", MCC_KEYWORD_CHAR},
    {"const", MCC_KEYWORD_CONST},
    {"continue", MCC_KEYWORD_CONTINUE},
    {"default", MCC_KEYWORD_DEFAULT},
    {"do", MCC_KEYWORD_DO},
    {"double", MCC_KEYWORD_DOUBLE},
    {"else", MCC_KEYWORD_ELSE},
    {"enum", MCC_KEYWORD_ENUM},
    {"extern", MCC_KEYWORD_EXTERN},
    {"float", MCC_KEYWORD_FLOAT},
    {"for", MCC_KEYWORD_FOR},
    {"goto", MCC_KEYWORD_GOTO},
    {"if", MCC_KEYWORD_IF},
    {"inline", MCC_KEYWORD_INLINE},
    {"int", MCC_KEYWORD_INT},
    {"long", MCC_KEYWORD_LONG},
    {"register", MCC_KEYWORD_REGISTER},
    {"restrict", MCC_KEYWORD_RESTRICT},
    {"return", MCC_KEYWORD_RETURN},
    {"short", MCC_KEYWORD_SHORT},
    {"signed", MCC_KEYWORD_SIGNED},
    {"sizeof", MCC_KEYWORD_SIZEOF},
    {"static", MCC_KEYWORD_STATIC},
    {"struct", MCC_KEYWORD_STRUCT},
    {"switch", MCC_KEYWORD_SWITCH},
    {"typedef", MCC_KEYWORD_TYPEDEF},
    {"union", MCC_KEYWORD_UNION},
    {"unsigned", MCC_KEYWORD_UNSIGNED},
    {"void", MCC_KEYWORD_VOID},
    {"volatile", MCC_KEYWORD_VOLATILE},
    {"while", MCC_KEYWORD_WHILE},
    {"_Bool", MCC_KEYWORD_BOOL},
    {"_Complex", MCC_KEYWORD_COMPLEX},
    {"_Imaginary", MCC_KEYWORD_IMAGINARY},
};

static enum mcc_keyword keyword_lookup(const char* str, size_t len) {
    for (size_t i = 0; i < ARRAY_SIZE(keyword_table); i++) {
        if (strlen(keyword_table[i].text) != len) {
            continue;
        }

        if (strncmp(keyword_table[i].text, str, len) == 0) {
            return keyword_table[i].keyword;
        }
    }
    return MCC_KEYWORD_NOT_FOUND;
}

struct suffix_entry {
    const char* text;
    enum mcc_constant_type type;
};

static const struct suffix_entry integer_suffix_table[] = {
    {"U", MCC_CONSTANT_TYPE_UNSIGNED_INT},
    {"L", MCC_CONSTANT_TYPE_LONG_INT},
    {"UL", MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT},
    {"LU", MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT},
    {"LL", MCC_CONSTANT_TYPE_LONG_LONG_INT},
    {"ULL", MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT},
    {"LLU", MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT},
};

static enum mcc_constant_type integer_suffix_lookup(const char* str, size_t len) {
    if (len > 3) {
        return -1; // No valid suffix is longer than 3 characters
    }

    char suffix[4] = {0};
    for (size_t i = 0; i < len; i++) {
        suffix[i] = (char)toupper(str[i]);
    }

    for (size_t i = 0; i < ARRAY_SIZE(integer_suffix_table); i++) {
        if (strlen(integer_suffix_table[i].text) != len) {
            continue;
        }

        if (strncmp(integer_suffix_table[i].text, suffix, len) == 0) {
            return integer_suffix_table[i].type;
        }
    }
    return -1;
}

static const struct suffix_entry float_suffix_table[] = {
    {"F", MCC_CONSTANT_TYPE_FLOAT},
    {"L", MCC_CONSTANT_TYPE_LONG_DOUBLE},
};

static enum mcc_constant_type float_suffix_lookup(const char* str, size_t len) {
    if (len > 1) {
        return -1; // No valid suffix is longer than 1 character
    }

    char suffix[2] = {(char)toupper(str[0]), '\0'};

    for (size_t i = 0; i < ARRAY_SIZE(float_suffix_table); i++) {
        if (strlen(float_suffix_table[i].text) != len) {
            continue;
        }

        if (strncmp(float_suffix_table[i].text, suffix, len) == 0) {
            return float_suffix_table[i].type;
        }
    }
    return -1;
}

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
    const struct mcc_lexer state = *lexer;

    while (isident(curr(lexer))) {
        next(lexer);
    }

    struct mcc_string_view lexeme = {
        .data = state.current,
        .size = lexer->current - state.current,
    };
    enum mcc_keyword keyword = keyword_lookup(lexeme.data, lexeme.size);

    if (keyword != MCC_KEYWORD_NOT_FOUND) {
        return (struct mcc_token){
            .type   = MCC_TOKEN_TYPE_KEYWORD,
            .value  = {.keyword = keyword},
            .lexeme = lexeme,
            .line   = state.line,
            .column = state.column,
        };
    } else {
        return (struct mcc_token){
            .type   = MCC_TOKEN_TYPE_IDENTIFIER,
            .value  = {.identifier = lexeme},
            .lexeme = lexeme,
            .line   = state.line,
            .column = state.column,
        };
    }
}

static struct mcc_constant parse_integer(struct mcc_string_view lexeme, int radix, int suffix_position) {
    struct mcc_constant constant = {.type = MCC_CONSTANT_TYPE_INT};

    if (suffix_position > 0) {
        constant.type = integer_suffix_lookup(lexeme.data + suffix_position, lexeme.size - suffix_position);
        assert(constant.type >= 0 && "must be valid");
    }
    size_t number_end = (size_t)(suffix_position < 0 ? lexeme.size : suffix_position);

    char tmp                = lexeme.data[number_end];
    lexeme.data[number_end] = '\0'; // temporarily null-terminate the string for strto* family functions

    switch (constant.type) {
        case MCC_CONSTANT_TYPE_INT:
            constant.value.i = (int)strtol(lexeme.data, NULL, radix);
            break;
        case MCC_CONSTANT_TYPE_LONG_INT:
            constant.value.l = strtol(lexeme.data, NULL, radix);
            break;
        case MCC_CONSTANT_TYPE_LONG_LONG_INT:
            constant.value.ll = strtoll(lexeme.data, NULL, radix);
            break;
        case MCC_CONSTANT_TYPE_UNSIGNED_INT:
            constant.value.u = (unsigned int)strtoul(lexeme.data, NULL, radix);
            break;
        case MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT:
            constant.value.ul = strtoul(lexeme.data, NULL, radix);
            break;
        case MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT:
            constant.value.ull = strtoull(lexeme.data, NULL, radix);
            break;
        default:
            assert(false);
    }

    lexeme.data[number_end] = tmp; // restore original character

    return constant;
}

static struct mcc_constant parse_float(struct mcc_string_view lexeme, int suffix_position) {
    struct mcc_constant constant = {.type = MCC_CONSTANT_TYPE_DOUBLE};

    if (suffix_position > 0) {
        constant.type = float_suffix_lookup(lexeme.data + suffix_position, lexeme.size - suffix_position);
        assert(constant.type >= 0 && "must be valid");
    }
    size_t number_end = (size_t)(suffix_position < 0 ? lexeme.size : suffix_position);

    char tmp                = lexeme.data[number_end];
    lexeme.data[number_end] = '\0'; // temporarily null-terminate the string for strto* family functions

    switch (constant.type) {
        case MCC_CONSTANT_TYPE_FLOAT:
            constant.value.f = strtof(lexeme.data, NULL);
            break;
        case MCC_CONSTANT_TYPE_DOUBLE:
            constant.value.d = strtod(lexeme.data, NULL);
            break;
        case MCC_CONSTANT_TYPE_LONG_DOUBLE:
            constant.value.ld = strtold(lexeme.data, NULL);
            break;
        default:
            assert(false);
    }

    lexeme.data[number_end] = tmp; // restore original character

    return constant;
}

static struct mcc_token scan_number(struct mcc_lexer* lexer) {
    const struct mcc_lexer state = *lexer;

    const char* error_message = NULL; // setting this to non-NULL indicates an error / invalid token

    bool is_float           = false;
    bool seen_decimal_point = false;
    bool seen_significand   = false;

    bool maybe_octal   = false;
    bool invalid_octal = false;

    int radix           = 10;
    int suffix_position = -1;

    if (curr(lexer) == '0') {
        if (peek(lexer) == 'x' || peek(lexer) == 'X') {
            if (!isxdigit(peek_n(lexer, 2))) {
                error_message = "Invalid character sequence in number";
            }
            radix = 16;
            next_n(lexer, 2);
        } else {
            radix       = 10;
            maybe_octal = true; // could be octal (01) or decimal float (0.1)
            next(lexer);
        }
    }

    // maximal munch
    char c = curr(lexer);
    while (isalnum(c) || c == '.') {
        if (c == '.') {
            if (seen_decimal_point) {
                error_message = "Multiple decimal points in number";
            } else if (seen_significand) {
                error_message = "Decimal point in exponent";
            }
            is_float = seen_decimal_point = true;
        } else if ((radix != 16) && (c == 'e' || c == 'E')) {
            if (seen_significand) {
                error_message = "Invalid character sequence in number";
            }
            is_float = seen_significand = true;

            if ((peek(lexer) == '+' || peek(lexer) == '-')) {
                c = next(lexer);
            }

            if (!isdigit(peek(lexer))) {
                error_message = "Invalid character sequence in exponent";
            }
        } else if ((radix == 16) && (c == 'p' || c == 'P')) {
            if (seen_significand) {
                error_message = "Invalid character sequence in number";
            }
            is_float = seen_significand = true;

            if ((peek(lexer) == '+' || peek(lexer) == '-')) {
                c = next(lexer);
            }

            if (!isdigit(peek(lexer))) {
                error_message = "Invalid character sequence in exponent";
            }
        } else if (((seen_significand || radix == 10) && !isdigit(c)) || (radix == 16 && !isxdigit(c))) {
            suffix_position          = (int)(lexer->current - state.current);
            const char* suffix_start = lexer->current;
            do {
                c = next(lexer);
            } while (isalnum(c) || c == '.');
            const char* suffix_end = lexer->current;

            enum mcc_constant_type type = is_float ? float_suffix_lookup(suffix_start, suffix_end - suffix_start)
                                                   : integer_suffix_lookup(suffix_start, suffix_end - suffix_start);
            if (type < 0) {
                error_message = is_float ? "Invalid float literal suffix" : "Invalid integer literal suffix";
            }
            break; // finding a suffix ends the number
        } else if (maybe_octal && !isodigit(c)) {
            invalid_octal = true;
        }

        c = next(lexer);
    }

    struct mcc_string_view lexeme = {
        .data = state.current,
        .size = lexer->current - state.current,
    };

    if ((radix == 16) && seen_decimal_point && !seen_significand) {
        error_message = "Hexadecimal floating point is not valid outside of binary exponentials";
    } else if (maybe_octal && !is_float) {
        radix = 8;
        if (invalid_octal) {
            error_message = "Octal integer literal contains non-octal digits";
        }
    }

    if (error_message) {
        return (struct mcc_token){
            .type   = MCC_TOKEN_TYPE_INVALID,
            .value  = {.error_message = error_message},
            .lexeme = lexeme,
            .line   = state.line,
            .column = state.column,
        };
    }

    struct mcc_constant constant =
        is_float ? parse_float(lexeme, suffix_position) : parse_integer(lexeme, radix, suffix_position);

    return (struct mcc_token){
        .type   = MCC_TOKEN_TYPE_CONSTANT,
        .value  = {.constant = constant},
        .lexeme = lexeme,
        .line   = state.line,
        .column = state.column,
    };
}

static struct mcc_token scan_char(struct mcc_lexer* lexer) {
    const struct mcc_lexer state = *lexer;

    return (struct mcc_token){
        .type   = MCC_TOKEN_TYPE_INVALID,
        .value  = {.error_message = "TODO"},
        .lexeme = {.data = state.current, .size = (size_t)(lexer->current - state.current)},
        .line   = state.line,
        .column = state.column,
    };
}

static struct mcc_token scan_string(struct mcc_lexer* lexer) {
    const struct mcc_lexer state = *lexer;

    return (struct mcc_token){
        .type   = MCC_TOKEN_TYPE_INVALID,
        .value  = {.error_message = "TODO"},
        .lexeme = {.data = state.current, .size = (size_t)(lexer->current - state.current)},
        .line   = state.line,
        .column = state.column,
    };
}

static struct mcc_token scan_punctuator(struct mcc_lexer* lexer) {
    const struct mcc_lexer state = *lexer;

    return (struct mcc_token){
        .type   = MCC_TOKEN_TYPE_INVALID,
        .value  = {.error_message = "TODO"},
        .lexeme = {.data = state.current, .size = (size_t)(lexer->current - state.current)},
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
    if (!(lexer->source = (char*)malloc(length + 1))) {
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

#if !defined(NDEBUG) || defined(_DEBUG)
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
        next(lexer);
        return scan_char(lexer);
    }

    if (c == '\"' || (c == 'L' && peek(lexer) == '\"')) {
        next(lexer);
        return scan_string(lexer);
    }

    next(lexer);
    return scan_punctuator(lexer);
}
