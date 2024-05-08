#include "Vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned long long size;
    void* mem;
} TestElement;

void TestDestructor(void* vp) {
    TestElement* test_elem = vp;
    free(test_elem->mem);
}

void TestCreate(void) {
    Vector vector;

    assert((vector = VectorCreate(0, sizeof(TestElement), NULL)));
    assert(VectorLength(vector) == 0);
    assert(VectorCapacity(vector) == 0);
    VectorDestroy(vector);

    assert((vector = VectorCreate(0, sizeof(TestElement), TestDestructor)));
    VectorDestroy(vector);

    assert((vector = VectorCreate(10, sizeof(TestElement), TestDestructor)));
    assert(VectorLength(vector) == 0);
    assert(VectorCapacity(vector) >= 10);
    VectorDestroy(vector);

    assert((vector = VectorCreate(10, sizeof(TestElement), NULL)));
    VectorDestroy(vector);
}

void TestReserve(void) {
    Vector vector;

    assert((vector = VectorCreate(4, sizeof(TestElement), NULL)));
    assert((vector = VectorReserve(vector, 2)));
    assert(VectorCapacity(vector) == 2);
    VectorDestroy(vector);

    assert((vector = VectorCreate(0, sizeof(TestElement), NULL)));
    assert((vector = VectorReserve(vector, 100)));
    assert(VectorLength(vector) == 0);
    VectorDestroy(vector);
}

void TestPush(void) {
    Vector vector;

    assert((vector = VectorCreate(0, sizeof(TestElement), TestDestructor)));
    assert((vector = VectorPush(vector, &(TestElement){.size = 100, .mem = malloc(100)})));
    assert((vector = VectorPush(vector, &(TestElement){.size = 200, .mem = malloc(200)})));
    assert((vector = VectorPush(vector, &(TestElement){.size = 300, .mem = malloc(300)})));
    printf("%llu\n", ((TestElement*)vector)[0].size);
    printf("%llu\n", ((TestElement*)vector)[1].size);
    printf("%llu\n", ((TestElement*)vector)[2].size);
    assert(((TestElement*)vector)[0].size == 100);
    assert(((TestElement*)vector)[1].size == 200);
    assert(((TestElement*)vector)[2].size == 300);
    VectorDestroy(vector);

    assert((vector = VectorCreate(0, sizeof(TestElement), TestDestructor)));

    for (int i = 0; i < 250; i++) {
        uint32_t* n = malloc(sizeof(*n));

        *n = i;

        TestElement t = {.size = i, .mem = n};
        assert((vector = VectorPush(vector, &t)));
    }
    assert(VectorLength(vector) == 250);

    for (int i = 0; i < 250; i++) {
        assert(((TestElement*)vector)[i].size == i);
        assert(*((uint32_t*)((TestElement*)vector)[i].mem) == i);
    }

    VectorDestroy(vector);
}

void TestPop(void) {}

void TestLength(void) {}

void TestCapacity(void) {}

void TestElementSize(void) {}

int main(void) {
    TestCreate();
    TestReserve();
    TestPush();
    // TestPop();
    // TestLength();
    // TestCapacity();
    // TestElementSize();

    return 0;
}
