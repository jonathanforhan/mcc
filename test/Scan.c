#include "Scan.h"
#include <assert.h>
#include <stdio.h>
#include "Log.h"
#include "String.h"

// TODO make test more exhaustive

int main(int argc, char* argv[]) {
    String filepath;
    if (argc >= 2) {
        assert((filepath = StringCreate(argv[1], 100)));
        assert(StringAppend(&filepath, "/test/constants.ctest"));
    } else {
        LOG_INFO("%s", "For manual tests do \"./build/$TEST `pwd`\", this allows the test to find the test files");
        assert((filepath = StringCreate("../../test/constants.ctest", 100)));
    }

    TokenVector tv;
    assert((tv = TokenVectorCreate(sizeof(size_t))));
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
            case TOKEN_CHARACTER_CONSTANT:
            case TOKEN_WIDE_CHARACTER_CONSTANT:
            default:
                LOG_ERROR("%s", "something is very wrong\n");
                exit(-1);
        }
    }

    TokenVectorDestroy(tv);
    StringDestroy(filepath);

    return 0;
}
