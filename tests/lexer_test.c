#include <defs.h>
#include <lexer.h>
#include <mcc.h>
#include <private/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <lexer.c>

static struct mcc_lexer lexer;

void _constant_test_case(char* src, struct mcc_constant expected, bool is_constant) {
    lexer = (struct mcc_lexer){.source = src, .current = src};

    struct mcc_token token = mcc_lexer_next_token(&lexer);
    if (token.type == MCC_TOKEN_TYPE_CONSTANT) {
        switch (token.value.constant.type) {
            case MCC_CONSTANT_TYPE_ENUM:
            case MCC_CONSTANT_TYPE_CHAR:
            case MCC_CONSTANT_TYPE_SIGNED_CHAR:
            case MCC_CONSTANT_TYPE_UNSIGNED_CHAR:
            case MCC_CONSTANT_TYPE_WIDE_CHAR:
                printf("TODO\n");
                break;
            case MCC_CONSTANT_TYPE_INT:
                printf("i   %i %i\n", expected.value.i, token.value.constant.value.i);
                break;
            case MCC_CONSTANT_TYPE_LONG_INT:
                printf("l   %li %li\n", expected.value.l, token.value.constant.value.l);
                break;
            case MCC_CONSTANT_TYPE_LONG_LONG_INT:
                printf("ll  %lli %lli\n", expected.value.ll, token.value.constant.value.ll);
                break;
            case MCC_CONSTANT_TYPE_UNSIGNED_INT:
                printf("u   %u %u\n", expected.value.u, token.value.constant.value.u);
                break;
            case MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT:
                printf("ul  %lu %lu\n", expected.value.ul, token.value.constant.value.ul);
                break;
            case MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT:
                printf("ull %llu %llu\n", expected.value.ull, token.value.constant.value.ull);
                break;
            case MCC_CONSTANT_TYPE_FLOAT:
                printf("f   %f %f\n", expected.value.f, token.value.constant.value.f);
                break;
            case MCC_CONSTANT_TYPE_DOUBLE:
                printf("d   %f %f\n", expected.value.d, token.value.constant.value.d);
                break;
            case MCC_CONSTANT_TYPE_LONG_DOUBLE:
                printf("ld  %Lf %Lf\n", expected.value.ld, token.value.constant.value.ld);
                break;
        }

        assert(is_constant && "Test failed: Expected constant token");
        assert(expected.type == token.value.constant.type && "Test failed: Type mismatch");
        assert(expected.value.ld == token.value.constant.value.ld && "Test failed: Value mismatch");
    } else {
        printf("Skipping non-constant token: %.*s\n", (int)token.lexeme.size, token.lexeme.data);
        assert(!is_constant && "Test failed: Expected non-constant token");
    }
}

#define CONSTANT_TEST_CASE(src, type, member) \
    _constant_test_case(#src, (struct mcc_constant){type, {.member = src}}, true)
#define CONSTANT_TEST_CASE_FAIL(src) _constant_test_case(src, (struct mcc_constant){0}, false)

void test_integer_lexing() {
    printf("=== Integer Constant Tests ===\n");

    // Basic decimal integers
    CONSTANT_TEST_CASE(0, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(1, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(42, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(123, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(999, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(2147483647, MCC_CONSTANT_TYPE_INT, i);

    // Decimal with unsigned suffix
    CONSTANT_TEST_CASE(10u, MCC_CONSTANT_TYPE_UNSIGNED_INT, u);
    CONSTANT_TEST_CASE(10U, MCC_CONSTANT_TYPE_UNSIGNED_INT, u);

    // Decimal with long suffix
    CONSTANT_TEST_CASE(42l, MCC_CONSTANT_TYPE_LONG_INT, l);
    CONSTANT_TEST_CASE(42L, MCC_CONSTANT_TYPE_LONG_INT, l);

    // Decimal with long long suffix
    CONSTANT_TEST_CASE(100ll, MCC_CONSTANT_TYPE_LONG_LONG_INT, ll);
    CONSTANT_TEST_CASE(100LL, MCC_CONSTANT_TYPE_LONG_LONG_INT, ll);

    // Unsigned long combinations
    CONSTANT_TEST_CASE(50ul, MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT, ul);
    CONSTANT_TEST_CASE(50uL, MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT, ul);
    CONSTANT_TEST_CASE(50Ul, MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT, ul);
    CONSTANT_TEST_CASE(50UL, MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT, ul);

    // Unsigned long long combinations
    CONSTANT_TEST_CASE(1000ull, MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT, ull);
    CONSTANT_TEST_CASE(1000uLL, MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT, ull);
    CONSTANT_TEST_CASE(1000Ull, MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT, ull);
    CONSTANT_TEST_CASE(1000ULL, MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT, ull);

    // Octal integers
    CONSTANT_TEST_CASE(00, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(01, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(07, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(010, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(0755, MCC_CONSTANT_TYPE_INT, i);

    // Octal with suffixes
    CONSTANT_TEST_CASE(077u, MCC_CONSTANT_TYPE_UNSIGNED_INT, u);
    CONSTANT_TEST_CASE(0123l, MCC_CONSTANT_TYPE_LONG_INT, l);
    CONSTANT_TEST_CASE(0777ll, MCC_CONSTANT_TYPE_LONG_LONG_INT, ll);

    // Hexadecimal integers
    CONSTANT_TEST_CASE(0x0, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(0x1, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(0xA, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(0xF, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(0xff, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(0xFF, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(0xDEAD, MCC_CONSTANT_TYPE_INT, i);
    CONSTANT_TEST_CASE(0xBEEF, MCC_CONSTANT_TYPE_INT, i);

    // Hex with suffixes
    CONSTANT_TEST_CASE(0x10u, MCC_CONSTANT_TYPE_UNSIGNED_INT, u);
    CONSTANT_TEST_CASE(0xFFl, MCC_CONSTANT_TYPE_LONG_INT, l);
    CONSTANT_TEST_CASE(0xABCDll, MCC_CONSTANT_TYPE_LONG_LONG_INT, ll);
    CONSTANT_TEST_CASE(0xDEADul, MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT, ul);
    CONSTANT_TEST_CASE(0xBEEFull, MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT, ull);

    // Edge cases - zero with suffixes
    CONSTANT_TEST_CASE(0u, MCC_CONSTANT_TYPE_UNSIGNED_INT, u);
    CONSTANT_TEST_CASE(0l, MCC_CONSTANT_TYPE_LONG_INT, l);
    CONSTANT_TEST_CASE(0ll, MCC_CONSTANT_TYPE_LONG_LONG_INT, ll);
    CONSTANT_TEST_CASE(0ul, MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT, ul);
    CONSTANT_TEST_CASE(0ull, MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT, ull);

    printf("\n=== Invalid Integer Tests ===\n");

    // Invalid octal digits
    CONSTANT_TEST_CASE_FAIL("08");
    CONSTANT_TEST_CASE_FAIL("09");
    CONSTANT_TEST_CASE_FAIL("089");
    CONSTANT_TEST_CASE_FAIL("0888");

    // Invalid suffixes
    CONSTANT_TEST_CASE_FAIL("123xyz");
    CONSTANT_TEST_CASE_FAIL("456abc");
    CONSTANT_TEST_CASE_FAIL("42lll");
    CONSTANT_TEST_CASE_FAIL("100uuu");
    CONSTANT_TEST_CASE_FAIL("0xFFgg");

    // Invalid hex
    CONSTANT_TEST_CASE_FAIL("0x");
    CONSTANT_TEST_CASE_FAIL("0X");
}

void test_float_lexing() {
    printf("\n=== Float Constant Tests ===\n");

    // Basic decimal floats
    CONSTANT_TEST_CASE(0.0, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(1.0, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(3.14, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(2.718, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(0.5, MCC_CONSTANT_TYPE_DOUBLE, d);

    // Floats without leading zero
    CONSTANT_TEST_CASE(.5, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(.25, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(.999, MCC_CONSTANT_TYPE_DOUBLE, d);

    // Floats without trailing digits
    CONSTANT_TEST_CASE(1., MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(99., MCC_CONSTANT_TYPE_DOUBLE, d);

    // Scientific notation
    CONSTANT_TEST_CASE(1e0, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(1e1, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(1e10, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(1e-1, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(1e+5, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(2.5e3, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(3.14e-2, MCC_CONSTANT_TYPE_DOUBLE, d);

    // Capital E
    CONSTANT_TEST_CASE(1E10, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(2.5E3, MCC_CONSTANT_TYPE_DOUBLE, d);

    // Float suffixes
    CONSTANT_TEST_CASE(1.0f, MCC_CONSTANT_TYPE_FLOAT, f);
    CONSTANT_TEST_CASE(1.0F, MCC_CONSTANT_TYPE_FLOAT, f);
    CONSTANT_TEST_CASE(3.14f, MCC_CONSTANT_TYPE_FLOAT, f);

    // Long double suffixes
    CONSTANT_TEST_CASE(1.0l, MCC_CONSTANT_TYPE_LONG_DOUBLE, ld);
    CONSTANT_TEST_CASE(1.0L, MCC_CONSTANT_TYPE_LONG_DOUBLE, ld);
    CONSTANT_TEST_CASE(3.14l, MCC_CONSTANT_TYPE_LONG_DOUBLE, ld);

    // Scientific with suffixes
    CONSTANT_TEST_CASE(1e10f, MCC_CONSTANT_TYPE_FLOAT, f);
    CONSTANT_TEST_CASE(2.5e3f, MCC_CONSTANT_TYPE_FLOAT, f);
    CONSTANT_TEST_CASE(1e10L, MCC_CONSTANT_TYPE_LONG_DOUBLE, ld);

    // Hexadecimal floats
    CONSTANT_TEST_CASE(0x1p0, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(0x1p1, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(0x1p-1, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(0x2p2, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(0x1.0p0, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(0x1.8p0, MCC_CONSTANT_TYPE_DOUBLE, d);
    CONSTANT_TEST_CASE(0x1.5p3, MCC_CONSTANT_TYPE_DOUBLE, d);

    // Hex floats with suffixes
    CONSTANT_TEST_CASE(0x1p0f, MCC_CONSTANT_TYPE_FLOAT, f);
    CONSTANT_TEST_CASE(0x1.5p3f, MCC_CONSTANT_TYPE_FLOAT, f);
    CONSTANT_TEST_CASE(0x1p10l, MCC_CONSTANT_TYPE_LONG_DOUBLE, ld);

    printf("\n=== Invalid Float Tests ===\n");

    // Multiple decimal points
    CONSTANT_TEST_CASE_FAIL("1.2.3");
    CONSTANT_TEST_CASE_FAIL("3.14.159");

    // Decimal in exponent
    CONSTANT_TEST_CASE_FAIL("1e2.5");
    CONSTANT_TEST_CASE_FAIL("2e3.14");

    // Multiple exponents
    CONSTANT_TEST_CASE_FAIL("1e2e3");

    // Missing exponent digits
    CONSTANT_TEST_CASE_FAIL("1e");
    CONSTANT_TEST_CASE_FAIL("1e+");
    CONSTANT_TEST_CASE_FAIL("1e-");

    // Hex float without binary exponent
    CONSTANT_TEST_CASE_FAIL("0x1.5");
    CONSTANT_TEST_CASE_FAIL("0xA.B");

    // Invalid suffixes
    CONSTANT_TEST_CASE_FAIL("1.5x");
    CONSTANT_TEST_CASE_FAIL("2.0abc");
    CONSTANT_TEST_CASE_FAIL("1.0ff");
    CONSTANT_TEST_CASE_FAIL("2.5ll");
}

int main() {
    test_integer_lexing();
    test_float_lexing();
    printf("\n=== All Tests Passed! ===\n");
    return 0;
}