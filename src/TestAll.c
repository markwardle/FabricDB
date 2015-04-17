/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 28, 2015
 * Updated: March 28, 2015
 */

#define _FABRIC_TEST_ALL__

#include "Fabric.h"
#include "TestClass.c"
#include "TestEdge.c"
#include "TestGraph.c"
#include "TestLabel.c"
#include "TestProperty.c"
#include "TestVertex.c"

#include "TestMemory.c"
#include "TestDynamicList.c"
#include "TestIdSet.c"
#include "TestEntityMap.c"


int main() {
    Fabric_meminit();

    test_memory();
    test_dynamic_list();
    test_id_set();
    test_entity_map();

    test_graph();

    test_class();
    test_edge();

    test_label();
    test_property();
    test_vertex();

    printf("All tests passed.\n");
    return 0;
}