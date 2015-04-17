/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 26, 2015
 * Updated: March 26, 2015
 */

#include <stdio.h>
#include <assert.h>
#ifndef _FABRIC_TEST_ALL__
#include "Fabric.c"
#endif

#define NUM_TESTS 509
#define TEST_POINTER_SIZE 6
#define TEST_POINTER_REALLOCATE_SIZE 10

void test_memory() {
    void *ptrs[NUM_TESTS];
    int i;
    void *ptr;


    assert(Fabric_meminit() == FABRIC_OK);
    assert(Fabric_memused() == 0);



    for (i = 0; i < NUM_TESTS; i++) {
        ptrs[i] = Fabric_memalloc(TEST_POINTER_SIZE);
        assert(ptrs[i] != NULL);
    }

    assert(Fabric_memused() == NUM_TESTS * TEST_POINTER_SIZE);

    for (i = 0; i < NUM_TESTS; i++) {
        ptrs[i] = Fabric_memrealloc(ptrs[i], TEST_POINTER_REALLOCATE_SIZE, TEST_POINTER_SIZE);
        assert(ptrs[i] != NULL);
    }

    assert(Fabric_memused() == NUM_TESTS * TEST_POINTER_REALLOCATE_SIZE);

    for (i = 0; i < NUM_TESTS; i++) {
        ptrs[i] = Fabric_memrealloc(ptrs[i], TEST_POINTER_SIZE, TEST_POINTER_REALLOCATE_SIZE);
        assert(ptrs[i] != NULL);
    }

    assert(Fabric_memused() == NUM_TESTS * TEST_POINTER_SIZE);

    for (i = 0; i < NUM_TESTS; i++) {
        Fabric_memfree(ptrs[i], TEST_POINTER_SIZE);
    }

    assert(Fabric_memused() == 0);

    printf("All tests past for memory module.\n");
}

#ifndef _FABRIC_TEST_ALL__
int main() {
    test_memory();
    return 0;
}
#endif