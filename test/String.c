#include "String.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static const char cstr[] = "Hello, world!\n";

void TestCreate(void) {
    String str;

    assert((str = StringCreate(NULL, 0)));
    assert(StringLength(str) == 0);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, "") == 0);
    StringDestroy(str);

    assert((str = StringCreate(cstr, sizeof(cstr) - 1)));
    assert(StringLength(str) == sizeof(cstr) - 1);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, cstr) == 0);
    StringDestroy(str);

    assert((str = StringCreate(cstr, 2)));
    assert(StringLength(str) == 2);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, "He") == 0);
    StringDestroy(str);

    assert((str = StringCreate(cstr, 100)));
    assert(StringLength(str) == sizeof(cstr) - 1);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, cstr) == 0);
    StringDestroy(str);
}

void TestReserve(void) {
    String str;

    assert((str = StringCreate("ABC", 3)));
    assert((str = StringReserve(str, 2)));
    assert(StringLength(str) == 2);
    assert(StringCapacity(str) == 3);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, "AB") == 0);

    assert((str = StringReserve(str, 100)));
    assert(StringLength(str) == 2);
    assert(StringCapacity(str) == 101);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, "AB") == 0);

    assert((str = StringReserve(str, 2)));
    assert(StringLength(str) == 2);
    assert(StringCapacity(str) == 3);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, "AB") == 0);

    StringDestroy(str);
}

void TestClear(void) {
    String str;

    assert((str = StringCreate(cstr, sizeof(cstr) - 1)));
    assert((str = StringClear(str)));
    assert(StringLength(str) == 0);
    assert(strcmp(str, "") == 0);
    assert((str = StringAppend(str, "Hi")));
    assert(StringLength(str) == 2);
    assert(strcmp(str, "Hi") == 0);

    StringDestroy(str);
}

void TestPush(void) {
    String str;

    assert((str = StringCreate(cstr, 1)));
    assert((str = StringPush(str, 'i')));
    assert((str = StringPush(str, '!')));
    assert((str = StringPush(str, '\n')));
    assert(StringLength(str) == 4);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, "Hi!\n") == 0);

    StringDestroy(str);
}

void TestPop(void) {
    String str;

    assert((str = StringCreate("123456", 6)));
    assert((str = StringPop(str)));
    assert((str = StringPop(str)));
    assert((str = StringPop(str)));
    assert(StringLength(str) == 3);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, "123") == 0);

    StringDestroy(str);
}

void TestAppend(void) {
    String str;

    assert((str = StringCreate("Hello, ", 7)));
    assert((str = StringAppend(str, "world!\n")));
    assert(StringLength(str) == sizeof(cstr) - 1);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, cstr) == 0);

    StringDestroy(str);
}

void TestCopy(void) {
    String str, str_cpy;

    assert((str = StringCreate(cstr, sizeof(cstr) - 1)));
    assert((str_cpy = StringCopy(str)));
    assert(StringLength(str) == StringLength(str_cpy));
    assert(str_cpy[StringLength(str_cpy)] == '\0');
    assert(strcmp(str, str_cpy) == 0);

    StringDestroy(str_cpy);
    StringDestroy(str);
}

void TestTrim(void) {
    String str;

    assert((str = StringCreate(cstr, sizeof(cstr) - 1)));
    uint32_t capacity = StringCapacity(str);
    assert((str = StringTrim(str, sizeof(", world!\n") - 1)));
    assert(StringLength(str) == sizeof("Hello") - 1);
    assert(str[StringLength(str)] == '\0');
    assert(StringCapacity(str) == capacity);
    assert(strcmp(str, "Hello") == 0);

    StringDestroy(str);
}

void TestShrink(void) {
    String str;

    assert((str = StringCreate(cstr, sizeof(cstr) - 1)));
    assert((str = StringShrink(str, sizeof(", world!\n") - 1)));
    assert(StringLength(str) == sizeof("Hello") - 1);
    assert(str[StringLength(str)] == '\0');
    assert(StringCapacity(str) == sizeof("Hello"));
    assert(strncmp(str, "Hello", 6) == 0);

    StringDestroy(str);
}

int main(void) {
    TestCreate();
    TestReserve();
    TestClear();
    TestPush();
    TestPop();
    TestAppend();
    TestCopy();
    TestTrim();
    TestShrink();

    return 0;
}
