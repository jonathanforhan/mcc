#include "Scan.h"
#include <assert.h>
#include <stdio.h>
#include "Log.h"
#include "String.h"
#include "Test.h"

// TODO make test more exhaustive

int main(int argc, char* argv[]) {
    String filepath;
    assert((filepath = StringCreate(NULL, 128)));
    assert(GetTestPath((const char*[]){"../../test/constants.ctest", "./test/constants.ctest"}, 2, &filepath));

    TokenVector tv;
    assert((tv = TokenVectorCreate(0)));
    assert(Tokenize(filepath, &tv));

    for (int i = 0; i < TokenVectorLength(tv); i++) {
        switch (tv[i].type) {
            case TOKEN_INT_CONSTANT:
                printf("%s %d\n", " INT", tv[i].data.i);
                break;
            case TOKEN_LONG_INT_CONSTANT:
                printf("%s %ld\n", " LONG_INT", tv[i].data.l);
                break;
            case TOKEN_LONG_LONG_INT_CONSTANT:
                printf("%s %lld\n", " LONG_LONG_INT", tv[i].data.ll);
                break;
            case TOKEN_UNSIGNED_INT_CONSTANT:
                printf("%s %u\n", " UNSIGNED_INT", tv[i].data.u);
                break;
            case TOKEN_UNSIGNED_LONG_INT_CONSTANT:
                printf("%s %lu\n", " UNSIGNED_LONG_INT", tv[i].data.ul);
                break;
            case TOKEN_UNSIGNED_LONG_LONG_INT_CONSTANT:
                printf("%s %llu\n", " UNSIGNED_LONG_LONG_INT", tv[i].data.ull);
                break;
            case TOKEN_FLOAT_CONSTANT:
                printf("%s %f\n", " FLOAT", tv[i].data.f);
                break;
            case TOKEN_DOUBLE_CONSTANT:
                printf("%s %lf\n", " DOUBLE", tv[i].data.d);
                break;
            case TOKEN_LONG_DOUBLE_CONSTANT:
                printf("%s %Lf\n", " LONG_DOUBLE", tv[i].data.ld);
                break;
            case TOKEN_ENUMERATION_CONSTANT:
                printf("%s\n", "TODO ENUM..."); // TODO
                break;
            case TOKEN_CHARACTER_CONSTANT:
                printf("%s %d %c\n", " CHARACTER", tv[i].data.c, tv[i].data.c);
                break;
            case TOKEN_WIDE_CHARACTER_CONSTANT:
                printf("%s %d %c\n", " WIDE_CHARACTER", tv[i].data.w, tv[i].data.w);
                break;
            default:
                LOG_ERROR("%s", "something is very wrong\n");
                exit(-1);
        }
    }

    TokenVectorDestroy(tv);
    StringDestroy(filepath);

    return 0;
}
