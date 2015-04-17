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
#include "Graph.c"
#endif

void test_vertex() {
    Vertex v;
    uint8_t data[14] = {
        0x00, 0x02,             // class_id is 2
        0x00, 0x00, 0x00, 0x03, // first_out_id is 3
        0x00, 0x00, 0x00, 0x04, // first_in_id is 4
        0x00, 0x00, 0x00, 0x11 // first_property_id is 17
    };

    int status;

    v.id = 0;

    status = Fabric_Vertex_init(&v, data);
    assert (FABRIC_VERTEX_INVALID_ID == status);

    assert(0 == Fabric_Vertex_get_id(&v));
    Fabric_Vertex_set_id(&v, 5);
    assert(5 == Fabric_Vertex_get_id(&v));

    status = Fabric_Vertex_init(&v, data);
    assert (FABRIC_OK == status);

    assert (2 == Fabric_Vertex_get_class_id(&v));
    assert(3 == Fabric_Vertex_get_first_out_edge_id(&v));
    assert(TRUE == Fabric_Vertex_has_out_edges(&v));
    v.first_out_id = 0;
    assert(FALSE == Fabric_Vertex_has_out_edges(&v));
    assert(4 == Fabric_Vertex_get_first_in_edge_id(&v));
    assert(TRUE == Fabric_Vertex_has_in_edges(&v));
    v.first_in_id = 0;
    assert(FALSE == Fabric_Vertex_has_in_edges(&v));
    assert(17 == Fabric_Vertex_get_first_property_id(&v));
    assert(TRUE == Fabric_Vertex_has_properties(&v));
    v.first_property_id = 0;
    assert(FALSE == Fabric_Vertex_has_properties(&v));

    printf("All unit tests passed for vertex.\n");
}

#ifndef _FABRIC_TEST_ALL__
int main() {
    test_vertex();
    return 0;
}
#endif