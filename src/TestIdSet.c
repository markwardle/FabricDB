/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 28, 2015
 * Updated: March 29, 2015
 */

#include <stdio.h>
#include <assert.h>
#ifndef _FABRIC_TEST_ALL__
#include "Fabric.c"
#endif


void test_id_set() {
    IdSet *set;
    error_t status;
    uint32_t id, i;
    int l;
    uint32_t *ids;
#ifndef _FABRIC_TEST_ALL__
    Fabric_meminit();
#endif
    size_t starting_memory = Fabric_memused();

    set = Fabric_IdSet_new(&status);

    assert (FABRIC_OK == status);
    assert(set != NULL);
    assert (0 == Fabric_IdSet_get_count(set));
    assert (FABRIC_IDSET_DEFAULT_CAPACITY == Fabric_IdSet_get_capacity(set));

    for (id = 3; id <= 150; id += 3) {
        assert(FALSE == Fabric_IdSet_has(set, id));
        status = Fabric_IdSet_add(set, id);
        assert(FABRIC_OK == status);
        assert(TRUE == Fabric_IdSet_has(set, id));
    }

    assert(50 == Fabric_IdSet_get_count(set));
    assert(50 < Fabric_IdSet_get_capacity(set));

    for (id = 1; id <= 150; id++) {
        if (id % 3 == 0) {
            assert(TRUE == Fabric_IdSet_has(set, id));
        } else {
            assert(FALSE == Fabric_IdSet_has(set, id));
        }
        if (id % 5 == 0) {
            Fabric_IdSet_remove(set, id);
            assert(FALSE == Fabric_IdSet_has(set, id));
        }

        assert(Fabric_IdSet_get_count(set) == 50 - id / 15);
    }

    l = Fabric_IdSet_get_count(set);
    assert(starting_memory != Fabric_memused());

    for (id = 4; id <= 150; id += 4) {
        Fabric_IdSet_add(set, id);
        assert(Fabric_IdSet_get_count(set) == l + id / 4 - id / 12 + id / 60);
    }

    l = Fabric_IdSet_get_count(set);
    ids = Fabric_IdSet_to_array(set, &status);
    assert(FABRIC_OK == status);



    for (i = 0; i < l; i++) {
        id = ids[i];
        assert (id % 3 == 0 || id % 4 == 0);
        assert (id % 5 != 0 || id % 4 == 0);
        assert (id != 0);
        assert (id != FABRIC_IDSET_TOMBSTONE);
        assert (id <= 150);
        assert (TRUE == Fabric_IdSet_has(set, id));
        Fabric_IdSet_remove(set, id);
        assert (FALSE == Fabric_IdSet_has(set, id));
    }

    assert (Fabric_IdSet_get_count(set) == 0);

    Fabric_memfree(ids, l * sizeof(uint32_t));

    Fabric_IdSet_destroy(set);
    assert(starting_memory == Fabric_memused());

    printf("All unit tests passed for id set.\n");

}

#ifndef _FABRIC_TEST_ALL__
int main() {
    test_id_set();
    return 0;
}
#endif