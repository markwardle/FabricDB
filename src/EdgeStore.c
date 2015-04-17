/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 27, 2015
 */

#ifndef _FABRIC_EDGESTORE_C__
#define _FABRIC_EDGESTORE_C__

#include "Internal.h"

/**
 * The Edge Store is the component of the graph that has the responsibility
 * of managing the storage of Edge objects.
 *
 * For many of the functions to work, it is assumed that the Edge Store
 * is embedded inside a Graph object.
 *
 * For a detailed description of Edge objects, see the accompanying
 * Edge.c file.
 */
typedef struct EdgeStore {
    uint32_t offset;    // graph file offset for the class store
    uint32_t size;      // the size of the class store
} EdgeStore;

/**
 * Initializes an Edge Store object
 *
 * Args:
 *      self: The Edge Store object being initialized.  Its offset should
 *            already be set by the Graph
 */
void Fabric_EdgeStore_init(EdgeStore *self) {
    Graph *graph = Fabric_EdgeStore_get_graph(self);
    self->size = Fabric_Graph_get_property_store_offset(graph) - self->offset;
}

Edge *Fabric_EdgeStore_get_edge(EdgeStore *self, edgeid_t edge_id, error_t *status) {
    // TODO this is a stub
    *status = FABRIC_OK;
    return NULL;
}

#endif