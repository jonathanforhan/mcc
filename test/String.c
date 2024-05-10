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
    assert(StringReserve(&str, 2));
    assert(StringLength(str) == 2);
    assert(StringCapacity(str) == 3);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, "AB") == 0);

    assert(StringReserve(&str, 100));
    assert(StringLength(str) == 2);
    assert(StringCapacity(str) == 101);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, "AB") == 0);

    assert(StringReserve(&str, 2));
    assert(StringLength(str) == 2);
    assert(StringCapacity(str) == 3);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, "AB") == 0);

    StringDestroy(str);
}

void TestClear(void) {
    String str;

    assert((str = StringCreate(cstr, sizeof(cstr) - 1)));
    StringClear(&str);
    assert(StringLength(str) == 0);
    assert(strcmp(str, "") == 0);
    assert(StringAppend(&str, "Hi"));
    assert(StringLength(str) == 2);
    assert(strcmp(str, "Hi") == 0);

    StringDestroy(str);
}

void TestPush(void) {
    String str;

    assert((str = StringCreate(cstr, 1)));
    assert(StringPush(&str, 'i'));
    assert(StringPush(&str, '!'));
    assert(StringPush(&str, '\n'));
    assert(StringLength(str) == 4);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, "Hi!\n") == 0);

    StringDestroy(str);
}

void TestPop(void) {
    String str;

    assert((str = StringCreate("123456", 6)));
    StringPop(&str);
    StringPop(&str);
    StringPop(&str);
    assert(StringLength(str) == 3);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, "123") == 0);

    StringDestroy(str);
}

void TestAppend(void) {
    String str;

    assert((str = StringCreate("Hello, ", 7)));
    assert(StringAppend(&str, "world!\n"));
    assert(StringLength(str) == sizeof(cstr) - 1);
    assert(str[StringLength(str)] == '\0');
    assert(strcmp(str, cstr) == 0);

    StringDestroy(str);
}

void TestCopy(void) {
    String str, str_cpy;

    assert((str = StringCreate(cstr, sizeof(cstr) - 1)));
    assert(StringCopy(str, &str_cpy));
    assert(StringLength(str) == StringLength(str_cpy));
    assert(str_cpy[StringLength(str_cpy)] == '\0');
    assert(strcmp(str, str_cpy) == 0);

    StringDestroy(str_cpy);
    StringDestroy(str);
}

void TestTrim(void) {
    String str;

    assert((str = StringCreate(cstr, sizeof(cstr) - 1)));
    size_t capacity = StringCapacity(str);
    StringTrim(&str, sizeof(", world!\n") - 1);
    assert(StringLength(str) == sizeof("Hello") - 1);
    assert(str[StringLength(str)] == '\0');
    assert(StringCapacity(str) == capacity);
    assert(strcmp(str, "Hello") == 0);

    StringDestroy(str);
}

void TestShrink(void) {
    String str;

    assert((str = StringCreate(cstr, sizeof(cstr) - 1)));
    StringShrink(&str, sizeof(", world!\n") - 1);
    assert(StringLength(str) == sizeof("Hello") - 1);
    assert(str[StringLength(str)] == '\0');
    assert(StringCapacity(str) == sizeof("Hello"));
    assert(strncmp(str, "Hello", 6) == 0);

    StringDestroy(str);
}

void TestContains(void) {
    String str;

    assert((str = StringCreate(cstr, sizeof(cstr) - 1)));
    assert(!StringContains(str, "hello", 5, true));
    assert(StringContains(str, "hello", 5, false));
    assert(StringContains(str, "world!", 6, true));

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
    TestContains();

    return 0;
}
