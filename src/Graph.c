/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 23, 2015
 */

#ifndef _FABRIC_GRAPH_C__
#define _FABRIC_GRAPH_C__

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include "Fabric.h"
#include "Memory.c"
#include "ClassStore.c"
#include "LabelStore.c"
#include "VertexStore.c"
#include "EdgeStore.c"
#include "PropertyStore.c"
#include "TextStore.c"
#include "IndexStore.c"
#include "Class.c"
#include "Label.c"
#include "Vertex.c"
#include "Edge.c"
#include "Property.c"
#include "Text.c"
#include "Index.c"
#include "DynamicList.c"
#include "IdSet.c"
#include "EntityMap.c"

/**
 * File offset definitions
 *
 * These are the number of bytes from the beginning of the file
 * at which each of the header fields is located.
 */
#define FABRIC_HEADER_STRING_OFFSET 0
#define APPLICATION_HEADER_STRING_OFFSET 16
#define FABRIC_VERSION_NUMBER_OFFSET 32
#define APPLICATION_VERSION_NUMBER_OFFSET 36
#define FILE_CHANGE_COUNTER_OFFSET 40
#define CLASS_STORE_OFFSET_OFFSET 44
#define LABEL_STORE_OFFSET_OFFSET 48
#define VERTEX_STORE_OFFSET_OFFSET 52
#define EDGE_STORE_OFFSET_OFFSET 56
#define PROPERTY_STORE_OFFSET_OFFSET 60
#define TEXT_STORE_OFFSET_OFFSET 64
#define TEXT_BLOCK_SIZE_OFFSET 68
#define INDEX_STORE_OFFSET_OFFSET 72
#define INDEX_PAGE_SIZE_OFFSET 76
#define INDEX_PAGE_COUNT_OFFSET 80
#define FABRIC_HEADER_SIZE 84

/**
 * A Graph object is responsible for managing the storage and retrieval
 * of a graph as a whole.  It is persistent in the sense that it writes
 * and reads itself from a binary file.
 *
 * A graph has several subcompenents which are responsible for managing different
 * portions of the graph store file.  The graph maintains important pieces
 * of metadata in the header of its file which it uses to initialize itself.
 */
typedef struct Graph {
    FILE *graph_file;                        // The file in which this graph is stored
    uint8_t fabric_header_string[16];       // Used to verify file type by Fabric
    uint8_t application_header_string[16];  // Optionally used by app to verify file type
    uint32_t fabric_version_number;         // Version of fabric in use
    uint32_t application_version_number;    // Optional version of application
    uint32_t file_change_counter;           // Tracks changes to this file
    ClassStore class_store;                  // Store for vertex classes
    LabelStore label_store;                  // Store for labels
    VertexStore vertex_store;                // Store for vertices
    EdgeStore edge_store;                    // Store for edges
    PropertyStore property_store;            // Store for properties
    TextStore text_store;                    // Store for text
    IndexStore index_store;                  // Store for indices
} Graph;

/**
 * Writes data to the graphs file
 *
 * Args:
 *      self: The graph object whose header is being written
 *      bytes: An array of bytes that are to be written whose size is specified in num_bytes
 *      num_bytes: The number of bytes to write to the file
 *      offset: The position in the file to write to or -1 to write from current position in file
 */
void Fabric_Graph_write_bytes (Graph *self, uint8_t *bytes, int num_bytes, long offset) {
    FILE *file = self->graph_file;
    int i;

    // Set position to appropriate offset
    if (offset != -1) {
        fseek(file, offset, SEEK_SET);
    }

#if FABRIC_DEBUG
    printf("Writing %d bytes at %lu\n", num_bytes, ftell(file));
#endif
    // Write the data
    for (i = 0; i < num_bytes; i++) {
        fputc(bytes[i], file);
#if FABRIC_DEBUG
        fprintf(stdout, "%d ", bytes[i]);
#endif
    }
#if DEBUG
    fputc('\n', stdout);
#endif
}

/**
 * Writes a uint32 value to the database
 *
 * Args:
 *      self: The graph being written to
 *      value: The value to write
 *      offset: The location in the file to write the value
 */
void Fabric_Graph_write_uint32 (Graph *self, uint32_t value, long offset) {
    uint32_t network_order_value = htobe32(value);
    uint8_t *bytes = (uint8_t*) &network_order_value;
#if FABRIC_DEBUG
    printf("Writing uint32 %u with network value %u\n", value, network_order_value);
#endif
    Fabric_Graph_write_bytes(self, bytes, sizeof(value), offset);
}

/**
 * Writes a uint16 value to the database
 *
 * Args:
 *      self: The graph being written to
 *      value: The value to write
 *      offset: The location in the file to write the value
 */
void Fabric_Graph_write_uint16 (Graph *self, uint16_t value, long offset) {
    uint16_t network_order_value = htobe32(value);
    uint8_t *bytes = (uint8_t*) &network_order_value;
#if FABRIC_DEBUG
    printf("Writing uint16 %u with network value %u\n", value, network_order_value);
#endif
    Fabric_Graph_write_bytes(self, bytes, sizeof(value), offset);
}

/**
 * Reads bytes from the graph file into a buffer
 *
 * Args:
 *      self: The graph object being read from
 *      destination: The location to store the read data
 *      num_bytes: The number of bytes to read
 *      offset: The offset to read from or -1 to read from current offset
 */
void Fabric_Graph_read_bytes (Graph *self, uint8_t *destination, int num_bytes, long offset) {
    int i;
    // Set position to appropriate offset
    if (offset != -1) {
        fseek(self->graph_file, offset, SEEK_SET);
    }
#if FABRIC_DEBUG
    printf("Reading %d bytes at %lu\n", num_bytes, ftell(self->graph_file));
#endif
    for (i = 0; i < num_bytes; i++) {
        destination[i] = (uint8_t)fgetc(self->graph_file);
#if FABRIC_DEBUG
        fprintf(stdout, "%d ", (int)destination[i]);
#endif
    }
#if FABRIC_DEBUG
    fputc('\n', stdout);
#endif
}


/**
 * Reads and returns a 32 bit unsigned integer value from the database file
 *
 * Args:
 *      self: The graph being read from
 *      offset: The position to read from or -1 for current position
 *
 * Returns: The value as an integer in host endianness
 */
uint32_t Fabric_Graph_read_uint32 (Graph *self, long offset) {
    uint8_t bytes[sizeof(uint32_t)];
    uint32_t result;
    Fabric_Graph_read_bytes(self, bytes, sizeof(uint32_t), offset);
    result = betoh32 (*((uint32_t*) bytes));
#if DEBUG
    printf("uint32 read is %u with network %u\n", result, (uint32_t) *bytes);
#endif
    return result;
}

/**
 * Reads and returns a 32 bit unsigned integer value from the database file
 *
 * Args:
 *      self: The graph being read from
 *      offset: The position to read from or -1 for current position
 *
 * Returns: The value as an integer in host endianness
 */
uint16_t Fabric_Graph_read_uint16 (Graph *self, long offset) {
    uint8_t bytes[sizeof(uint16_t)];
    uint16_t result;
    Fabric_Graph_read_bytes(self, bytes, sizeof(uint16_t), offset);
    result = betoh16 (*((uint16_t*) bytes));
#if DEBUG
    printf("uint16 read is %u with network %u\n", result, (uint16_t) *bytes);
#endif
    return result;
}

/**
 * Writes all of a graphs header values to its file
 *
 * Primarily used for initialization of a new graph file
 *
 * Args:
 *      self: A fully instantiated Graph object whose values will be written
 *
 * Returns: 0 on success, less than 0 on failure
 */
int Fabric_Graph_write_header (Graph *self) {
    FILE *file = self->graph_file;
    int i;

    fseek (file, 0L, SEEK_SET);   // ensure file pointer at beginning of file

    // Write the file header string
    Fabric_Graph_write_bytes(self, self->fabric_header_string, 16, 0);
    // Write the application header string
    Fabric_Graph_write_bytes(self, self->application_header_string, 16, -1);
    // Write the version number
    Fabric_Graph_write_uint32 (self, self->fabric_version_number, -1);
    // Write application version number;
    Fabric_Graph_write_uint32 (self, self->application_version_number, -1);
    // Write file change counter
    Fabric_Graph_write_uint32 (self, self->file_change_counter, -1);
    // Write class store offset
    Fabric_Graph_write_uint32 (self, self->class_store.offset, -1);
    // Write label store offset
    Fabric_Graph_write_uint32 (self, self->label_store.offset, -1);
    // Write the vertex store offset
    Fabric_Graph_write_uint32 (self, self->vertex_store.offset, -1);
    // Write the edge store offest
    Fabric_Graph_write_uint32 (self, self->edge_store.offset, -1);
    // Write the property store
    Fabric_Graph_write_uint32 (self, self->property_store.offset, -1);
    // Write the text store offset
    Fabric_Graph_write_uint32 (self, self->text_store.offset, -1);
    // Write the text block size
    Fabric_Graph_write_uint32 (self, self->text_store.block_size, -1);
    // Write the index store offset
    Fabric_Graph_write_uint32 (self, self->index_store.offset, -1);
    // Write the index page size
    Fabric_Graph_write_uint32 (self, self->index_store.page_size, -1);
    // Write the index page count
    Fabric_Graph_write_uint32 (self, self->index_store.page_count, -1);

    return 0;
}

/**
 * Initializes a Graph object from a file
 *
 * Args:
 *      self: An uninitialized graph object to be initialized
 *      graph_file: The file to read the graph from
 *
 * Returns: 0 on success; less than 0 on error
 */
int Fabric_Graph_init(Graph *self, FILE *graph_file) {
    // Set the file for the graph
    self->graph_file = graph_file;
    // Read the fabric header string
    Fabric_Graph_read_bytes(self, self->fabric_header_string, 16, 0);
    // Read the application header string
    Fabric_Graph_read_bytes(self, self->application_header_string, 16, -1);
    // Read the fabric version number
    self->fabric_version_number = Fabric_Graph_read_uint32(self, -1);
    // Read the application version number
    self->application_version_number = Fabric_Graph_read_uint32(self, -1);
    // Read the file change counter
    self->file_change_counter = Fabric_Graph_read_uint32(self, -1);
    // Read the class store offset
    self->class_store.offset = Fabric_Graph_read_uint32(self, -1);
    // Read the label store offset
    self->label_store.offset = Fabric_Graph_read_uint32(self, -1);
    // Read the vertex store ofset
    self->vertex_store.offset = Fabric_Graph_read_uint32(self, -1);
    // Read the edge store offset
    self->edge_store.offset = Fabric_Graph_read_uint32(self, -1);
    // Read the property store offset
    self->property_store.offset = Fabric_Graph_read_uint32(self, -1);
    // Read the text store offset
    self->text_store.offset = Fabric_Graph_read_uint32(self, -1);
    // Read the text store block size
    self->text_store.block_size = Fabric_Graph_read_uint32(self, -1);
    // Read the index store offset
    self->index_store.offset = Fabric_Graph_read_uint32(self, -1);
    // Read the index page size
    self->index_store.page_size = Fabric_Graph_read_uint32(self, -1);
    // Read the index page count
    self->index_store.page_count = Fabric_Graph_read_uint32(self, -1);

    // Initialize each of the stores
    Fabric_ClassStore_init(&self->class_store);
    Fabric_LabelStore_init(&self->label_store);
    Fabric_VertexStore_init(&self->vertex_store);
    Fabric_EdgeStore_init(&self->edge_store);
    Fabric_PropertyStore_init(&self->property_store);
    Fabric_TextStore_init(&self->text_store);
    Fabric_IndexStore_init(&self->index_store);

    return 0;
}


/**
 * Returns the Graph object a Class Store belongs to
 *
 * Args:
 *      self: The Class Store whose Graph is being retrieved
 *
 * Returns: The Graph object the Class Store belongs to
 */
Graph* Fabric_ClassStore_get_graph(ClassStore *self) {
    size_t self_int = (size_t) self;
    size_t offset = offsetof(Graph, class_store);
    return (Graph*) (self_int - offset);
}

/**
 * Returns the Graph object a Label Store belongs to
 *
 * Args:
 *      self: The Label Store whose Graph is being retrieved
 *
 * Returns: The Graph object the Label Store belongs to
 */
Graph* Fabric_LabelStore_get_graph(LabelStore *self) {
    size_t self_int = (size_t) self;
    size_t offset = offsetof(Graph, class_store);
    return (Graph*) (self_int - offset);
}

/**
 * Returns the Graph object a Vertex Store belongs to
 *
 * Args:
 *      self: The Vertex Store whose Graph is being retrieved
 *
 * Returns: The Graph object the Vertex Store belongs to
 */
Graph* Fabric_VertexStore_get_graph(VertexStore *self) {
    size_t self_int = (size_t) self;
    size_t offset = offsetof(Graph, vertex_store);
    return (Graph*) (self_int - offset);
}

/**
 * Returns the Graph object an Edge Store belongs to
 *
 * Args:
 *      self: The Edge Store whose Graph is being retrieved
 *
 * Returns: The Graph object the Edge Store belongs to
 */
Graph* Fabric_EdgeStore_get_graph(EdgeStore *self) {
    size_t self_int = (size_t) self;
    size_t offset = offsetof(Graph, edge_store);
    return (Graph*) (self_int - offset);
}

/**
 * Returns the Graph object a Property Store belongs to
 *
 * Args:
 *      self: The Property Store whose Graph is being retrieved
 *
 * Returns: The Graph object the Property Store belongs to
 */
Graph* Fabric_PropertyStore_get_graph(PropertyStore *self) {
    size_t self_int = (size_t) self;
    size_t offset = offsetof(Graph, property_store);
    return (Graph*) (self_int - offset);
}

/**
 * Returns the Graph object a Text Store belongs to
 *
 * Args:
 *      self: The Text Store whose Graph is being retrieved
 *
 * Returns: The Graph object the Text Store belongs to
 */
Graph* Fabric_TextStore_get_graph(TextStore *self) {
    size_t self_int = (size_t) self;
    size_t offset = offsetof(Graph, text_store);
    return (Graph*) (self_int - offset);
}

/**
 * Returns the Graph object an Index Store belongs to
 *
 * Args:
 *      self: The Index Store whose Graph is being retrieved
 *
 * Returns: The Graph object the Index Store belongs to
 */
Graph* Fabric_IndexStore_get_graph(IndexStore *self) {
    size_t self_int = (size_t) self;
    size_t offset = offsetof(Graph, index_store);
    return (Graph*) (self_int - offset);
}

/**
 * Gets the file offset of a graph's class store data
 *
 * Args:
 *      self: A fully instantiated Graph object
 *
 * Returns: The offset of the graph's class store's data
 */
uint32_t Fabric_Graph_get_class_store_offset(Graph* self) {
    return self->class_store.offset;
}

/**
 * Gets the file offset of a graph's label store data
 *
 * Args:
 *      self: A fully instantiated Graph object
 *
 * Returns: The offset of the graph's label store's data
 */
uint32_t Fabric_Graph_get_label_store_offset(Graph* self) {
    return self->label_store.offset;
}

/**
 * Gets the file offset of a graph's vertex store data
 *
 * Args:
 *      self: A fully instantiated Graph object
 *
 * Returns: The offset of the graph's vertex store's data
 */
uint32_t Fabric_Graph_get_vertex_store_offset(Graph* self) {
    return self->vertex_store.offset;
}

/**
 * Gets the file offset of a graph's edge store data
 *
 * Args:
 *      self: A fully instantiated Graph object
 *
 * Returns: The offset of the graph's edge store's data
 */
uint32_t Fabric_Graph_get_edge_store_offset(Graph* self) {
    return self->edge_store.offset;
}

/**
 * Gets the file offset of a graph's property store data
 *
 * Args:
 *      self: A fully instantiated Graph object
 *
 * Returns: The offset of the graph's property store's data
 */
uint32_t Fabric_Graph_get_property_store_offset(Graph* self) {
    return self->property_store.offset;
}

/**
 * Gets the file offset of a graph's text store data
 *
 * Args:
 *      self: A fully instantiated Graph object
 *
 * Returns: The offset of the graph's text store's data
 */
uint32_t Fabric_Graph_get_text_store_offset(Graph* self) {
    return self->text_store.offset;
}

/**
 * Gets the file offset of a graph's index store data
 *
 * Args:
 *      self: A fully instantiated Graph object
 *
 * Returns: The offset of the graph's index store's data
 */
uint32_t Fabric_Graph_get_index_store_offset(Graph* self) {
    return self->index_store.offset;
}

/**
 * Gets a graph's class store
 *
 * Args:
 *      self: The graph whose class store is being retrieved
 *
 * Returns: the graph's class store
 */
ClassStore *Fabric_Graph_get_class_store(Graph *self) {
    return &self->class_store;
}

/**
 * Gets a graph's label store
 *
 * Args:
 *      self: The graph whose label store is being retrieved
 *
 * Returns: the graph's label store
 */
LabelStore *Fabric_Graph_get_label_store(Graph *self) {
    return &self->label_store;
}

/**
 * Gets a graph's vertex store
 *
 * Args:
 *      self: The graph whose vertex store is being retrieved
 *
 * Returns: the graph's vertex store
 */
VertexStore *Fabric_Graph_get_vertex_store(Graph *self) {
    return &self->vertex_store;
}

/**
 * Gets a graph's edge store
 *
 * Args:
 *      self: The graph whose edge store is being retrieved
 *
 * Returns: the graph's edge store
 */
EdgeStore *Fabric_Graph_get_edge_store(Graph *self) {
    return &self->edge_store;
}

/**
 * Gets a graph's property store
 *
 * Args:
 *      self: The graph whose property store is being retrieved
 *
 * Returns: the graph's property store
 */
PropertyStore *Fabric_Graph_get_property_store(Graph *self) {
    return &self->property_store;
}

/**
 * Gets a graph's text store
 *
 * Args:
 *      self: The graph whose text store is being retrieved
 *
 * Returns: the graph's text store
 */
TextStore *Fabric_Graph_get_text_store(Graph *self) {
    return &self->text_store;
}

/**
 * Gets a graph's index store
 *
 * Args:
 *      self: The graph whose index store is being retrieved
 *
 * Returns: the graph's index store
 */
IndexStore *Fabric_Graph_get_index_store(Graph *self) {
    return &self->index_store;
}

#endif