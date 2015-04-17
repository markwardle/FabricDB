/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 26, 2015
 * Updated: March 26, 2015
 */

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#ifndef _FABRIC_TEST_ALL__
#include "Fabric.h"
#include "Memory.c"
#endif
#include "DynamicList.c"

typedef struct _TestDummy {
    int i;
} _TestDummy;


void test_dynamic_list() {
    size_t mem_used_start = Fabric_memused();
    int i, status;
    _TestDummy *dummy;

    DynamicList *list = Fabric_DynamicList_allocate();
    assert (list != NULL);

    status = Fabric_DynamicList_init_with_capacity(list, 50);
    assert (status == FABRIC_OK);

    assert (Fabric_DynamicList_capacity(list) == 50);
    assert (Fabric_DynamicList_count(list) == 0);

    for (i = 0; i < 50; i++) {
        dummy = Fabric_memalloc(sizeof(_TestDummy));
        assert(dummy != NULL);
        Fabric_DynamicList_append(list, dummy);
        assert(Fabric_DynamicList_at(list, i) == dummy);
    }

    assert (Fabric_DynamicList_capacity(list) == 50);
    assert (Fabric_DynamicList_count(list) == 50);

    dummy = Fabric_memalloc(sizeof(_TestDummy));
    assert(dummy != NULL);
    Fabric_DynamicList_append(list, dummy);
    assert (Fabric_DynamicList_capacity(list) > 50);
    assert (Fabric_DynamicList_count(list) == 51);
    assert(Fabric_DynamicList_at(list, 50) == dummy);

    Fabric_DynamicList_remove_last(list);
    assert (Fabric_DynamicList_capacity(list) > 50);
    assert (Fabric_DynamicList_count(list) == 50);
    Fabric_memfree(dummy, sizeof(_TestDummy));

    assert(mem_used_start < Fabric_memused());

    Fabric_DynamicList_free_items_and_deinit(list, sizeof(_TestDummy));
    Fabric_DynamicList_deallocate(list);

    assert(mem_used_start == Fabric_memused());

    printf("All tests passed for dynamic list implementation.\n");
}

#ifndef _FABRIC_TEST_ALL__
int main() {
    test_dynamic_list();
    return 0;
}
#endif