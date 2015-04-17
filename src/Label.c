/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 31, 2015
 */

#ifndef _FABRIC_LABEL_C__
#define _FABRIC_LABEL_C__

#include "Internal.h"

/**
 * A label is used for class names, attribute names, and edge type labels.
 *
 * They are used in order to reduce the total load on the database
 * for text that is often repeated or needs quicker lookup, since
 * they are always indexed.
 *
 * Labels keep track of the number of times they are used.  This allows
 * the database to collect them as well as their text when they are
 * no longer needed.
 *
 * The database stores a label in 8 bytes
 *
 * +----+----+----+----+----+----+----+----+
 * | text_id           | refs              |
 * +----+----+----+----+----+----+----+----+
 */
typedef struct Label {
    labelid_t id;        // The internal id of the label
    textid_t text_id;    // The id of the Text entity of this label
    uint32_t refs;      // The number of references to this label
} Label;

/**
 * Gets a label's id
 */
labelid_t Fabric_Label_get_id(Label *self) {
    return self->id;
}

/**
 * Sets a label's id
 */
void Fabric_Label_set_id(Label *self, labelid_t id) {
    self->id = id;
}

Label* Fabric_Label_new(labelid_t id, error_t *status) {
    Label* new_label = Fabric_memalloc(sizeof(Label));
    id (NULL == new_label) {
        *status = Fabric_memerrno();
    } else {
        new_label->id = id;
    }

    return new_label;
}

void Fabric_Label_destroy(Label *self) {
    Fabric_memfree(self, sizeof(Label));
}

/**
 * Initializes a label from its stored data block
 *
 * The id should already be set for the label
 *
 * Args:
 *      self: The label being initialized
 *      data: An array of eight bytes holding the label's data.
 *
 * Returns: FABRIC_OK on success, other error code on failure
 */
error_t Fabric_Label_init(Label *self, uint8_t *data) {
    // A Label's id must be set externally
    if (self->id < 1) {
        return FABRIC_LABEL_INVALID_ID;
    }
    self->text_id = betoh32(*(uint32_t*)data);
    self->refs = betoh32(*(uint32_t*)(data+4));
    return FABRIC_OK;
}

/**
 * Get's the id of a label's text
 */
textid_t Fabric_Label_get_text_id(Label *self) {
    return self->text_id;
}

/**
 * Set's a label's text id
 */
void Fabric_Label_set_text_id(Label *self, textid_t text_id) {
    self->text_id = text_id;
}

/**
 * Retrieves a label's text object
 *
 * Args:
 *      self: The label object whose text is being retrieved
 *      graph: The graph the label belongs to
 *      out: A memory location where the result will be stored
 *
 * Returns:
 *      The text object for this label
 *      status is set to FABRIC_OK on success, other error code on failure
 */
Text *Fabric_Label_get_text(Label *self, Graph *graph, error_t *status) {
    TextStore *ts = Fabric_Graph_get_text_store(graph);
    return Fabric_TextStore_get_text(ts, self->text_id, status);
}

/**
 * Gets the number of references to this label
 */
uint32_t Fabric_Label_get_refs(Label *self) {
    return self->refs;
}

void Fabric_Label_set_refs(Label *self, uint32_t refs) {
    self->refs = refs;
}

/**
 * Returns whether or not a label has any references to it
 */
bool_t Fabric_Label_has_refs(Label *self) {
    return self->refs > 0;
}

/**
 * Increments the label's refs
 */
void Fabric_Label_add_ref(Label *self) {
    self->refs++;
}

/**
 * Decrements the label's refs
 */
void Fabric_Label_remove_ref(Label *self) {
    self->refs--;
}

#endif