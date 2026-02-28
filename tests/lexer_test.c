#include <assert.h>
#include <defs.h>
#include <lexer.h>
#include <mcc.h>
#include <private/utils.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct mcc_lexer lexer;

// Helper function for keyword tests
void _keyword_test_case(char* src, enum mcc_keyword expected, bool is_keyword) {
    lexer = (struct mcc_lexer){.source = src, .current = src};

    struct mcc_token token = mcc_lexer_next_token(&lexer);

    if (is_keyword) {
        if (token.type == MCC_TOKEN_TYPE_KEYWORD) {
            printf("'%s' -> keyword %d\n", src, token.value.keyword);
            assert(token.value.keyword == expected && "Keyword mismatch");
        } else {
            printf("'%s' expected keyword, got type %d\n", src, token.type);
            assert(false && "Expected keyword token");
        }
    } else {
        if (token.type == MCC_TOKEN_TYPE_IDENTIFIER) {
            printf("'%.*s' -> identifier (not keyword)\n", (int)token.lexeme.size, token.lexeme.data);
        } else {
            printf("'%s' expected identifier, got type %d\n", src, token.type);
            assert(false && "Expected identifier token");
        }
    }
}
#define KEYWORD_TEST_CASE(src, expected)             _keyword_test_case(src, expected, true)
#define IDENTIFIER_TEST_CASE(src)                    _keyword_test_case(src, 0, false)
#define KEYWORD_TEST_CASE_WITH_SUFFIX(src, expected) _keyword_test_case(src, expected, true)

void _constant_test_case(char* src, struct mcc_constant expected, bool is_constant) {
    mcc_lexer_create(&lexer, src, strlen(src));

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

    mcc_lexer_destroy(&lexer);
}
#define CONSTANT_TEST_CASE(src, type, member) \
    _constant_test_case(#src, (struct mcc_constant){type, {.member = src}}, true)
#define CONSTANT_TEST_CASE_FAIL(src) _constant_test_case(src, (struct mcc_constant){0}, false)

void _punctuator_test_case(const char* src, enum mcc_punctuator expected, bool is_punctuator) {
    mcc_lexer_create(&lexer, src, strlen(src));

    struct mcc_token token = mcc_lexer_next_token(&lexer);

    if (is_punctuator) {
        if (token.type == MCC_TOKEN_TYPE_PUNCTUATOR) {
            printf("'%s' -> punctuator %d\n", src, token.value.punctuator);
            assert(token.value.punctuator == expected && "Punctuator mismatch");
        } else {
            printf("'%s' expected punctuator, got type %d\n", src, token.type);
            assert(false && "Expected punctuator token");
        }
    } else {
        printf("'%s' should be invalid\n", src);
        assert(token.type == MCC_TOKEN_TYPE_INVALID && "Expected invalid token");
    }

    mcc_lexer_destroy(&lexer);
}
#define PUNCTUATOR_TEST_CASE(src, expected) _punctuator_test_case(src, expected, true)
#define PUNCTUATOR_TEST_CASE_FAIL(src)      _punctuator_test_case(src, 0, false)

void test_keyword_lexing() {
    printf("\n=== Keyword Lexing Tests ===\n");

    // Storage class specifiers
    printf("Storage class specifiers:\n");
    KEYWORD_TEST_CASE("auto", MCC_KEYWORD_AUTO);
    KEYWORD_TEST_CASE("register", MCC_KEYWORD_REGISTER);
    KEYWORD_TEST_CASE("static", MCC_KEYWORD_STATIC);
    KEYWORD_TEST_CASE("extern", MCC_KEYWORD_EXTERN);
    KEYWORD_TEST_CASE("typedef", MCC_KEYWORD_TYPEDEF);

    // Type specifiers
    printf("\nType specifiers:\n");
    KEYWORD_TEST_CASE("void", MCC_KEYWORD_VOID);
    KEYWORD_TEST_CASE("char", MCC_KEYWORD_CHAR);
    KEYWORD_TEST_CASE("short", MCC_KEYWORD_SHORT);
    KEYWORD_TEST_CASE("int", MCC_KEYWORD_INT);
    KEYWORD_TEST_CASE("long", MCC_KEYWORD_LONG);
    KEYWORD_TEST_CASE("float", MCC_KEYWORD_FLOAT);
    KEYWORD_TEST_CASE("double", MCC_KEYWORD_DOUBLE);
    KEYWORD_TEST_CASE("signed", MCC_KEYWORD_SIGNED);
    KEYWORD_TEST_CASE("unsigned", MCC_KEYWORD_UNSIGNED);
    KEYWORD_TEST_CASE("_Bool", MCC_KEYWORD_BOOL);
    KEYWORD_TEST_CASE("_Complex", MCC_KEYWORD_COMPLEX);
    KEYWORD_TEST_CASE("_Imaginary", MCC_KEYWORD_IMAGINARY);

    // Type qualifiers
    printf("\nType qualifiers:\n");
    KEYWORD_TEST_CASE("const", MCC_KEYWORD_CONST);
    KEYWORD_TEST_CASE("restrict", MCC_KEYWORD_RESTRICT);
    KEYWORD_TEST_CASE("volatile", MCC_KEYWORD_VOLATILE);

    // Function specifiers
    printf("\nFunction specifiers:\n");
    KEYWORD_TEST_CASE("inline", MCC_KEYWORD_INLINE);

    // Control flow
    printf("\nControl flow:\n");
    KEYWORD_TEST_CASE("if", MCC_KEYWORD_IF);
    KEYWORD_TEST_CASE("else", MCC_KEYWORD_ELSE);
    KEYWORD_TEST_CASE("switch", MCC_KEYWORD_SWITCH);
    KEYWORD_TEST_CASE("case", MCC_KEYWORD_CASE);
    KEYWORD_TEST_CASE("default", MCC_KEYWORD_DEFAULT);
    KEYWORD_TEST_CASE("while", MCC_KEYWORD_WHILE);
    KEYWORD_TEST_CASE("do", MCC_KEYWORD_DO);
    KEYWORD_TEST_CASE("for", MCC_KEYWORD_FOR);
    KEYWORD_TEST_CASE("goto", MCC_KEYWORD_GOTO);
    KEYWORD_TEST_CASE("continue", MCC_KEYWORD_CONTINUE);
    KEYWORD_TEST_CASE("break", MCC_KEYWORD_BREAK);
    KEYWORD_TEST_CASE("return", MCC_KEYWORD_RETURN);

    // Aggregate types
    printf("\nAggregate types:\n");
    KEYWORD_TEST_CASE("struct", MCC_KEYWORD_STRUCT);
    KEYWORD_TEST_CASE("union", MCC_KEYWORD_UNION);
    KEYWORD_TEST_CASE("enum", MCC_KEYWORD_ENUM);

    // Other
    printf("\nOther:\n");
    KEYWORD_TEST_CASE("sizeof", MCC_KEYWORD_SIZEOF);

    printf("\n=== Non-Keyword Identifier Tests ===\n");

    // These should be identifiers, not keywords
    IDENTIFIER_TEST_CASE("integer");   // Not "int"
    IDENTIFIER_TEST_CASE("returned");  // Not "return"
    IDENTIFIER_TEST_CASE("iff");       // Not "if"
    IDENTIFIER_TEST_CASE("whileloop"); // Not "while"
    IDENTIFIER_TEST_CASE("_int");      // Underscore prefix
    IDENTIFIER_TEST_CASE("int_");      // Underscore suffix
    IDENTIFIER_TEST_CASE("Int");       // Wrong case
    IDENTIFIER_TEST_CASE("INT");       // Wrong case
    IDENTIFIER_TEST_CASE("my_var");
    IDENTIFIER_TEST_CASE("x");
    IDENTIFIER_TEST_CASE("foo123");
    IDENTIFIER_TEST_CASE("_private");

    printf("\n=== Edge Case Tests ===\n");

    // Keywords at boundaries (with whitespace/punctuation)
    KEYWORD_TEST_CASE_WITH_SUFFIX("int ", MCC_KEYWORD_INT);
    KEYWORD_TEST_CASE_WITH_SUFFIX("return;", MCC_KEYWORD_RETURN);
    KEYWORD_TEST_CASE_WITH_SUFFIX("if(", MCC_KEYWORD_IF);
    KEYWORD_TEST_CASE_WITH_SUFFIX("struct{", MCC_KEYWORD_STRUCT);
}

void test_integer_lexing() {
    printf("\n=== Integer Constant Tests ===\n");

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

void test_punctuator_lexing() {
    printf("\n=== Punctuator Lexing Tests ===\n");

    // Single character punctuators
    printf("\nSingle character:\n");
    PUNCTUATOR_TEST_CASE("(", MCC_PUNCTUATOR_LEFT_PARENTHESIS);
    PUNCTUATOR_TEST_CASE(")", MCC_PUNCTUATOR_RIGHT_PARENTHESIS);
    PUNCTUATOR_TEST_CASE("{", MCC_PUNCTUATOR_LEFT_BRACE);
    PUNCTUATOR_TEST_CASE("}", MCC_PUNCTUATOR_RIGHT_BRACE);
    PUNCTUATOR_TEST_CASE("[", MCC_PUNCTUATOR_LEFT_BRACKET);
    PUNCTUATOR_TEST_CASE("]", MCC_PUNCTUATOR_RIGHT_BRACKET);
    PUNCTUATOR_TEST_CASE(";", MCC_PUNCTUATOR_SEMICOLON);
    PUNCTUATOR_TEST_CASE(",", MCC_PUNCTUATOR_COMMA);
    PUNCTUATOR_TEST_CASE(":", MCC_PUNCTUATOR_COLON);
    PUNCTUATOR_TEST_CASE("?", MCC_PUNCTUATOR_QUESTION_MARK);
    PUNCTUATOR_TEST_CASE("~", MCC_PUNCTUATOR_TILDE);

    // Single or double character (no equals)
    printf("\nSingle or double (no =):\n");
    PUNCTUATOR_TEST_CASE(".", MCC_PUNCTUATOR_DOT);
    PUNCTUATOR_TEST_CASE("&", MCC_PUNCTUATOR_AMPERSAND);
    PUNCTUATOR_TEST_CASE("&&", MCC_PUNCTUATOR_AMPERSAND_AMPERSAND);
    PUNCTUATOR_TEST_CASE("|", MCC_PUNCTUATOR_PIPE);
    PUNCTUATOR_TEST_CASE("||", MCC_PUNCTUATOR_PIPE_PIPE);
    PUNCTUATOR_TEST_CASE("#", MCC_PUNCTUATOR_HASH);
    PUNCTUATOR_TEST_CASE("##", MCC_PUNCTUATOR_HASH_HASH);

    // Arithmetic operators
    printf("\nArithmetic operators:\n");
    PUNCTUATOR_TEST_CASE("+", MCC_PUNCTUATOR_PLUS);
    PUNCTUATOR_TEST_CASE("++", MCC_PUNCTUATOR_PLUS_PLUS);
    PUNCTUATOR_TEST_CASE("+=", MCC_PUNCTUATOR_PLUS_EQUAL);
    PUNCTUATOR_TEST_CASE("-", MCC_PUNCTUATOR_MINUS);
    PUNCTUATOR_TEST_CASE("--", MCC_PUNCTUATOR_MINUS_MINUS);
    PUNCTUATOR_TEST_CASE("-=", MCC_PUNCTUATOR_MINUS_EQUAL);
    PUNCTUATOR_TEST_CASE("->", MCC_PUNCTUATOR_ARROW);
    PUNCTUATOR_TEST_CASE("*", MCC_PUNCTUATOR_ASTERISK);
    PUNCTUATOR_TEST_CASE("*=", MCC_PUNCTUATOR_ASTERISK_EQUAL);
    PUNCTUATOR_TEST_CASE("/", MCC_PUNCTUATOR_SLASH);
    PUNCTUATOR_TEST_CASE("/=", MCC_PUNCTUATOR_SLASH_EQUAL);
    PUNCTUATOR_TEST_CASE("%", MCC_PUNCTUATOR_PERCENT);
    PUNCTUATOR_TEST_CASE("%=", MCC_PUNCTUATOR_PERCENT_EQUAL);

    // Comparison operators
    printf("\nComparison operators:\n");
    PUNCTUATOR_TEST_CASE("=", MCC_PUNCTUATOR_EQUAL);
    PUNCTUATOR_TEST_CASE("==", MCC_PUNCTUATOR_EQUAL_EQUAL);
    PUNCTUATOR_TEST_CASE("!", MCC_PUNCTUATOR_BANG);
    PUNCTUATOR_TEST_CASE("!=", MCC_PUNCTUATOR_BANG_EQUAL);
    PUNCTUATOR_TEST_CASE("<", MCC_PUNCTUATOR_LEFT_CHEVRON);
    PUNCTUATOR_TEST_CASE("<=", MCC_PUNCTUATOR_LEFT_CHEVRON_EQUAL);
    PUNCTUATOR_TEST_CASE(">", MCC_PUNCTUATOR_RIGHT_CHEVRON);
    PUNCTUATOR_TEST_CASE(">=", MCC_PUNCTUATOR_RIGHT_CHEVRON_EQUAL);

    // Bitwise operators
    printf("\nBitwise operators:\n");
    PUNCTUATOR_TEST_CASE("^", MCC_PUNCTUATOR_CARET);
    PUNCTUATOR_TEST_CASE("^=", MCC_PUNCTUATOR_CARET_EQUAL);
    PUNCTUATOR_TEST_CASE("&=", MCC_PUNCTUATOR_AMPERSAND_EQUAL);
    PUNCTUATOR_TEST_CASE("|=", MCC_PUNCTUATOR_PIPE_EQUAL);

    // Shift operators
    printf("\nShift operators:\n");
    PUNCTUATOR_TEST_CASE("<<", MCC_PUNCTUATOR_DOUBLE_LEFT_CHEVRON);
    PUNCTUATOR_TEST_CASE("<<=", MCC_PUNCTUATOR_DOUBLE_LEFT_CHEVRON_EQUAL);
    PUNCTUATOR_TEST_CASE(">>", MCC_PUNCTUATOR_DOUBLE_RIGHT_CHEVRON);
    PUNCTUATOR_TEST_CASE(">>=", MCC_PUNCTUATOR_DOUBLE_RIGHT_CHEVRON_EQUAL);

    // Special
    printf("\nSpecial:\n");
    PUNCTUATOR_TEST_CASE("...", MCC_PUNCTUATOR_ELLIPSIS);

    printf("\n=== Maximal Munch Tests ===\n");

    // Test that longer sequences are matched correctly
    printf("\nEnsure maximal munch works:\n");
    PUNCTUATOR_TEST_CASE("++", MCC_PUNCTUATOR_PLUS_PLUS);                   // Not + +
    PUNCTUATOR_TEST_CASE("--", MCC_PUNCTUATOR_MINUS_MINUS);                 // Not - -
    PUNCTUATOR_TEST_CASE("->", MCC_PUNCTUATOR_ARROW);                       // Not - >
    PUNCTUATOR_TEST_CASE("<<=", MCC_PUNCTUATOR_DOUBLE_LEFT_CHEVRON_EQUAL);  // Not << =
    PUNCTUATOR_TEST_CASE(">>=", MCC_PUNCTUATOR_DOUBLE_RIGHT_CHEVRON_EQUAL); // Not >> =
    PUNCTUATOR_TEST_CASE("&&", MCC_PUNCTUATOR_AMPERSAND_AMPERSAND);         // Not & &
    PUNCTUATOR_TEST_CASE("||", MCC_PUNCTUATOR_PIPE_PIPE);                   // Not | |
    PUNCTUATOR_TEST_CASE("==", MCC_PUNCTUATOR_EQUAL_EQUAL);                 // Not = =
    PUNCTUATOR_TEST_CASE("!=", MCC_PUNCTUATOR_BANG_EQUAL);                  // Not ! =
    PUNCTUATOR_TEST_CASE("...", MCC_PUNCTUATOR_ELLIPSIS);                   // Not . . .

    printf("\n=== Invalid Punctuator Tests ===\n");

    // Test invalid characters (these should be caught as invalid)
    PUNCTUATOR_TEST_CASE_FAIL("@");
    PUNCTUATOR_TEST_CASE_FAIL("$");
    PUNCTUATOR_TEST_CASE_FAIL("`");
    PUNCTUATOR_TEST_CASE_FAIL("\\");

    printf("\n=== Sequence Tests (Multiple Tokens) ===\n");

    // Test that sequences are properly separated
    // Note: These test the FIRST token only
    printf("\nFirst token of sequences:\n");
    PUNCTUATOR_TEST_CASE("()", MCC_PUNCTUATOR_LEFT_PARENTHESIS);
    PUNCTUATOR_TEST_CASE("{}", MCC_PUNCTUATOR_LEFT_BRACE);
    PUNCTUATOR_TEST_CASE("[]", MCC_PUNCTUATOR_LEFT_BRACKET);
    PUNCTUATOR_TEST_CASE("+-", MCC_PUNCTUATOR_PLUS);
    PUNCTUATOR_TEST_CASE("*/", MCC_PUNCTUATOR_ASTERISK);
    PUNCTUATOR_TEST_CASE("<>", MCC_PUNCTUATOR_LEFT_CHEVRON);
}

int main() {
    test_keyword_lexing();
    test_integer_lexing();
    test_float_lexing();
    test_punctuator_lexing();
    printf("\n=== All Tests Passed! ===\n");
    return 0;
}
