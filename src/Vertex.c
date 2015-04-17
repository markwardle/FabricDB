/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 24, 2015
 * Updated: March 27, 2015
 */

#ifndef _FABRIC_VERTEX_C__
#define _FABRIC_VERTEX_C__

#include "Internal.h"

/**
 * Vertices are the primary entity of a graph.
 *
 * A vertex must have a class and may have any number of out vertices, in
 * vertices, and properties.  Vertices are the connections a vertex has
 * with other vertices.  Properties are data associated with the vertex in
 * the form of key-value pairs.
 *
 * A vertex's class is used as a means of organization within the graph.
 * A vertex is considered to be a member of its immediate class as well as
 * of all that class's ancestor classes.
 *
 * The database stores a vertex object with 14 bytes
 *
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 * |class_id | first_out_id      | first_in_id       | first_property_id |
 * +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
 */
typedef struct Vertex {
    vertexid_t id;                  // The internal id of this vertex
    classid_t class_id;             // The id of the class this vertex belongs to
    edgeid_t first_out_id;          // The id of the first out edge for this vertex
    edgeid_t first_in_id;           // The id of the first in edge for this vertex
    propertyid_t first_property_id; // the id of the first property for this vertex
} Vertex;

/**
 * Gets a vertex's id
 */
vertexid_t Fabric_Vertex_get_id(Vertex *self) {
    return self->id;
}

/**
 * Sets a vertex's id
 */
void Fabric_Vertex_set_id(Vertex *self, vertexid_t id) {
    self->id = id;
}

/**
 * Initializes a vertex object from its database data
 *
 * The vertex's id should be set before it is initialized
 *
 * Args:
 *      self: The vertex being initialized
 *      data: An array of bytes that represent this vertex in the database (14 bytes)
 *
 * Returns: FABRIC_OK on success, other error code on failure
 */
error_t Fabric_Vertex_init(Vertex *self, uint8_t *data) {
    // A Vertex's id must be set externally
    if (self->id < 1) {
        return FABRIC_VERTEX_INVALID_ID;
    }
    self->class_id = betoh16(*(classid_t*)data);
    self->first_out_id = betoh32(*(edgeid_t*)(data+2));
    self->first_in_id = betoh32(*(edgeid_t*)(data+6));
    self->first_property_id = betoh32(*(propertyid_t*)(data+10));

    return FABRIC_OK;
}

/**
 * Gets the id of a vertex's class
 */
classid_t Fabric_Vertex_get_class_id(Vertex *self) {
    return self->class_id;
}

/**
 * Gets the vertex's class object
 *
 * Args:
 *      self: The vertex whose class is being retrieved
 *      graph: The graph the vertex belongs to
 *      out: The memory location where the result will be stored
 *
 * Returns:
 *      The class of the vertex
 *      status is set to FABRIC_OK on success, other error code on failure
 */
Class *Fabric_Vertex_get_class(Vertex *self, Graph *graph, error_t *status) {
    ClassStore *cs = Fabric_Graph_get_class_store(graph);
    return Fabric_ClassStore_get_class(cs, self->class_id, status);
}

/**
 * Gets the id of the vertex's first out edge
 */
edgeid_t Fabric_Vertex_get_first_out_edge_id(Vertex *self) {
    return self->first_out_id;
}

/**
 * Returns: TRUE if the vertex has at least one outgoing edge, FALSE if not.
 */
bool_t Fabric_Vertex_has_out_edges(Vertex *self) {
    return self->first_out_id != 0;
}

/**
 * Gets the vertex's first out edge
 *
 * Args:
 *      self: The vertex whose first out edge is being retrieved
 *      graph: The graph the vertex belongs to
 *      out: The memory location where the result will be stored
 *
 * Returns:
 *      The vertex's first out edge
 *      status is set to FABRIC_OK on success, other error code on failure
 */
Edge *Fabric_Vertex_get_first_out_edge(Vertex *self, Graph *graph, error_t *status) {
    EdgeStore *es = Fabric_Graph_get_edge_store(graph);
    return Fabric_EdgeStore_get_edge(es, self->first_out_id, status);
}

/**
 * Gets the id of the vertex's first in edge
 */
edgeid_t Fabric_Vertex_get_first_in_edge_id(Vertex *self) {
    return self->first_in_id;
}

/**
 * Returns: TRUE if the vertex has at least one incoming edge, FALSE if not.
 */
bool_t Fabric_Vertex_has_in_edges(Vertex *self) {
    return self->first_in_id != 0;
}

/**
 * Gets the vertex's first in edge
 *
 * Args:
 *      self: The vertex whose first in edge is being retrieved
 *      graph: The graph the vertex belongs to
 *      out: The memory location where the result will be stored
 *
 * Returns:
 *      The vertex's first in edge
 *      status is set to FABRIC_OK on success, other error code on failure
 */
Edge *Fabric_Vertex_get_first_in_edge(Vertex *self, Graph *graph, error_t *status) {
    EdgeStore *es = Fabric_Graph_get_edge_store(graph);
    return Fabric_EdgeStore_get_edge(es, self->first_in_id, status);
}

/**
 * Gets the id of the vertex's first property
 */
propertyid_t Fabric_Vertex_get_first_property_id(Vertex *self) {
    return self->first_property_id;
}

/**
 * Gets the vertex's first property
 *
 * Fabric_Vertex_has_properties(1) should be called before this method
 * is called to confirm the vertex has a property to return.  Otherwise
 * an error will be returned.
 *
 * Args:
 *      self: The vertex whose first property is being retrieved
 *      graph: The graph the vertex belongs to
 *      out: The memory location where the result will be stored
 *
 * Returns:
 *      The first property of this vertex
 *      status is set to FABRIC_OK on success, other error code on failure
 */
Property *Fabric_Vertex_get_first_property(Vertex *self, Graph *graph, error_t *status) {
    PropertyStore *ps = Fabric_Graph_get_property_store(graph);
    return Fabric_PropertyStore_get_property(ps, self->first_property_id, status);
}

/**
 * Returns: TRUE if the vertex has at least one property, FALSE otherwise
 */
bool_t Fabric_Vertex_has_properties(Vertex *self) {
    return self->first_property_id != 0;
}

#endif