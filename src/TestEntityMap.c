/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 29, 2015
 * Updated: March 29, 2015
 */

#include <stdio.h>
#include <assert.h>
#ifndef _FABRIC_TEST_ALL__
#include "Fabric.c"
#endif

typedef struct EMapDummy {
    uint32_t id;
    uint32_t value;
} EMapDummy;

EMapDummy **generate_emap_dummies(int value_change) {

    EMapDummy **dummies = Fabric_memalloc(50 * sizeof(EMapDummy*));
    assert (dummies != NULL);
    EMapDummy *dummy;
    int i;
    for (i = 0; i < 50; i++) {
        dummy = Fabric_memalloc(sizeof(EMapDummy));
        assert(dummy != NULL);
        dummy->id = (i + 1) * 3;
        dummy->value = dummy->id + value_change;
        dummies[i] = dummy;
    }

    return dummies;
}

void free_emap_dummies(EMapDummy **dummies) {
    int i;
    for (i = 0; i < 50; i++) {
        Fabric_memfree(dummies[i], sizeof(EMapDummy));
    }

    Fabric_memfree(dummies, 50 * sizeof(EMapDummy*));
}

void test_entity_map() {
#ifndef _FABRIC_TEST_ALL__
    Fabric_meminit();
#endif
    error_t status;

    int i, id;

    EMapDummy **dummies = generate_emap_dummies(0);
    EMapDummy **changed_dummies = generate_emap_dummies(1);
    EMapDummy *dummy;

    size_t mem_used_start = Fabric_memused();

    EntityMap *map = Fabric_EntityMap_new(&status);
    assert(map != NULL);
    assert(status == FABRIC_OK);
    assert(mem_used_start < Fabric_memused());

    assert(Fabric_EntityMap_get_count(map) == 0);
    assert(Fabric_EntityMap_get_capacity(map) == FABRIC_ENTITYMAP_DEFAULT_CAPACITY);

    for (i = 0; i < 50; i++) {
        dummy = dummies[i];
        assert(FALSE == Fabric_EntityMap_has_key(map, dummy->id));
        status = Fabric_EntityMap_set(map, dummy->id, dummy);
        assert(status == FABRIC_OK);
        assert(TRUE == Fabric_EntityMap_has_key(map, dummy->id));
        assert(Fabric_EntityMap_get_count(map) == i + 1);
        assert(Fabric_EntityMap_get(map, dummy->id) == dummy);
    }


    assert(Fabric_EntityMap_get_count(map) == 50);
    assert(Fabric_EntityMap_get_capacity(map) > 50);

    for (id = 4; i <= 150; i += 4) {
        if (id % 3 == 0) {
            int start_count = Fabric_EntityMap_get_count(map);
            assert(TRUE == Fabric_EntityMap_has_key(map, id));
            assert(dummies[id / 3 - 1] == Fabric_EntityMap_get(map, id));
            Fabric_EntityMap_unset(map, id);
            assert(FALSE == Fabric_EntityMap_has_key(map, id));
            assert(NULL == Fabric_EntityMap_get(map, id));
            assert(start_count - Fabric_EntityMap_get_count(map) == 1 );
            assert(Fabric_EntityMap_get_count(map) == 50 - id / 12);
        } else {
            assert(FALSE == Fabric_EntityMap_has_key(map, id));
            assert(NULL == Fabric_EntityMap_get(map, id));
        }
    }

    for (i = 0; i < 50; i++) {
        dummy = changed_dummies[i];
        if (Fabric_EntityMap_has_key(map, dummy->id)) {
            assert(((EMapDummy*)Fabric_EntityMap_get(map, dummy->id))->value == dummy->id);
        }
        status = Fabric_EntityMap_set(map, dummy->id, dummy);
        assert(((EMapDummy*)Fabric_EntityMap_get(map, dummy->id))->value == dummy->id + 1);
    }

    Fabric_EntityMap_destroy(map);
    assert(mem_used_start == Fabric_memused());

    free_emap_dummies(dummies);
    free_emap_dummies(changed_dummies);

    printf("All tests passed for entity map.\n");

}

#ifndef _FABRIC_TEST_ALL__
int main() {
    test_entity_map();
    return 0;
}
#endif