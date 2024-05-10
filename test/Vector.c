#include "Vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned long long size;
    void* mem;
} TestElement;

void TestDestructor(void* vp) {
    if (vp != NULL) {
        TestElement* test_elem = vp;
        free(test_elem->mem);
    }
}

void TestCreate(void) {
    Vector vec;

    assert((vec = VectorCreate(0, sizeof(TestElement), NULL)));
    assert(VectorLength(vec) == 0);
    assert(VectorCapacity(vec) == 0);
    assert(VectorElementSize(vec) == sizeof(TestElement));
    VectorDestroy(vec);

    assert((vec = VectorCreate(0, sizeof(TestElement), TestDestructor)));
    VectorDestroy(vec);

    assert((vec = VectorCreate(10, sizeof(TestElement), TestDestructor)));
    assert(VectorLength(vec) == 0);
    assert(VectorCapacity(vec) >= 10);
    VectorDestroy(vec);

    assert((vec = VectorCreate(10, sizeof(TestElement), NULL)));
    VectorDestroy(vec);
}

void TestReserve(void) {
    Vector vec;

    assert((vec = VectorCreate(4, sizeof(TestElement), NULL)));
    assert(VectorReserve(&vec, 2));
    assert(VectorCapacity(vec) == 2);
    VectorDestroy(vec);

    assert((vec = VectorCreate(0, sizeof(TestElement), NULL)));
    assert(VectorReserve(&vec, 100));
    assert(VectorLength(vec) == 0);
    assert(VectorCapacity(vec) >= 100);
    VectorDestroy(vec);
}

void TestClear(void) {
    Vector vec;

    assert((vec = VectorCreate(0, sizeof(TestElement), TestDestructor)));
    assert(VectorPush(&vec, &(TestElement){.size = 42, .mem = malloc(1000)}));
    VectorClear(&vec);
    assert(VectorLength(vec) == 0);
    assert(VectorPush(&vec, &(TestElement){.size = 1}));
    assert(((TestElement*)vec)[0].size == 1);

    VectorDestroy(vec);
}

void TestPush(void) {
    Vector vec;

    assert((vec = VectorCreate(0, sizeof(TestElement), TestDestructor)));
    assert(VectorPush(&vec, &(TestElement){.size = 100, .mem = malloc(100)}));
    assert(VectorPush(&vec, &(TestElement){.size = 200, .mem = malloc(200)}));
    assert(VectorPush(&vec, &(TestElement){.size = 300, .mem = malloc(300)}));
    assert(((TestElement*)vec)[0].size == 100);
    assert(((TestElement*)vec)[1].size == 200);
    assert(((TestElement*)vec)[2].size == 300);
    VectorDestroy(vec);

    assert((vec = VectorCreate(0, sizeof(TestElement), TestDestructor)));

    for (int i = 0; i < 250; i++) {
        size_t* n = malloc(sizeof(*n));

        *n = i;

        TestElement t = {.size = i, .mem = n};
        assert(VectorPush(&vec, &t));
    }
    assert(VectorLength(vec) == 250);

    for (int i = 0; i < 250; i++) {
        assert(((TestElement*)vec)[i].size == i);
        assert(*((size_t*)((TestElement*)vec)[i].mem) == i);
    }

    VectorDestroy(vec);
}

void TestPop(void) {
    Vector vec;

    assert((vec = VectorCreate(0, sizeof(TestElement), TestDestructor)));
    assert(VectorPush(&vec, &(TestElement){.size = 0, .mem = malloc(0)}));
    assert(VectorPush(&vec, &(TestElement){.size = 1, .mem = malloc(1)}));
    assert(VectorPush(&vec, &(TestElement){.size = 2, .mem = malloc(2)}));
    assert(VectorPush(&vec, &(TestElement){.size = 3, .mem = malloc(3)}));
    assert(VectorPush(&vec, &(TestElement){.size = 4, .mem = malloc(4)}));
    VectorPop(&vec);
    VectorPop(&vec);
    VectorPop(&vec);
    assert(VectorLength(vec) == 2);
    assert(((TestElement*)vec)[0].size == 0);
    assert(((TestElement*)vec)[1].size == 1);

    VectorDestroy(vec);
}

int main(void) {
    TestCreate();
    TestReserve();
    TestClear();
    TestPush();
    TestPop();

    return 0;
}
