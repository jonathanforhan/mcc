#include "Scan.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "Log.h"
#include "Token.h"

typedef enum {
    BINARY      = 2,
    OCTAL       = 8,
    DECIMAL     = 10,
    HEXIDECIMAL = 16,
} Radix;

static inline char _Next(FILE* fptr) {
    return fgetc(fptr);
}

static inline char _Peek(FILE* fptr) {
    int c = fgetc(fptr);
    int r = ungetc(c, fptr);
    assert(r != EOF);
    return c;
}

static inline void _Rewind(FILE* fptr, char c) {
    int r = ungetc(c, fptr);
    assert(r != EOF);
}

// returns the index of an error, if no error returns -1
static ssize_t _ParseConstant(String str, Token* token) {
    assert(str && token && StringLength(str) > 0);

    Radix radix = DECIMAL;
    size_t num, post; // indices for start of number, and postfix
    size_t i       = 0;
    char seperator = 0;
    char c, low_c;
    bool at_post     = false;    // at postfix indicator
    size_t bad_octal = SIZE_MAX; // 0009.1 is a valid decimal float,
                                 // bad_octal == SIZE_MAX if good, or the index of suspect number if bad

    token->data.str = NULL;
    c = str[i], low_c = tolower(c);

    // prefix
    while (low_c == '0') {
        c = str[++i], low_c = tolower(c);

        radix = low_c == 'b' ? BINARY : low_c == 'x' ? HEXIDECIMAL : OCTAL;

        if (radix != OCTAL) {
            if (i > 1) {
                token->data.str = "invalid suffix on integer constant";
                goto abort;
            }

            c = str[++i], low_c = tolower(c);
            break;
        }
    }

    num = i;

    // number
    while ((isalnum(low_c) || low_c == '.' || low_c == '-' || low_c == '+') && !at_post) {
        switch (radix) {
            case BINARY:
                if (low_c == 'l' || low_c == 'u') {
                    at_post = true;
                } else if (low_c != '0' && low_c != '1') {
                    token->data.str = "invalid digit in binary constant";
                    goto abort;
                }
                break;
            case OCTAL:
                if (low_c == '.' || low_c == 'e')
                    seperator = c, radix = DECIMAL;
                else if (low_c == 'l' || low_c == 'u')
                    at_post = true;
                else if (!(low_c >= '0' && low_c < '8'))
                    bad_octal = i;
                break;
            case DECIMAL:
                if (low_c == '.') {
                    if (seperator) {
                        token->data.str = "invalid floating point constant decimal format";
                        goto abort;
                    }
                    seperator = c;
                } else if (low_c == 'e') {
                    // 0.1e2 is valid
                    if (seperator == 'e') {
                        token->data.str = "invalid floating point constant exponent format";
                        goto abort;
                    }
                    seperator = c;
                } else if ((low_c == 'f' && seperator) || low_c == 'l' || low_c == 'u') {
                    at_post = true;
                } else if (!isdigit(low_c)) {
                    if (!(seperator && tolower(str[i - 1]) == 'e' && (low_c == '-' || low_c == '+'))) {
                        token->data.str = "invalid suffix on decimal constant";
                        goto abort;
                    }
                }
                break;
            case HEXIDECIMAL:
                if (low_c == '.') {
                    token->data.str = "invalid floating point constant decimal format";
                    goto abort;
                } else if (low_c == 'p') {
                    if (seperator) {
                        token->data.str = "invalid floating point constant exponent format";
                        goto abort;
                    }
                    seperator = c;
                } else if ((low_c == 'f' && seperator) || low_c == 'l' || low_c == 'u') {
                    at_post = true;
                } else if (!isxdigit(low_c)) {
                    if (!(seperator && tolower(str[i - 1]) == 'p' && (low_c == '-' || low_c == '+'))) {
                        token->data.str = "invalid suffix on hexidecimal constant";
                        goto abort;
                    }
                }
                break;
        }

        if (!at_post)
            c = str[++i], low_c = tolower(c);
    }

    if (radix == OCTAL && bad_octal != SIZE_MAX) {
        token->data.str = "invalid digit in octal constant";
        i               = bad_octal; // for error formatting purposes
        goto abort;
    }

    post = i;

    token->type = seperator ? TOKEN_DOUBLE_CONSTANT : TOKEN_INT_CONSTANT;

    switch (StringLength(str) - post) { // length of postfix
        case 0:
            break;
        case 1:
            if (seperator) {
                if (str[post] == 'f' || str[post] == 'F') {
                    token->type = TOKEN_FLOAT_CONSTANT;
                } else if (str[post] == 'l' || str[post] == 'L') {
                    token->type = TOKEN_LONG_DOUBLE_CONSTANT;
                } else {
                    token->data.str = "invalid suffix on floating point constant";
                    goto abort;
                }
            } else {
                if (str[post] == 'u' || str[post] == 'U') {
                    token->type = TOKEN_UNSIGNED_INT_CONSTANT;
                } else if (str[post] == 'l' || str[post] == 'L') {
                    token->type = TOKEN_LONG_INT_CONSTANT;
                } else {
                    token->data.str = "invalid suffix on integer constant";
                    goto abort;
                }
            }
            break;
        case 2:
            if (seperator) {
                token->data.str = "invalid suffix on floating point constant";
                goto abort;
            }

            if (strcmp(&str[post], "ll") == 0 || strcmp(&str[post], "LL") == 0) {
                token->type = TOKEN_LONG_LONG_INT_CONSTANT;
            } else if ((tolower(str[post]) == 'u' || tolower(str[post]) == 'U') &&
                       (tolower(str[post + 1]) == 'l' || tolower(str[post + 1]) == 'L')) {
                token->type = TOKEN_UNSIGNED_LONG_INT_CONSTANT;
            } else if ((tolower(str[post]) == 'l' || tolower(str[post]) == 'L') &&
                       (tolower(str[post + 1]) == 'u' || tolower(str[post + 1]) == 'U')) {
                token->type = TOKEN_UNSIGNED_LONG_INT_CONSTANT;
            } else {
                token->data.str = "invalid suffix on integer constant";
                goto abort;
            }
            break;
        case 3:
            if (seperator) {
                token->data.str = "invalid suffix on floating point constant";
                goto abort;
            }

            if ((str[post + 2] == 'u' || str[post + 2] == 'U') &&
                (strncmp(&str[post], "ll", 2) == 0 || strncmp(&str[post], "LL", 2) == 0)) {
                token->type = TOKEN_UNSIGNED_LONG_LONG_INT_CONSTANT;
            } else if ((str[post] == 'u' || str[post] == 'U') &&
                       (strncmp(&str[post + 1], "ll", 2) == 0 || strncmp(&str[post + 1], "LL", 2) == 0)) {
                token->type = TOKEN_UNSIGNED_LONG_LONG_INT_CONSTANT;
            } else {
                token->data.str = "invalid suffix on integer constant";
                goto abort;
            }
            break;
        default:
            if (seperator)
                token->data.str = "invalid suffix on floating point constant";
            else
                token->data.str = "invalid suffix on integer constant";
            goto abort;
    }

    if (seperator) {
        switch (token->type) {
            case TOKEN_FLOAT_CONSTANT:
                token->data.f = (float)strtod(&str[num], NULL);
                break;
            case TOKEN_DOUBLE_CONSTANT:
                token->data.d = strtod(&str[num], NULL);
                break;
            case TOKEN_LONG_DOUBLE_CONSTANT:
                token->data.ld = strtold(&str[num], NULL);
                break;
            default:
                LOG_ERROR("%s", "unreachable condition reached");
                exit(1);
        }
    } else {
        token->data.ull = strtoull(&str[num], NULL, radix);
    }

    //--- TODO

    // printf("[");
    // for (i = 0; i < num; i++)
    //     putc(str[i], stdout);
    // printf("][");
    // for (; i < post; i++)
    //     putc(str[i], stdout);
    // printf("][");
    // for (; i < StringLength(str); i++)
    //     putc(str[i], stdout);
    // printf("]\n");

    //--- TODO

    return -1; // happy path

abort:
    return i;
}

// static bool _ParseChar(void) { return true; }
// static bool _ParseLongChar(void) { return true; }
// static bool _ParseStr(void) { return true; }
// static bool _ParseLongStr(void) { return true; }
// static bool _ParseKeywordOrIdentifier(void) { return true; }
// static bool _ParsePunctuator(void) { return true; }

bool Tokenize(const char* filepath, TokenVector* token_vector) {
    assert(filepath);

    FILE* fptr = NULL;
    Token token;
    String buf = NULL;
    char c, prev, peek;
    ssize_t line = 0, bad_index;

    if (!(fptr = fopen(filepath, "r"))) {
        LOG_ERROR("unable to open file %s", filepath);
        goto abort;
    }

    if (!(buf = StringCreate(NULL, sizeof(size_t))))
        goto abort;

    for (c = '\0';;) {
        prev = c, c = _Next(fptr);

        if (c == '\n') {
            line++;
            continue;
        }

        if (isdigit(c) || (c == '.' && isdigit(_Peek(fptr)))) {
            while (isalnum(c) || c == '.' || // maximal munch
                   ((c == '+' || c == '-') && (tolower(prev) == 'e' || tolower(prev) == 'p'))) {
                if (!(StringPush(&buf, c)))
                    goto abort;
                prev = c, c = _Next(fptr);
            }
            _Rewind(fptr, c); // overshot

            if ((bad_index = _ParseConstant(buf, &token)) > 0) {
                LogDetailedError(&(DetailedLog){
                    .filename     = filepath,
                    .line         = line,
                    .error        = buf,
                    .bad_index    = bad_index,
                    .error_length = StringLength(buf) - bad_index,
                    .explaination = token.data.str, // statically allocated don't worry
                });
            }

            StringClear(&buf);

            if (!(TokenVectorPush(token_vector, &token)))
                goto abort;
        } else if (isalpha(c)) {
            if (c == 'L') {
                peek = _Peek(fptr);
                if (peek == '\'') {
                    // long char
                } else if (peek == '"') {
                    // long str
                }
            }
            // keyword or identifier
        } else if (c == '\'') {
            // char
        } else if (c == '"') {
            // str
        } else {
            // punctuator
        }

        if (feof(fptr))
            break;
    }

    fclose(fptr);

    StringDestroy(buf);

    return true;

abort:
    if (fptr)
        fclose(fptr);

    if (buf)
        StringDestroy(buf);

    return true;
}
