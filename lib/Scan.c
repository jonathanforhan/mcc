#include "Scan.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "Log.h"
#include "Token.h"

typedef enum {
    OCT = 8,
    DEC = 10,
    HEX = 16,
} Radix;

static inline char _Next(FILE* fptr) {
    return fgetc(fptr);
}

static inline char _Peek(FILE* fptr) {
    int c, r;
    c = fgetc(fptr);
    r = ungetc(c, fptr);
    assert(r != EOF);
    return c;
}

static inline void _Rewind(FILE* fptr, char c) {
    int r = ungetc(c, fptr);
    assert(r != EOF);
}

// int suffix
static ssize_t _ParseConstantSuffix(String str, Token* token) {
    assert(str && token);

    switch (strlen(str)) {
        case 0:
            token->type = TOKEN_INT_CONSTANT;
            break;
        case 1:
            if (tolower(str[0]) == 'u')
                token->type = TOKEN_UNSIGNED_INT_CONSTANT;
            else if (tolower(str[0]) == 'l')
                token->type = TOKEN_LONG_INT_CONSTANT;
            else
                goto abort;
            break;
        case 2:
            if (tolower(str[0]) == 'l' && str[0] == str[1])
                token->type = TOKEN_LONG_LONG_INT_CONSTANT;
            else if (tolower(str[0]) == 'u' && tolower(str[1]) == 'l')
                token->type = TOKEN_UNSIGNED_LONG_INT_CONSTANT;
            else if (tolower(str[0]) == 'l' && tolower(str[1]) == 'u')
                token->type = TOKEN_UNSIGNED_LONG_INT_CONSTANT;
            else
                goto abort;
            break;
        case 3:
            if (tolower(str[0]) == 'u' && tolower(str[1]) == 'l' && str[1] == str[2])
                token->type = TOKEN_UNSIGNED_LONG_LONG_INT_CONSTANT;
            else if (tolower(str[2]) == 'u' && tolower(str[0]) == 'l' && str[0] == str[1])
                token->type = TOKEN_UNSIGNED_LONG_LONG_INT_CONSTANT;
            else
                goto abort;
            break;
        default:
            goto abort;
    }

    return -1;

abort:
    token->data.str = "invalid suffix on integer constant";
    return 0;
}

// floating suffix
static ssize_t _ParseConstantSuffixFP(String str, Token* token) {
    assert(str && token);

    if (strlen(str) > 1)
        goto abort;

    if (strlen(str) == 0) {
        token->type = TOKEN_DOUBLE_CONSTANT;
    } else if (tolower(str[0]) == 'f') {
        token->type = TOKEN_FLOAT_CONSTANT;
    } else if (tolower(str[0]) == 'l') {
        token->type = TOKEN_LONG_DOUBLE_CONSTANT;
    } else {
        goto abort;
    }

    return -1;

abort:
    token->data.str = "invalid suffix on floating point constant";
    return 0;
}

// octal
static ssize_t _ParseConstantOct(String str, Token* token) {
    assert(str && token && token->data.str == NULL);

    char c;
    ssize_t result, i = 0;

    c = str[i];

    while (isdigit(c)) {
        if (c == '8' || c == '9') {
            token->data.str = "unsupported digit in octal constant";
            goto abort;
        }
        c = str[++i];
    }

    token->data.ull = i == 0 ? 0 : strtoull(str, NULL, OCT);

    result = _ParseConstantSuffix(&str[i], token);
    return result == -1 ? result : result + i;

abort:
    return i;
}

// int decimal
static ssize_t _ParseConstantDec(String str, Token* token) {
    assert(str && token && token->data.str == NULL);

    char c;
    ssize_t result, i = 0;

    c = str[i];

    while (isdigit(c))
        c = str[++i];

    token->data.ull = strtoull(str, NULL, DEC);

    result = _ParseConstantSuffix(&str[i], token);
    return result == -1 ? result : result + i;
}

// floating decimal
static ssize_t _ParseConstantDecFP(String str, Token* token) {
    assert(str && token && token->data.str == NULL);

    char c, low_c;
    ssize_t result, i = 0;
    bool seen_dot = false, seen_exp = false;

    c = str[i], low_c = tolower(c);

    while (isdigit(low_c) || low_c == '.' || low_c == 'e' || low_c == '-' || low_c == '+') {
        if (low_c == '.') {
            if (seen_dot) {
                goto abort_dot;
            } else if (seen_exp) {
                goto abort_exp;
            }
            seen_dot = true;
        } else if (low_c == 'e') {
            if (seen_exp) {
                token->data.str = "malformed exponention in floating constant";
                goto abort_exp;
            }
            seen_exp = true;
        }

        c = str[++i], low_c = tolower(c);
    }

    if (-1 == (result = _ParseConstantSuffixFP(&str[i], token) /* <- mutates token.type */)) {
        if (token->type == TOKEN_FLOAT_CONSTANT)
            token->data.f = strtof(str, NULL);
        else if (token->type == TOKEN_DOUBLE_CONSTANT)
            token->data.d = strtod(str, NULL);
        else if (token->type == TOKEN_LONG_DOUBLE_CONSTANT)
            token->data.ld = strtold(str, NULL);
        else
            assert(0);

        return -1; // happy path
    }

    return result + i;

abort_dot:
    token->data.str = "multiple decimal points in floating constant";
    goto abort;

abort_exp:
    token->data.str = "malformed exponention in floating constant";
    goto abort;

abort:
    return i;
}

// int hex
static ssize_t _ParseConstantHex(String str, Token* token) {
    assert(str && token && token->data.str == NULL);

    char c;
    ssize_t result, i = 0;

    c = str[i];

    while (isxdigit(c))
        c = str[++i];

    token->data.ull = strtoull(str, NULL, HEX);

    result = _ParseConstantSuffix(&str[i], token);
    return result == -1 ? result : result + i;
}

// floating hex
static ssize_t _ParseConstantHexFP(String str, Token* token) {
    assert(str && token && token->data.str == NULL);

    char c, low_c;
    ssize_t result, i = 0;
    bool seen_dot = false, seen_exp = false;

    c = str[i], low_c = tolower(c);

    while (isxdigit(low_c) || low_c == '.' || low_c == 'p' || low_c == '-' || low_c == '+') {
        if (low_c == '.') {
            if (seen_dot)
                goto abort_dot;
            else if (seen_exp)
                goto abort_exp;

            seen_dot = true;
        } else if (low_c == 'p') {
            if (seen_exp)
                goto abort_exp;

            seen_exp = true;
        } else if (!(isdigit(low_c) || low_c == '-' || low_c == '+') && seen_exp) {
            // reached prefix
            break;
        }

        c = str[++i], low_c = tolower(c);
    }

    if (-1 == (result = _ParseConstantSuffixFP(&str[i], token) /* <- mutates token.type */)) {
        str -= 2; // we need to include '0x' for strto[f|d|ld] to work

        if (token->type == TOKEN_FLOAT_CONSTANT)
            token->data.f = strtof(str, NULL);
        else if (token->type == TOKEN_DOUBLE_CONSTANT)
            token->data.d = strtod(str, NULL);
        else if (token->type == TOKEN_LONG_DOUBLE_CONSTANT)
            token->data.ld = strtold(str, NULL);
        else
            assert(0);

        return -1; // happy path
    }

    return result + i;

abort_dot:
    token->data.str = "multiple decimal points in floating constant";
    goto abort;

abort_exp:
    token->data.str = "malformed exponention in floating constant";
    goto abort;

abort:
    return i;
}

// returns the index of an error, if no error returns -1
static ssize_t _ParseConstant(String str, Token* token) {
    assert(str && token && StringLength(str) > 0);

    Radix radix = DEC;
    char c, low_c;
    ssize_t result, i = 0;
    bool floating = false;

    token->data.str = NULL;
    c = str[i], low_c = tolower(c);

    // prefix will tell us the radix
    while (low_c == '0') {
        c = str[++i], low_c = tolower(c);

        radix = low_c == 'x' ? HEX : OCT;

        if (radix == HEX) {
            if (i > 1) {
                token->data.str = "invalid prefix on integer constant";
                goto abort;
            }

            c = str[++i], low_c = tolower(c);
            break;
        }
    }

    if (radix == HEX) {
        floating = StringContains(str, "p", 1, false);
    } else {
        floating = StringContains(str, ".", 1, true) || StringContains(str, "e", 1, false);
        radix    = floating ? DEC : radix;
    }

    // parse at i-th index, trucating prefix
    switch (radix) {
        case OCT:
            result = _ParseConstantOct(&str[i], token);
            break;
        case DEC:
            result = floating ? _ParseConstantDecFP(&str[i], token) : _ParseConstantDec(&str[i], token);
            break;
        case HEX:
            result = floating ? _ParseConstantHexFP(&str[i], token) : _ParseConstantHex(&str[i], token);
            break;
        default:
            assert(0);
    }

    return result == -1 ? result : result + i;

abort:
    return i;
}

// static bool _ParseChar() { return true; }
// static bool _ParseLongChar(void) { return true; }
// static bool _ParseStr(void) { return true; }
// static bool _ParseLongStr(void) { return true; }
// static bool _ParseKeywordOrIdentifier(void) { return true; }
// static bool _ParsePunctuator(void) { return true; }

bool Tokenize(const char* filepath, TokenVector* token_vector) {
    assert(filepath && token_vector && *token_vector && TokenVectorLength(*token_vector) == 0);

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
