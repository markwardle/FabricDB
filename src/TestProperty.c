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

void test_property() {
    Property p;
    uint8_t data[17] = {
        0x00, 0x00, 0x00, 0x12,  // label_id is 18
        0x00, 0x00, 0x00, 0x0A,  // next_property_id is 10
        0x01,                    // type is integer
        0x20, 0xBD, 0x93, 0xD4,
        0x9F, 0xCC, 0x41, 0x92   // value is 2359204321235321234
    };

    p.id = 0;
    assert(FABRIC_PROPERTY_INVALID_ID == Fabric_Property_init(&p, data));

    Fabric_Property_set_id(&p, 16);
    assert(16 == Fabric_Property_get_id(&p));
    assert(FABRIC_OK == Fabric_Property_init(&p, data));

    assert(18 == Fabric_Property_get_label_id(&p));
    assert(10 == Fabric_Property_get_next_property_id(&p));
    assert(FABRIC_PROPTYPE_INTEGER == Fabric_Property_get_type(&p));
    assert(2359204321235321234 == Fabric_Property_get_integer_value(&p));

    assert(TRUE == Fabric_Property_has_next_property(&p));
    p.next_property_id = 0;
    assert(FALSE == Fabric_Property_has_next_property(&p));

    Fabric_Property_set_integer_value(&p, -5764);
    assert(-5764 == Fabric_Property_get_integer_value(&p));

    assert(FALSE == Fabric_Property_is_text(&p));
    assert(FALSE == Fabric_Property_is_short_text(&p));

    Fabric_Property_set_type(&p, FABRIC_PROPTYPE_REAL);
    assert(FABRIC_PROPTYPE_REAL == Fabric_Property_get_type(&p));
    Fabric_Property_set_real_value(&p, 3.14);
    assert(3.14 == Fabric_Property_get_real_value(&p));

    assert(FALSE == Fabric_Property_is_text(&p));
    assert(FALSE == Fabric_Property_is_short_text(&p));

    Fabric_Property_set_type(&p, FABRIC_PROPTYPE_EMPTYTEXT);
    assert(TRUE == Fabric_Property_is_text(&p));
    assert(TRUE == Fabric_Property_is_short_text(&p));
    assert(0 == Fabric_Property_get_short_text_length(&p));

    Fabric_Property_set_type(&p, FABRIC_PROPTYPE_TEXT3);
    assert(TRUE == Fabric_Property_is_text(&p));
    assert(TRUE == Fabric_Property_is_short_text(&p));
    assert(3 == Fabric_Property_get_short_text_length(&p));

    Fabric_Property_set_short_text(&p, "ABC");

    char out_string[4];
    Fabric_Property_get_short_text(&p, out_string);

    assert(strcmp(out_string, "ABC") == 0);

    Fabric_Property_set_type(&p, FABRIC_PROPTYPE_LONGTEXT);
    Fabric_Property_set_text_value_id(&p, 134);

    assert(TRUE == Fabric_Property_is_text(&p));
    assert(FALSE == Fabric_Property_is_short_text(&p));

    assert(134 == Fabric_Property_get_text_value_id(&p));

    assert(FALSE == Fabric_Property_is_boolean(&p));

    Fabric_Property_set_type(&p, FABRIC_PROPTYPE_TRUE);
    assert(TRUE == Fabric_Property_is_boolean(&p));
    assert(TRUE == Fabric_Property_get_boolean_value(&p));

    Fabric_Property_set_type(&p, FABRIC_PROPTYPE_FALSE);
    assert(TRUE == Fabric_Property_is_boolean(&p));
    assert(FALSE == Fabric_Property_get_boolean_value(&p));

    printf("All unit tests passed for property.\n");

}

#ifndef _FABRIC_TEST_ALL__
int main() {
    test_property();
    return 0;
}
#endif