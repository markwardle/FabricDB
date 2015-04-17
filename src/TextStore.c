/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 27, 2015
 */

#ifndef _FABRIC_TEXTSTORE_C__
#define _FABRIC_TEXTSTORE_C__

#include "Internal.h"

/**
 * The Text Store is the component of the graph that has the responsibility
 * of managing the storage of text.
 *
 * For many of the functions to work, it is assumed that the Text Store
 * is embedded inside a Graph object.
 *
 * For a detailed description of text objects, see the accompanying
 * Text.c file.
 */
typedef struct TextStore {
    uint32_t offset;        // graph file offset for the class store
    uint32_t size;          // the size of the class store
    uint32_t block_size;    // the size of each block of text
} TextStore;

/**
 * Initializes a Text Store object
 *
 * Args:
 *      self: The Property Store object being initialized.  Its offset and
 *            block_size should already be set by the Graph
 */
void Fabric_TextStore_init(TextStore *self) {
    Graph *graph = Fabric_TextStore_get_graph(self);
    self->size = Fabric_Graph_get_index_store_offset(graph) - self->offset;
}

/**
 * Loads a text object from the database
 *
 * Args:
 *      self: A graph's text store
 *      text_id: The id of the text object being retrieved
 *      out: A memory location to store the text object in
 *
 * Returns: FABRIC_OK on success, a different error code on failure
 */
Text *Fabric_TextStore_get_text(TextStore *self, textid_t text_id, error_t *status) {
    // TODO: this is a stub
    *status = FABRIC_OK;
    return NULL;
}

textid_t Fabric_TextStore_create_text(TextStore *self, text_t value, error_t *status) {
    // TODO: this is a stub
    *status = FABRIC_OK;
    return NULL;
}

#endif