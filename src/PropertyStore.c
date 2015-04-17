/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 27, 2015
 */

#ifndef _FABRIC_PROPERTYSTORE_C__
#define _FABRIC_PROPERTYSTORE_C__

#include "Internal.h"

/**
 * The Property Store is the component of the graph that has the responsibility
 * of managing the storage of Property objects.
 *
 * For many of the functions to work, it is assumed that the Property Store
 * is embedded inside a Graph object.
 *
 * For a detailed description of Property objects, see the accompanying
 * Class.c file.
 */
typedef struct PropertyStore {
    uint32_t offset;    // graph file offset for the property store
    uint32_t size;      // the size of the property store
} PropertyStore;

/**
 * Initializes a Property Store object
 *
 * Args:
 *      self: The Property Store object being initialized.  Its offset should
 *            already be set by the Graph
 */
void Fabric_PropertyStore_init(PropertyStore *self) {
    Graph *graph = Fabric_PropertyStore_get_graph(self);
    self->size = Fabric_Graph_get_text_store_offset(graph) - self->offset;
}

Property *Fabric_PropertyStore_get_property(PropertyStore *self, propertyid_t property_id, error_t *status) {
    // TODO: this is a stub
    *status = FABRIC_OK;
    return NULL;
}

#endif