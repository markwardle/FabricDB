/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 24, 2015
 * Updated: March 27, 2015
 */

#ifndef _FABRIC_EDGE_C__
#define _FABRIC_EDGE_C__

#include "Internal.h"

/**
 * An edge represents the connections between primary entities in a graph.
 *
 * In fact, an edge is an entity all its own since it can have properties
 * associated with it.  A property is a piece of data associated with an
 * an edge in the form of a key-value pair.  An edge may have any number
 * of properties.
 *
 * An edge always has a label which identifies the type of relationship
 * it represents.  Edge labels are automatically indexed.
 *
 * An edge also must have a from vertex and a to vertex.  As such, a Fabric
 * graph is directed.  However, it is just as easy to search for outgoing
 * edges as it is to search for incoming edges.
 *
 * The incoming and outoing edges of a vertex are stored as a linked link,
 * hence the from_next_id and to_next_id fields.
 *
 * The database represents an edge with 24 bytes
 *
 * +----+----+----+----+----+----+----+----+----+----+----+----+
 * | label_id          | from_id           | to_id             |
 * +----+----+----+----+----+----+----+----+----+----+----+----+
 * | next_out_id       | next_in_id        | first_property_id |
 * +----+----+----+----+----+----+----+----+----+----+----+----+
 */
typedef struct Edge {
    edgeid_t id;                    // The internal id of this edge
    labelid_t label_id;             // The id of the label associated with this edge
    vertexid_t from_id;             // The id of the start vertex for this edge
    vertexid_t to_id;               // The id of the end vertex for this edge
    edgeid_t next_out_id;           // The id of the start vertex's next outgoing edge
    edgeid_t next_in_id;            // The id of the end vertex's next incoming edge
    propertyid_t first_property_id; // the id of the first property for this edge
} Edge;


/**
 * Gets an edges's id
 */
edgeid_t Fabric_Edge_get_id(Edge *self) {
    return self->id;
}

/**
 * Sets an edge's id
 */
void Fabric_Edge_set_id(Edge *self, edgeid_t id) {
    self->id = id;
}

/**
 * Initializes an edge object from its database data
 *
 * The edges's id should be set before it is initialized
 *
 * Args:
 *      self: The edge being initialized
 *      data: An array of bytes that represent this edge in the database (24 bytes)
 *
 * Returns: FABRIC_OK on success, other error code on failure
 */
int Fabric_Edge_init(Edge *self, uint8_t *data) {
    // An Edge's id must be set externally
    if (self->id < 1) {
        return FABRIC_EDGE_INVALID_ID;
    }
    self->label_id = betoh32(*(labelid_t*)data);
    self->from_id = betoh32(*(vertexid_t*)(data+4));
    self->to_id = betoh32(*(vertexid_t*)(data+8));
    self->next_out_id = betoh32(*(edgeid_t*)(data+12));
    self->next_in_id = betoh32(*(edgeid_t*)(data+16));
    self->first_property_id = betoh32(*(propertyid_t*)(data+20));

    return FABRIC_OK;
}

/**
 * Returns the label id for an edge
 */
labelid_t Fabric_Edge_get_label_id(Edge *self) {
    return self->label_id;
}

/**
 * Gets the label for an edge
 *
 * Args:
 *      self: The edge object whose label is being retrieved
 *      graph: The graph object the edge belongs to
 *      out: A pointer to where the result will be stored
 *
 * Returns:
 *      The label for the edge
 *      status set to FABRIC_OK on success, other error number on failure
 */
Label *Fabric_Edge_get_label(Edge *self, Graph *graph, error_t *status) {
    LabelStore *ls = Fabric_Graph_get_label_store(graph);
    return Fabric_LabelStore_get_label(ls, self->label_id, status);
}

/**
 * Gets the id of the edge's start vertex
 */
vertexid_t Fabric_Edge_get_from_vertex_id(Edge *self) {
    return self->from_id;
}

/**
 * Returns the edge's start vertex
 *
 * Args:
 *      self: The edge whose start vertex is being retrieved
 *      graph: The graph the edge belongs to
 *      out: Memory location to store result in
 *
 * Returns:
 *      The edge's starting vertex
 *      status is set to FABRIC_OK on success, other error number on failure
 */
Vertex *Fabric_Edge_get_from_vertex(Edge *self, Graph *graph, error_t *status) {
    VertexStore *vs = Fabric_Graph_get_vertex_store(graph);
    return Fabric_VertexStore_get_vertex(vs, self->from_id, status);
}

/**
 * Gets the id of the edge's end vertex
 */
vertexid_t Fabric_Edge_get_to_vertex_id(Edge *self) {
    return self->to_id;
}

/**
 * Returns the edge's end vertex
 *
 * Args:
 *      self: The edge whose end vertex is being retrieved
 *      graph: The graph the edge belongs to
 *      out: Memory location to store result in
 *
 * Returns:
 *      The edge's ending vertex
 *      status is set to FABRIC_OK on success, other error number on failure
 */
Vertex *Fabric_Edge_get_to_vertex(Edge *self, Graph *graph, error_t *status) {
    VertexStore *vs = Fabric_Graph_get_vertex_store(graph);
    return Fabric_VertexStore_get_vertex(vs, self->to_id, status);
}

/**
 * Gets the id of the edge's start vertex's next out edge
 */
edgeid_t Fabric_Edge_get_next_out_edge_id(Edge *self) {
    return self->next_out_id;
}

/**
 * Returns the edge's start vertex's next out edge
 *
 * Fabric_Edge_has_next_out_edge(1) should be called to confirm
 * the edge has an edge to return before this method is called
 *
 * Args:
 *      self: The edge whose start vertex's next out edge is being retrieved
 *      graph: The graph the edge belongs to
 *      out: Memory location to store result in
 *
 * Returns:
 *      The edge's start vertex's next outgoing edge
 *      FABRIC_OK on success, other error number on failure
 */
Edge *Fabric_Edge_get_next_out_edge(Edge *self, Graph *graph, error_t *status) {
    EdgeStore *es = Fabric_Graph_get_edge_store(graph);
    return Fabric_EdgeStore_get_edge(es, self->next_out_id, status);
}

/**
 * Returns whether or not the edge has a next out edge
 */
bool_t Fabric_Edge_has_next_out_edge(Edge *self) {
    return self->next_out_id != 0;
}

/**
 * Gets the id of the edge's end vertex's next in edge
 */
edgeid_t Fabric_Edge_get_next_in_edge_id(Edge *self) {
    return self->next_in_id;
}

/**
 * Returns the edge's end vertex's next in edge
 *
 * Fabric_Edge_has_next_in_edge(1) should be called to confirm
 * the edge has an edge to return before this method is called
 *
 * Args:
 *      self: The edge whose end vertex's next in edge is being retrieved
 *      graph: The graph the edge belongs to
 *      out: Memory location to store result in
 *
 * Returns:
 *      The edge's end vertex's next incoming edge
 *      FABRIC_OK on success, other error number on failure
 */
Edge *Fabric_Edge_get_next_in_edge(Edge *self, Graph *graph, error_t *status) {
    EdgeStore *es = Fabric_Graph_get_edge_store(graph);
    return Fabric_EdgeStore_get_edge(es, self->next_in_id, status);
}

/**
 * Returns whether or not the edge has a next in edge
 */
bool_t Fabric_Edge_has_next_in_edge(Edge *self) {
    return self->next_in_id != 0;
}

/**
 * Gets the id of the edge's first property
 */
propertyid_t Fabric_Edge_get_first_property_id(Edge *self) {
    return self->first_property_id;
}

/**
 * Gets the edges's first property
 *
 * Fabric_Edge_has_properties(1) should be called before this method
 * is called to confirm the edge has a property to return.  Otherwise
 * an error will be returned.
 *
 * Args:
 *      self: The edge whose first property is being retrieved
 *      graph: The graph the edge belongs to
 *      out: The memory location where the result will be stored
 *
 * Returns:
 *      The edge's first property
 *      FABRIC_OK on success, other error code on failure
 */
Property *Fabric_Edge_get_first_property(Edge *self, Graph *graph, error_t *status) {
    PropertyStore *ps = Fabric_Graph_get_property_store(graph);
    return Fabric_PropertyStore_get_property(ps, self->first_property_id, status);
}

/**
 * Returns whether or not an edge has at least one property
 */
bool_t Fabric_Edge_has_properties(Edge *self) {
    return self->first_property_id != 0;
}

#endif