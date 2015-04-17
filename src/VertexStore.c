/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 27, 2015
 */

#ifndef _FABRIC_VERTEXSTORE_C__
#define _FABRIC_VERTEXSTORE_C__

#include "Internal.h"

/**
 * The Vertex Store is the component of the graph that has the responsibility
 * of managing the storage of Vertex objects.
 *
 * For many of the functions to work, it is assumed that the Vertex Store
 * is embedded inside a Graph object.
 *
 * For a detailed description of Vertex objects, see the accompanying
 * Vertex.c file.
 */
typedef struct VertexStore {
    uint32_t offset;    // graph file offset for the property store
    uint32_t size;      // the size of the property store
} VertexStore;

/**
 * Initializes a vertex store object
 *
 * Args:
 *      self: The Vertex Store object being initialized.  Its offset should
 *            already be set by the Graph
 */
void Fabric_VertexStore_init(VertexStore *self) {
    Graph *graph = Fabric_VertexStore_get_graph(self);
    self->size = Fabric_Graph_get_edge_store_offset(graph) - self->offset;
}

Vertex *Fabric_VertexStore_get_vertex(VertexStore *self, edgeid_t edge_id, error_t *status) {
    // TODO this is a stub
    *status = FABRIC_OK;
    return NULL;
}

#endif