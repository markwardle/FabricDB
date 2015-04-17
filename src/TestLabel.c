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

void test_label() {
    Label l;
    uint8_t data[8] = {
        0x00, 0x00, 0x00, 0x09, // text_id is 9
        0x00, 0x00, 0x00, 0x15  // refs is 21
    };

    int status;

    l.id = 0;

    status = Fabric_Label_init(&l, data);
    assert(FABRIC_LABEL_INVALID_ID == status);

    Fabric_Label_set_id(&l, 8);
    assert(8 == Fabric_Label_get_id(&l));

    status = Fabric_Label_init(&l, data);
    assert(FABRIC_OK == status);

    assert(9 == Fabric_Label_get_text_id(&l));
    assert(21 == Fabric_Label_get_refs(&l));

    assert(TRUE == Fabric_Label_has_refs(&l));
    l.refs = 0;
    assert(FALSE == Fabric_Label_has_refs(&l));

    Fabric_Label_add_ref(&l);
    assert(1 == Fabric_Label_get_refs(&l));
    Fabric_Label_remove_ref(&l);
    assert(0 == Fabric_Label_get_refs(&l));

    printf("All unit tests passed for label.\n");

}

#ifndef _FABRIC_TEST_ALL__
int main() {
    test_label();
    return 0;
}
#endif