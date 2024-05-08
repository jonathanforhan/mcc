#include "scan.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "mcc.h"
#include "token.h"
#include "types/string.h"

static char _Next(FILE* fptr) {
    int c = fgetc(fptr);
    return (char)c;
}

static char _Peek(FILE* fptr) {
    int c;

    c = fgetc(fptr);
    ungetc(c, fptr);

    return (char)c;
}

static void _Rewind(FILE* fptr, char c) {
    ungetc(c, fptr);
}

// keyword or identifier
static Result _ParseKeywordOrIdentifier(FILE* fptr, char c, Token* token) {
    return SUCCESS;
}

static Result _ParseConstant(FILE* fptr, char c, Token* token) {
    typedef enum {
        BINARY      = 2,
        OCTAL       = 8,
        DECIMAL     = 10,
        HEXIDECIMAL = 16,
    } Radix;

    Radix radix         = DECIMAL;
    bool floating_point = false;

    uint64_t number = -1;
    int iter        = 0;
    bool run        = true;

    String s = StringCreate(NULL, 0);

    do {
        switch (tolower(c)) {
            case '0':
                if (iter == 0)
                    radix = OCTAL;

                break;
            case '1':
                break;
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                if (radix != HEXIDECIMAL) {
                    if (!floating_point && c == 'e')
                        floating_point = true;
                    else if (c == 'b' && radix == OCTAL)
                        radix = BINARY;
                    else if (floating_point && !isalnum(_Peek(fptr)))
                        break;
                    else
                        goto abort;
                }
                break;
            case 'x':
                if (iter == 1 && radix == OCTAL)
                    radix = HEXIDECIMAL;
                else
                    goto abort;

                break;
            case '.':
                if (radix == OCTAL)
                    radix = DECIMAL;

                if (radix != DECIMAL || floating_point)
                    goto abort;

                floating_point = true;
                break;
            case 'p':
                if (radix != HEXIDECIMAL || floating_point)
                    goto abort;

                floating_point = true;
                break;
            case 'l':
                if (!floating_point || isalnum(_Peek(fptr)))
                    goto abort;

                break;
            default:
                run = false;
        }

        if (run) {
            StringPush(s, c);
            c = _Next(fptr);
            iter++;
        } else {
            _Rewind(fptr, c);
        }
    } while (run);

    // TODO strto(*) is undesirable, write own function
    if (floating_point) {
        *((double*)&number) = strtod(s, NULL);
        token->type         = TOKEN_FLOATING_CONSTANT;
    } else {
        number      = strtoull(s, NULL, radix);
        token->type = TOKEN_INTEGER_CONSTANT;
    }

    token->data = malloc(sizeof(number)); // freed on TokenStringDestroy
    assert(token->data != NULL);          // TODO
    memcpy(token->data, &number, sizeof(number));

    StringDestroy(s);
    return SUCCESS;

abort:
    LOG_ERROR("%s", "unsupported character in constant sequence");
    StringDestroy(s);
    return FAILURE;
}

static Result _ParseStringLiteral(FILE* fptr, char c, Token* token) {
    return SUCCESS;
}

static Result _ParsePunctuator(FILE* fptr, char c, Token* token) {
    return SUCCESS;
}

Result Tokenize(const char* filepath, TokenString* token_string) {
    assert(token_string != NULL);

    FILE* fptr = fopen(filepath, "r");

    if (fptr == NULL) {
        LOG_ERROR("Unable to open file %s", filepath);
        return IO_FAILURE;
    }

    char c;

    for (;;) {
        c = _Next(fptr);

        Result result;
        Token token;

        if (isdigit(c)) {
            result = _ParseConstant(fptr, c, &token);
            TokenStringAppend(token_string, token);
        } else if (isalpha(c)) {
            if (c == 'L' && _Peek(fptr) == '\'') {
                // result = _ParseConstant(fptr, c, &token);
            } else if (c == 'L' && _Peek(fptr) == '"') {
                // result = _ParseStringLiteral(fptr, c, &token);
            }

            // result = _ParseKeywordOrIdentifier(fptr, c, &token);
        } else if (c == '\'') {
            // result = _ParseConstant(fptr, c, &token);
        } else if (c == '"') {
            // result = _ParseStringLiteral(fptr, c, &token);
        } else {
            // result = _ParsePunctuator(fptr, c, &token);
        }

        // TokenStringAppend(token_string, token);

        if (feof(fptr))
            break;
    }

    fclose(fptr);

    return SUCCESS;
}

Result GenerateAST(const TokenString* token_string) {
    // TODO
    (void)token_string;
    return SUCCESS;
}
