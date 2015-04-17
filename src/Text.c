/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 26, 2015
 * Updated: March 27, 2015
 */

#ifndef _FABRIC_TEXT_C__
#define _FABRIC_TEXT_C__

#include "Internal.h"

/**
 * A text object is used to store almost all textual
 * values.  Saving text separately allows other entities
 * to be a fixed size allowing for constant time lookup
 * by id.
 *
 * Each text object has a stored header which consists of
 * a 4 byte size.  A text's data is stored in FABRIC_TEXT_BLOCK_SIZE
 * units.  What this means is that a text's data will use
 * at least that much space to store a value and will will
 * add enough of this size of data in order to have enough
 * space for the text.  The header is included in this, so
 * a text object will fit in one FABRIC_TEXT_BLOCK_SIZE space
 * if its data is less than or equal to this value - the size
 * of the header (4 bytes).  A text object's internal id is
 * its location from the beginning in this size blocks with 1
 * as the first id.
 *
 * Thus a text objects location within the text store can be
 * found with the formula
 *
 *      FABRIC_TEXT_BLOCK_SIZE * (id - 1)
 *
 * And the total number of blocks the text object will take
 * is determined with the formula
 *
 *      (size + sizeof(uint32_t)) / FABRIC_TEXT_BLOCK_SIZE + 1
 *
 * A null terminator is not saved in the database, but it is
 * added when the value is retrieved.
 *
 * A text objects value is loaded lazily, since there are situations
 * where knowing the size of the text is all that is needed.
 */
typedef struct Text {
    textid_t id;        // The id of the text
    uint32_t size;     // The size of the data in bytes
    text_t value;       // The value of this text node
} Text;

/**
 * Gets the id of a text object
 */
textid_t Fabric_Text_get_id(Text *self) {
    return self->id;
}

/**
 * Sets the id of a text object
 */
void Fabric_Text_set_id(Text *self, textid_t id) {
    self->id = id;
}

/**
 * Initializes a text object from it's stored data
 *
 * This does not initialize the text objects text value.
 * the text value must be set separately.  The reason
 * for this is that it may be expensive in terms of
 * memory to load the text's value, and there are
 * situations in which it is sufficient to only load
 * the size.
 *
 * The text objects id should be set before this method
 * is used.
 *
 * Args:
 *      self: The text object being initialized
 *      data: The text object's header (4 bytes)
 *
 * Returns: FABRIC_OK on success, other error number of failure
 */
error_t Fabric_Text_init(Text *self, uint8_t *data) {
    if (self->id < 1) {
        return FABRIC_TEXT_INVALID_ID;
    }
    self->size = betoh32(*(uint32_t*)data);
    // The value isn't loaded at initialization time
    // It must be set externally
    self->value = NULL;
    return FABRIC_OK;
}

/**
 * Gets the size of a text objects data in bytes.
 */
uint32_t Fabric_Text_get_size(Text *self) {
    return self->size;
}

/**
 * Sets a text objects data size.
 */
void Fabric_Text_set_size(Text *self, uint32_t size) {
    self->size = size;
}

/**
 * Get's the text objects value
 *
 * May return null if the text object's value has not been initialized
 */
text_t Fabric_Text_get_value(Text *self) {
    return self->value;
}

/**
 * Sets the text objects value
 */
void Fabric_Text_set_value(Text *self, text_t value) {
    self->value = value;
}

#endif