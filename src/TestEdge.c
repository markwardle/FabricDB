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
#include "Fabric.h"
#endif
#include "Graph.c"

void test_edge() {
    Edge e;
    uint8_t data[24] = {
        0x00, 0x00, 0x00, 0x09, // label_id is 9
        0x00, 0x00, 0x00, 0x17, // from_id is 23
        0x00, 0x00, 0x00, 0x19, // to_id is 25
        0x00, 0x00, 0x00, 0x0A, // next_out_id is 10
        0x00, 0x00, 0x00, 0x0C, // next_in_id is 12
        0x00, 0x00, 0x00, 0x11  // first_property_id is 17
    };

    e.id = 0;
    assert(FABRIC_EDGE_INVALID_ID == Fabric_Edge_init(&e, data));
    Fabric_Edge_set_id(&e, 8);
    assert(8 == Fabric_Edge_get_id(&e));
    assert(FABRIC_OK == Fabric_Edge_init(&e, data));

    assert(9 == Fabric_Edge_get_label_id(&e));
    assert(23 == Fabric_Edge_get_from_vertex_id(&e));
    assert(25 == Fabric_Edge_get_to_vertex_id(&e));
    assert(10 == Fabric_Edge_get_next_out_edge_id(&e));
    assert(12 == Fabric_Edge_get_next_in_edge_id(&e));
    assert(17 == Fabric_Edge_get_first_property_id(&e));
    assert(TRUE == Fabric_Edge_has_properties(&e));
    assert(TRUE == Fabric_Edge_has_next_out_edge(&e));
    assert(TRUE == Fabric_Edge_has_next_in_edge(&e));
    e.next_out_id = 0;
    e.next_in_id = 0;
    e.first_property_id = 0;
    assert(FALSE == Fabric_Edge_has_properties(&e));
    assert(FALSE == Fabric_Edge_has_next_out_edge(&e));
    assert(FALSE == Fabric_Edge_has_next_in_edge(&e));

    printf("All unit tests passed for edge.\n");
}

#ifndef _FABRIC_TEST_ALL__
int main() {
    test_edge();
    return 0;
}
#endif