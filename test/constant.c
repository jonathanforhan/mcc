#include <stdio.h>
#include "scan.h"
#include "token.h"

int main(int argc, char** argv) {
    TokenString ts = TokenStringCreate();

    Tokenize("./build/constant.test.c", &ts);

    for (int i = 0; i < ts.size; i++) {
        TokenType type = ts.tokens[i].type;

        if (type == TOKEN_FLOATING_CONSTANT) {
            printf("%f\n", *(double*)ts.tokens[i].data);
        } else {
            printf("%lu\n", *(uint64_t*)ts.tokens[i].data);
        }
    }

    TokenStringDestroy(&ts);

    return 1;
}
