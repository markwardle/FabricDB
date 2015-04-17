/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 25, 2015
 * Updated: March 26, 2015
 */

#include <stdio.h>
#include <assert.h>
#ifndef _FABRIC_TEST_ALL__
#include "Fabric.h"
#endif
#include "Graph.c"

void test_class() {
    Class cl;
    uint8_t data[29] = {
        0x00, 0x00, 0x00, 0x09, // label_id is 9 big endian
        0x00, 0x01,             // parent_id is 1
        0x00, 0x04,             // first_child_id is 4
        0x00, 0x00,             // next_child_id is 0
        0x00, 0x10,             // first_index_id is 16
        0x00, 0x00, 0x00, 0x23, // count is 35
        0x00,                   // is_abstract is 0
        0x00, 0x00, 0x00, 0x25  // incrementer is 37
    };

    int status;

    cl.id = 0; // Give it an id less than 1 to test for expected failure

    status = Fabric_Class_init(&cl, data);
    assert (status == FABRIC_CLASS_INVALID_ID);

    cl.id = 2;

    status = Fabric_Class_init(&cl, data);
    assert (status == FABRIC_OK);


    assert(Fabric_Class_get_label_id(&cl) == 9);
    assert(Fabric_Class_get_parent_class_id(&cl) == 1);
    assert(Fabric_Class_has_child_classes(&cl) == 1);
    cl.first_child_id = 0;
    assert(Fabric_Class_has_child_classes(&cl) == 0);
    assert(Fabric_Class_has_next_child_class(&cl) == 0);
    cl.next_child_id = 17;
    assert(Fabric_Class_has_next_child_class(&cl) == 1);
    assert(Fabric_Class_get_first_index_id(&cl) == 16);
    assert(Fabric_Class_get_count(&cl) == 35);
    assert(Fabric_Class_is_abstract(&cl) == 0);
    cl.is_abstract = 1;
    assert(Fabric_Class_is_abstract(&cl) == 1);
    assert(Fabric_Class_increment(&cl) == 37);
    assert(Fabric_Class_increment(&cl) == 38);

    printf ("All tests passed for class implementation.\n");

}

#ifndef _FABRIC_TEST_ALL__
int main() {
    test_class();
    return 0;
}
#endif