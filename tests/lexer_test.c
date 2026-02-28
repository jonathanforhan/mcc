/// @file test/test_lexer.c
/// @brief Lexer unit tests for the MCC C99 compiler.

#include <defs.h>
#include <lexer.h>
#include <private/utils.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_tests_run    = 0;
static int g_tests_passed = 0;
static int g_tests_failed = 0;

#define TEST_PASS()       \
    do {                  \
        g_tests_run++;    \
        g_tests_passed++; \
    } while (0)

#define TEST_FAIL(fmt, ...)                                                                    \
    do {                                                                                       \
        g_tests_run++;                                                                         \
        g_tests_failed++;                                                                      \
        (void)fprintf(stderr, "  FAIL [%s:%d]: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define EXPECT(cond, fmt, ...)             \
    do {                                   \
        if ((cond)) {                      \
            TEST_PASS();                   \
        } else {                           \
            TEST_FAIL(fmt, ##__VA_ARGS__); \
        }                                  \
    } while (0)

#define TEST_SUITE(name)                \
    do {                                \
        printf("\n=== " name " ===\n"); \
    } while (0)

static void print_results(void) {
    printf("\n----------------------------------------\n");
    printf("Results: %d/%d passed", g_tests_passed, g_tests_run);
    if (g_tests_failed > 0) {
        printf(", %d FAILED", g_tests_failed);
    }
    printf("\n");
}

// =============================================================================
// Lexer Helpers
// =============================================================================

/// @brief Lex a single token from a null-terminated source string.
static struct mcc_token lex_one(const char* src) {
    struct mcc_lexer lexer;
    mcc_lexer_create(&lexer, src, strlen(src));
    struct mcc_token tok = mcc_lexer_next_token(&lexer);
    mcc_lexer_destroy(&lexer);
    return tok;
}

// =============================================================================
// Expect Helpers
// =============================================================================

static void expect_keyword(const char* src, enum mcc_keyword expected) {
    struct mcc_token tok = lex_one(src);
    if (tok.type != MCC_TOKEN_TYPE_KEYWORD) {
        TEST_FAIL("'%s': expected KEYWORD, got token type %d", src, tok.type);
        return;
    }
    EXPECT(tok.value.keyword == expected, "'%s': keyword %d != expected %d", src, tok.value.keyword, expected);
}

static void expect_identifier(const char* src) {
    struct mcc_token tok = lex_one(src);
    EXPECT(tok.type == MCC_TOKEN_TYPE_IDENTIFIER, "'%s': expected IDENTIFIER, got token type %d", src, tok.type);
}

static void expect_punctuator(const char* src, enum mcc_punctuator expected) {
    struct mcc_token tok = lex_one(src);
    if (tok.type != MCC_TOKEN_TYPE_PUNCTUATOR) {
        TEST_FAIL("'%s': expected PUNCTUATOR, got token type %d", src, tok.type);
        return;
    }
    EXPECT(tok.value.punctuator == expected, "'%s': punctuator %d != expected %d", src, tok.value.punctuator, expected);
}

static void expect_invalid(const char* src) {
    struct mcc_token tok = lex_one(src);
    EXPECT(tok.type == MCC_TOKEN_TYPE_INVALID, "'%s': expected INVALID, got token type %d", src, tok.type);
}

// Constant helpers

static void expect_int_constant(const char* src, int expected) {
    struct mcc_token tok = lex_one(src);
    if (tok.type != MCC_TOKEN_TYPE_CONSTANT) {
        TEST_FAIL("'%s': expected CONSTANT, got token type %d", src, tok.type);
        return;
    }
    EXPECT(tok.value.constant.type == MCC_CONSTANT_TYPE_INT,
           "'%s': expected MCC_CONSTANT_TYPE_INT, got %d",
           src,
           tok.value.constant.type);
    EXPECT(tok.value.constant.value.i == expected,
           "'%s': value %d != expected %d",
           src,
           tok.value.constant.value.i,
           expected);
}

static void expect_uint_constant(const char* src, unsigned int expected) {
    struct mcc_token tok = lex_one(src);
    if (tok.type != MCC_TOKEN_TYPE_CONSTANT) {
        TEST_FAIL("'%s': expected CONSTANT, got token type %d", src, tok.type);
        return;
    }
    EXPECT(tok.value.constant.type == MCC_CONSTANT_TYPE_UNSIGNED_INT,
           "'%s': expected MCC_CONSTANT_TYPE_UNSIGNED_INT, got %d",
           src,
           tok.value.constant.type);
    EXPECT(tok.value.constant.value.u == expected,
           "'%s': value %u != expected %u",
           src,
           tok.value.constant.value.u,
           expected);
}

static void expect_long_constant(const char* src, long expected) {
    struct mcc_token tok = lex_one(src);
    if (tok.type != MCC_TOKEN_TYPE_CONSTANT) {
        TEST_FAIL("'%s': expected CONSTANT, got token type %d", src, tok.type);
        return;
    }
    EXPECT(tok.value.constant.type == MCC_CONSTANT_TYPE_LONG_INT,
           "'%s': expected MCC_CONSTANT_TYPE_LONG_INT, got %d",
           src,
           tok.value.constant.type);
    EXPECT(tok.value.constant.value.l == expected,
           "'%s': value %li != expected %li",
           src,
           tok.value.constant.value.l,
           expected);
}

static void expect_ulong_constant(const char* src, unsigned long expected) {
    struct mcc_token tok = lex_one(src);
    if (tok.type != MCC_TOKEN_TYPE_CONSTANT) {
        TEST_FAIL("'%s': expected CONSTANT, got token type %d", src, tok.type);
        return;
    }
    EXPECT(tok.value.constant.type == MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT,
           "'%s': expected MCC_CONSTANT_TYPE_UNSIGNED_LONG_INT, got %d",
           src,
           tok.value.constant.type);
    EXPECT(tok.value.constant.value.ul == expected,
           "'%s': value %lu != expected %lu",
           src,
           tok.value.constant.value.ul,
           expected);
}

static void expect_llong_constant(const char* src, long long expected) {
    struct mcc_token tok = lex_one(src);
    if (tok.type != MCC_TOKEN_TYPE_CONSTANT) {
        TEST_FAIL("'%s': expected CONSTANT, got token type %d", src, tok.type);
        return;
    }
    EXPECT(tok.value.constant.type == MCC_CONSTANT_TYPE_LONG_LONG_INT,
           "'%s': expected MCC_CONSTANT_TYPE_LONG_LONG_INT, got %d",
           src,
           tok.value.constant.type);
    EXPECT(tok.value.constant.value.ll == expected,
           "'%s': value %lli != expected %lli",
           src,
           tok.value.constant.value.ll,
           expected);
}

static void expect_ullong_constant(const char* src, unsigned long long expected) {
    struct mcc_token tok = lex_one(src);
    if (tok.type != MCC_TOKEN_TYPE_CONSTANT) {
        TEST_FAIL("'%s': expected CONSTANT, got token type %d", src, tok.type);
        return;
    }
    EXPECT(tok.value.constant.type == MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT,
           "'%s': expected MCC_CONSTANT_TYPE_UNSIGNED_LONG_LONG_INT, got %d",
           src,
           tok.value.constant.type);
    EXPECT(tok.value.constant.value.ull == expected,
           "'%s': value %llu != expected %llu",
           src,
           tok.value.constant.value.ull,
           expected);
}

static void expect_float_constant(const char* src, float expected) {
    struct mcc_token tok = lex_one(src);
    if (tok.type != MCC_TOKEN_TYPE_CONSTANT) {
        TEST_FAIL("'%s': expected CONSTANT, got token type %d", src, tok.type);
        return;
    }
    EXPECT(tok.value.constant.type == MCC_CONSTANT_TYPE_FLOAT,
           "'%s': expected MCC_CONSTANT_TYPE_FLOAT, got %d",
           src,
           tok.value.constant.type);
    EXPECT(tok.value.constant.value.f == expected,
           "'%s': value %f != expected %f",
           src,
           (double)tok.value.constant.value.f,
           (double)expected);
}

static void expect_double_constant(const char* src, double expected) {
    struct mcc_token tok = lex_one(src);
    if (tok.type != MCC_TOKEN_TYPE_CONSTANT) {
        TEST_FAIL("'%s': expected CONSTANT, got token type %d", src, tok.type);
        return;
    }
    EXPECT(tok.value.constant.type == MCC_CONSTANT_TYPE_DOUBLE,
           "'%s': expected MCC_CONSTANT_TYPE_DOUBLE, got %d",
           src,
           tok.value.constant.type);
    EXPECT(tok.value.constant.value.d == expected,
           "'%s': value %f != expected %f",
           src,
           tok.value.constant.value.d,
           expected);
}

static void expect_ldouble_constant(const char* src, long double expected) {
    struct mcc_token tok = lex_one(src);
    if (tok.type != MCC_TOKEN_TYPE_CONSTANT) {
        TEST_FAIL("'%s': expected CONSTANT, got token type %d", src, tok.type);
        return;
    }
    EXPECT(tok.value.constant.type == MCC_CONSTANT_TYPE_LONG_DOUBLE,
           "'%s': expected MCC_CONSTANT_TYPE_LONG_DOUBLE, got %d",
           src,
           tok.value.constant.type);
    EXPECT(tok.value.constant.value.ld == expected,
           "'%s': value %Lf != expected %Lf",
           src,
           tok.value.constant.value.ld,
           expected);
}

static void expect_overflow(const char* src) {
    expect_invalid(src);
}

// =============================================================================
// Tests
// =============================================================================

static void test_keywords(void) {
    TEST_SUITE("Keywords");

    // Storage class specifiers
    expect_keyword("auto", MCC_KEYWORD_AUTO);
    expect_keyword("register", MCC_KEYWORD_REGISTER);
    expect_keyword("static", MCC_KEYWORD_STATIC);
    expect_keyword("extern", MCC_KEYWORD_EXTERN);
    expect_keyword("typedef", MCC_KEYWORD_TYPEDEF);

    // Type specifiers
    expect_keyword("void", MCC_KEYWORD_VOID);
    expect_keyword("char", MCC_KEYWORD_CHAR);
    expect_keyword("short", MCC_KEYWORD_SHORT);
    expect_keyword("int", MCC_KEYWORD_INT);
    expect_keyword("long", MCC_KEYWORD_LONG);
    expect_keyword("float", MCC_KEYWORD_FLOAT);
    expect_keyword("double", MCC_KEYWORD_DOUBLE);
    expect_keyword("signed", MCC_KEYWORD_SIGNED);
    expect_keyword("unsigned", MCC_KEYWORD_UNSIGNED);
    expect_keyword("_Bool", MCC_KEYWORD_BOOL);
    expect_keyword("_Complex", MCC_KEYWORD_COMPLEX);
    expect_keyword("_Imaginary", MCC_KEYWORD_IMAGINARY);

    // Type qualifiers
    expect_keyword("const", MCC_KEYWORD_CONST);
    expect_keyword("restrict", MCC_KEYWORD_RESTRICT);
    expect_keyword("volatile", MCC_KEYWORD_VOLATILE);

    // Function specifiers
    expect_keyword("inline", MCC_KEYWORD_INLINE);

    // Control flow
    expect_keyword("if", MCC_KEYWORD_IF);
    expect_keyword("else", MCC_KEYWORD_ELSE);
    expect_keyword("switch", MCC_KEYWORD_SWITCH);
    expect_keyword("case", MCC_KEYWORD_CASE);
    expect_keyword("default", MCC_KEYWORD_DEFAULT);
    expect_keyword("while", MCC_KEYWORD_WHILE);
    expect_keyword("do", MCC_KEYWORD_DO);
    expect_keyword("for", MCC_KEYWORD_FOR);
    expect_keyword("goto", MCC_KEYWORD_GOTO);
    expect_keyword("continue", MCC_KEYWORD_CONTINUE);
    expect_keyword("break", MCC_KEYWORD_BREAK);
    expect_keyword("return", MCC_KEYWORD_RETURN);

    // Aggregate types
    expect_keyword("struct", MCC_KEYWORD_STRUCT);
    expect_keyword("union", MCC_KEYWORD_UNION);
    expect_keyword("enum", MCC_KEYWORD_ENUM);

    // Other
    expect_keyword("sizeof", MCC_KEYWORD_SIZEOF);

    // Keywords at token boundaries
    expect_keyword("int ", MCC_KEYWORD_INT);
    expect_keyword("return;", MCC_KEYWORD_RETURN);
    expect_keyword("if(", MCC_KEYWORD_IF);
    expect_keyword("struct{", MCC_KEYWORD_STRUCT);
    expect_keyword("_Bool ", MCC_KEYWORD_BOOL);
}

static void test_identifiers(void) {
    TEST_SUITE("Identifiers (not keywords)");

    expect_identifier("integer");   // prefix of "int"
    expect_identifier("returned");  // prefix of "return"
    expect_identifier("iff");       // prefix of "if"
    expect_identifier("whileloop"); // prefix of "while"
    expect_identifier("_int");      // underscore prefix
    expect_identifier("int_");      // underscore suffix
    expect_identifier("Int");       // wrong case
    expect_identifier("INT");       // wrong case
    expect_identifier("my_var");
    expect_identifier("x");
    expect_identifier("foo123");
    expect_identifier("_private");
    expect_identifier("_Bool2");
    expect_identifier("_Complex_");
}

static void test_integer_constants(void) {
    TEST_SUITE("Integer Constants — Decimal");

    expect_int_constant("0", 0);
    expect_int_constant("1", 1);
    expect_int_constant("42", 42);
    expect_int_constant("2147483647", 2147483647);

    expect_uint_constant("10u", 10u);
    expect_uint_constant("10U", 10u);

    expect_long_constant("42l", 42l);
    expect_long_constant("42L", 42l);

    expect_llong_constant("100ll", 100ll);
    expect_llong_constant("100LL", 100ll);

    expect_ulong_constant("50ul", 50ul);
    expect_ulong_constant("50uL", 50ul);
    expect_ulong_constant("50Ul", 50ul);
    expect_ulong_constant("50UL", 50ul);

    expect_ullong_constant("1000ull", 1000ull);
    expect_ullong_constant("1000uLL", 1000ull);
    expect_ullong_constant("1000Ull", 1000ull);
    expect_ullong_constant("1000ULL", 1000ull);

    TEST_SUITE("Integer Constants — Octal");

    expect_int_constant("00", 00);
    expect_int_constant("01", 01);
    expect_int_constant("07", 07);
    expect_int_constant("010", 010);
    expect_int_constant("0755", 0755);

    expect_uint_constant("077u", 077u);
    expect_long_constant("0123l", 0123l);
    expect_llong_constant("0777ll", 0777ll);

    TEST_SUITE("Integer Constants — Hexadecimal");

    expect_int_constant("0x0", 0x0);
    expect_int_constant("0x1", 0x1);
    expect_int_constant("0xA", 0xA);
    expect_int_constant("0xff", 0xff);
    expect_int_constant("0xFF", 0xFF);
    expect_int_constant("0xDEAD", 0xDEAD);
    expect_int_constant("0xBEEF", 0xBEEF);

    expect_uint_constant("0x10u", 0x10u);
    expect_long_constant("0xFFl", 0xFFl);
    expect_llong_constant("0xABCDll", 0xABCDll);
    expect_ulong_constant("0xDEADul", 0xDEADul);
    expect_ullong_constant("0xBEEFull", 0xBEEFull);

    TEST_SUITE("Integer Constants — Zero with suffixes");

    expect_uint_constant("0u", 0u);
    expect_long_constant("0l", 0l);
    expect_llong_constant("0ll", 0ll);
    expect_ulong_constant("0ul", 0ul);
    expect_ullong_constant("0ull", 0ull);

    TEST_SUITE("Integer Constants — Promotion Chain");

    // INT_MAX fits in int
    expect_int_constant("2147483647", 2147483647);
    // INT_MAX + 1 must promote to long (on 64-bit where long is 8 bytes)
    expect_long_constant("2147483648", 2147483648);
    // LONG_MAX fits in long
    expect_long_constant("9223372036854775807", 9223372036854775807l);

    // LONG_MAX + 1 must promote to long long
    // On 64-bit where long == long long this would overflow to MCC_CONSTANT_TYPE_OVERFLOW
    // so this test is platform-dependent — skip or conditionalize

    // Explicit ll suffix bypasses promotion ladder entirely
    expect_llong_constant("2147483648ll", 2147483648ll);

    // Hex/octal can promote to unsigned before going to long (C99 6.4.4.1 table)
    // 0x80000000 doesn't fit in int but fits in unsigned int
    expect_uint_constant("0x80000000", 0x80000000);

    // Octal same rule
    expect_uint_constant("020000000000", 020000000000); // 2^31

    // Hex value that overflows long long but fits in unsigned long long
    // 0xFFFFFFFFFFFFFFFF == ULLONG_MAX, too big for long long but valid ull
    expect_ulong_constant("0xFFFFFFFFFFFFFFFF", 0xFFFFFFFFFFFFFFFFul);

    TEST_SUITE("Integer Constants — Overflow");

    expect_overflow("0xFFFFFFFFFFFFFFFFll");
    expect_overflow("99999999999999999999999999999");    // too big for any signed type
    expect_overflow("99999999999999999999999999999ull"); // too big for ull
    expect_overflow("99999999999999999999999999999u");   // too big for unsigned ladder

    TEST_SUITE("Float Constants — Overflow");

    expect_overflow("1e99999f"); // float overflow
    expect_overflow("1e99999");  // double overflow
    expect_overflow("1e99999l"); // long double overflow

    TEST_SUITE("Integer Constants — Invalid");

    expect_invalid("08");
    expect_invalid("09");
    expect_invalid("089");
    expect_invalid("0888");
    expect_invalid("123xyz");
    expect_invalid("42lll");
    expect_invalid("100uuu");
    expect_invalid("0xFFgg");
    expect_invalid("0x");
    expect_invalid("0X");
}

static void test_float_constants(void) {
    TEST_SUITE("Float Constants — Decimal double");

    expect_double_constant("0.0", 0.0);
    expect_double_constant("1.0", 1.0);
    expect_double_constant("3.14", 3.14);
    expect_double_constant("0.5", 0.5);
    expect_double_constant(".5", .5);
    expect_double_constant(".25", .25);
    expect_double_constant("1.", 1.);
    expect_double_constant("99.", 99.);

    TEST_SUITE("Float Constants — Scientific notation");

    expect_double_constant("1e0", 1e0);
    expect_double_constant("1e10", 1e10);
    expect_double_constant("1e-1", 1e-1);
    expect_double_constant("1e+5", 1e+5);
    expect_double_constant("2.5e3", 2.5e3);
    expect_double_constant("3.14e-2", 3.14e-2);
    expect_double_constant("1E10", 1E10);
    expect_double_constant("2.5E3", 2.5E3);

    TEST_SUITE("Float Constants — Suffixes");

    expect_float_constant("1.0f", 1.0f);
    expect_float_constant("1.0F", 1.0F);
    expect_float_constant("3.14f", 3.14f);
    expect_float_constant("1e10f", 1e10f);
    expect_float_constant("2.5e3f", 2.5e3f);

    expect_ldouble_constant("1.0l", 1.0l);
    expect_ldouble_constant("1.0L", 1.0L);
    expect_ldouble_constant("3.14l", 3.14l);
    expect_ldouble_constant("1e10L", 1e10L);

    TEST_SUITE("Float Constants — Hexadecimal");

    expect_double_constant("0x1p0", 0x1p0);
    expect_double_constant("0x1p1", 0x1p1);
    expect_double_constant("0x1p-1", 0x1p-1);
    expect_double_constant("0x2p2", 0x2p2);
    expect_double_constant("0x1.0p0", 0x1.0p0);
    expect_double_constant("0x1.8p0", 0x1.8p0);
    expect_double_constant("0x1.5p3", 0x1.5p3);

    expect_float_constant("0x1p0f", 0x1p0f);
    expect_float_constant("0x1.5p3f", 0x1.5p3f);
    expect_ldouble_constant("0x1p10l", 0x1p10l);

    TEST_SUITE("Float Constants — Edge Cases");

    expect_double_constant("0.", 0.);     // trailing dot, no digits after
    expect_double_constant(".0", .0);     // leading dot, already have .5 but not .0
    expect_double_constant("0e0", 0e0);   // zero with exponent
    expect_double_constant("1.e1", 1.e1); // dot with exponent, no fractional digits

    TEST_SUITE("Float Constants — Invalid");

    expect_invalid("1.2.3");
    expect_invalid("1e2.5");
    expect_invalid("1e2e3");
    expect_invalid("1e");
    expect_invalid("1e+");
    expect_invalid("1e-");
    expect_invalid("0x1.5"); // hex float missing binary exponent
    expect_invalid("0xA.B");
    expect_invalid("1.5x");
    expect_invalid("1.0ff");
    expect_invalid("2.5ll");
}

static void test_punctuators(void) {
    TEST_SUITE("Punctuators — Single character");

    expect_punctuator("(", MCC_PUNCTUATOR_LEFT_PARENTHESIS);
    expect_punctuator(")", MCC_PUNCTUATOR_RIGHT_PARENTHESIS);
    expect_punctuator("{", MCC_PUNCTUATOR_LEFT_BRACE);
    expect_punctuator("}", MCC_PUNCTUATOR_RIGHT_BRACE);
    expect_punctuator("[", MCC_PUNCTUATOR_LEFT_BRACKET);
    expect_punctuator("]", MCC_PUNCTUATOR_RIGHT_BRACKET);
    expect_punctuator(";", MCC_PUNCTUATOR_SEMICOLON);
    expect_punctuator(",", MCC_PUNCTUATOR_COMMA);
    expect_punctuator(":", MCC_PUNCTUATOR_COLON);
    expect_punctuator("?", MCC_PUNCTUATOR_QUESTION_MARK);
    expect_punctuator("~", MCC_PUNCTUATOR_TILDE);
    expect_punctuator(".", MCC_PUNCTUATOR_DOT);
    expect_punctuator("#", MCC_PUNCTUATOR_HASH);

    TEST_SUITE("Punctuators — Arithmetic");

    expect_punctuator("+", MCC_PUNCTUATOR_PLUS);
    expect_punctuator("++", MCC_PUNCTUATOR_PLUS_PLUS);
    expect_punctuator("+=", MCC_PUNCTUATOR_PLUS_EQUAL);
    expect_punctuator("-", MCC_PUNCTUATOR_MINUS);
    expect_punctuator("--", MCC_PUNCTUATOR_MINUS_MINUS);
    expect_punctuator("-=", MCC_PUNCTUATOR_MINUS_EQUAL);
    expect_punctuator("->", MCC_PUNCTUATOR_ARROW);
    expect_punctuator("*", MCC_PUNCTUATOR_ASTERISK);
    expect_punctuator("*=", MCC_PUNCTUATOR_ASTERISK_EQUAL);
    expect_punctuator("/", MCC_PUNCTUATOR_SLASH);
    expect_punctuator("/=", MCC_PUNCTUATOR_SLASH_EQUAL);
    expect_punctuator("%", MCC_PUNCTUATOR_PERCENT);
    expect_punctuator("%=", MCC_PUNCTUATOR_PERCENT_EQUAL);

    TEST_SUITE("Punctuators — Comparison");

    expect_punctuator("=", MCC_PUNCTUATOR_EQUAL);
    expect_punctuator("==", MCC_PUNCTUATOR_EQUAL_EQUAL);
    expect_punctuator("!", MCC_PUNCTUATOR_BANG);
    expect_punctuator("!=", MCC_PUNCTUATOR_BANG_EQUAL);
    expect_punctuator("<", MCC_PUNCTUATOR_LEFT_CHEVRON);
    expect_punctuator("<=", MCC_PUNCTUATOR_LEFT_CHEVRON_EQUAL);
    expect_punctuator(">", MCC_PUNCTUATOR_RIGHT_CHEVRON);
    expect_punctuator(">=", MCC_PUNCTUATOR_RIGHT_CHEVRON_EQUAL);

    TEST_SUITE("Punctuators — Bitwise and logical");

    expect_punctuator("&", MCC_PUNCTUATOR_AMPERSAND);
    expect_punctuator("&&", MCC_PUNCTUATOR_AMPERSAND_AMPERSAND);
    expect_punctuator("&=", MCC_PUNCTUATOR_AMPERSAND_EQUAL);
    expect_punctuator("|", MCC_PUNCTUATOR_PIPE);
    expect_punctuator("||", MCC_PUNCTUATOR_PIPE_PIPE);
    expect_punctuator("|=", MCC_PUNCTUATOR_PIPE_EQUAL);
    expect_punctuator("^", MCC_PUNCTUATOR_CARET);
    expect_punctuator("^=", MCC_PUNCTUATOR_CARET_EQUAL);

    TEST_SUITE("Punctuators — Shift");

    expect_punctuator("<<", MCC_PUNCTUATOR_DOUBLE_LEFT_CHEVRON);
    expect_punctuator("<<=", MCC_PUNCTUATOR_DOUBLE_LEFT_CHEVRON_EQUAL);
    expect_punctuator(">>", MCC_PUNCTUATOR_DOUBLE_RIGHT_CHEVRON);
    expect_punctuator(">>=", MCC_PUNCTUATOR_DOUBLE_RIGHT_CHEVRON_EQUAL);

    TEST_SUITE("Punctuators — Misc multi-character");

    expect_punctuator("##", MCC_PUNCTUATOR_HASH_HASH);
    expect_punctuator("...", MCC_PUNCTUATOR_ELLIPSIS);

    TEST_SUITE("Punctuators — Maximal munch");

    // Ensure greedy tokenization picks the longest valid token.
    expect_punctuator("++", MCC_PUNCTUATOR_PLUS_PLUS);                   // not + +
    expect_punctuator("--", MCC_PUNCTUATOR_MINUS_MINUS);                 // not - -
    expect_punctuator("->", MCC_PUNCTUATOR_ARROW);                       // not - >
    expect_punctuator("<<=", MCC_PUNCTUATOR_DOUBLE_LEFT_CHEVRON_EQUAL);  // not << =
    expect_punctuator(">>=", MCC_PUNCTUATOR_DOUBLE_RIGHT_CHEVRON_EQUAL); // not >> =
    expect_punctuator("...", MCC_PUNCTUATOR_ELLIPSIS);                   // not . . .

    TEST_SUITE("Punctuators — Sequences (first token only)");

    expect_punctuator("()", MCC_PUNCTUATOR_LEFT_PARENTHESIS);
    expect_punctuator("{}", MCC_PUNCTUATOR_LEFT_BRACE);
    expect_punctuator("[]", MCC_PUNCTUATOR_LEFT_BRACKET);
    expect_punctuator("+-", MCC_PUNCTUATOR_PLUS);
    expect_punctuator("*/", MCC_PUNCTUATOR_ASTERISK);
    expect_punctuator("<>", MCC_PUNCTUATOR_LEFT_CHEVRON);

    TEST_SUITE("Punctuators — Invalid characters");

    expect_invalid("@");
    expect_invalid("$");
    expect_invalid("`");
    expect_invalid("\\");
}

// =============================================================================
// Entry Point
// =============================================================================

int main(void) {
    test_keywords();
    test_identifiers();
    test_integer_constants();
    test_float_constants();
    test_punctuators();

    print_results();
    return g_tests_failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
