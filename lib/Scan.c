#include "Scan.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "Log.h"
#include "Token.h"

#if defined __GNUC__ || defined __clang__
#define EXPLICIT_FALLTHROUGH __attribute__((fallthrough))
#else
#define EXPLICIT_FALLTHROUGH
#endif

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

// TODO create individual tokenize functions that will call their parse functions
bool Tokenize(const char* filepath, TokenVector* token_vector) {
    assert(filepath && token_vector && *token_vector && TokenVectorLength(*token_vector) == 0);

    FILE* fptr = NULL;
    Token token;
    String buf = NULL;
    char c, prev, peek;
    ssize_t line = 0, bad_index;
    bool escaped = false;

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

        if (feof(fptr))
            break;

        if (isdigit(c) || (c == '.' && isdigit(_Peek(fptr)))) {
            while (isalnum(c) || c == '.' || // maximal munch
                   ((c == '+' || c == '-') && (tolower(prev) == 'e' || tolower(prev) == 'p'))) {
                if (!(StringPush(&buf, c)))
                    goto abort;
                prev = c, c = _Next(fptr);
            }

            _Rewind(fptr, c); // overshot
            bad_index = ParseConstant(buf, &token);
        } else if (isalpha(c)) {
            if (c == 'L' && (peek = _Peek(fptr)) == '\'') {
                peek = _Peek(fptr);
                if (!(StringPush(&buf, c)))
                    goto abort;
                prev = c, c = _Next(fptr);

                do {
                    if (!(StringPush(&buf, c)))
                        goto abort;
                    prev = c, c = _Next(fptr);

                    escaped = prev == '\\' && !escaped;
                } while ((c != '\'' || escaped) && c != EOF);

                if (!(StringPush(&buf, c))) // add closing '
                    goto abort;

                bad_index = ParseLongChar(buf, &token);
            } else if (c == 'L' && peek == '"') {
                // long str
                continue;
            } else {
                // keyword or identifier
                continue;
            }
        } else if (c == '\'') {
            do {
                if (!(StringPush(&buf, c)))
                    goto abort;
                prev = c, c = _Next(fptr);

                escaped = prev == '\\' && !escaped;
            } while ((c != '\'' || escaped) && c != EOF);

            if (!(StringPush(&buf, c))) // add closing '
                goto abort;

            bad_index = ParseChar(buf, &token);
        } else if (c == '"') {
            // str
            continue;
        } else {
            // punctuator
            continue;
        }

        if (!(bad_index < 0)) {
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

ssize_t ParseConstantSuffix(String str, Token* token) {
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

ssize_t ParseConstantSuffixFP(String str, Token* token) {
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

ssize_t ParseConstantOct(String str, Token* token) {
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

    result = ParseConstantSuffix(&str[i], token);
    return result < 0 ? result : result + i;

abort:
    return i;
}

ssize_t ParseConstantDec(String str, Token* token) {
    assert(str && token && token->data.str == NULL);

    char c;
    ssize_t result, i = 0;

    c = str[i];

    while (isdigit(c))
        c = str[++i];

    token->data.ull = strtoull(str, NULL, DEC);

    result = ParseConstantSuffix(&str[i], token);
    return result < 0 ? result : result + i;
}

ssize_t ParseConstantDecFP(String str, Token* token) {
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

    if (-1 == (result = ParseConstantSuffixFP(&str[i], token) /* <- mutates token.type */)) {
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

ssize_t ParseConstantHex(String str, Token* token) {
    assert(str && token && token->data.str == NULL);

    char c;
    ssize_t result, i = 0;

    c = str[i];

    while (isxdigit(c))
        c = str[++i];

    token->data.ull = strtoull(str, NULL, HEX);

    result = ParseConstantSuffix(&str[i], token);
    return result < 0 ? result : result + i;
}

ssize_t ParseConstantHexFP(String str, Token* token) {
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

    if (-1 == (result = ParseConstantSuffixFP(&str[i], token) /* <- mutates token.type */)) {
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

ssize_t ParseConstant(String str, Token* token) {
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
            result = ParseConstantOct(&str[i], token);
            break;
        case DEC:
            result = floating ? ParseConstantDecFP(&str[i], token) : ParseConstantDec(&str[i], token);
            break;
        case HEX:
            result = floating ? ParseConstantHexFP(&str[i], token) : ParseConstantHex(&str[i], token);
            break;
        default:
            assert(0);
    }

    return result < 0 ? result : result + i;

abort:
    return i;
}

ssize_t ParseChar(String str, Token* token) {
    assert(str && token && StringLength(str) > 0);
    assert(str[0] == '\'');

    char c;
    ssize_t i = 0;

    c = str[++i];

    if (c == '\\') {
        // in escape sequence
        c = str[++i];

        switch (c) {
            case '\'':
            case '\"':
            case '\?':
            case '\\':
                break;
            case 'a':
                c = '\a';
                break;
            case 'b':
                c = '\b';
                break;
            case 'f':
                c = '\f';
                break;
            case 'n':
                c = '\n';
                break;
            case 'r':
                c = '\r';
                break;
            case 't':
                c = '\t';
                break;
            case 'v':
                c = '\v';
                break;
            case 'x':
                LOG_ERROR("%s", "hexidecimal escape sequences are WIP"); // TODO
                EXPLICIT_FALLTHROUGH;
            case '0':
                LOG_ERROR("%s", "octal escape sequences are WIP"); // TODO
                EXPLICIT_FALLTHROUGH;
            default:
                token->data.str = "invalid escape sequence";
                goto abort;
        }
    }

    if (str[i + 1] != '\'') {
        token->data.str = "invalid char";
        goto abort;
    }

    token->type   = TOKEN_CHARACTER_CONSTANT;
    token->data.c = c;

    return -1;

abort:
    return i;
}

ssize_t ParseLongChar(String str, Token* token) {
    assert(str[0] == 'L');

    ssize_t result, i = 1;

    result      = ParseChar(&str[i], token);
    token->type = TOKEN_WIDE_CHARACTER_CONSTANT;
    return result < 0 ? result : result + i;
}

// bool ParseStr(void) { return true; }
// bool ParseLongStr(void) { return true; }
// bool ParseKeywordOrIdentifier(void) { return true; }
// bool ParsePunctuator(void) { return true; }
