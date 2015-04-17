/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 23, 2015
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "Fabric.h"
#include "Graph.c"

/**
 * This file implements most of the public interface for the FabricDB library.
 */
#define FABRIC_VERSION_NUMBER 1

const char FABRIC_HEADER_STRING[16] = {
    'f', 'a', 'b', 'r', 'i', 'c', 'd', 'b', ' ', 'v', '0', '.', '1', '\0', '\0', '\0'
};

/**
 * Creates a new Fabric graph in the specified file
 *
 * Args:
 *      file: A valid, open file object where the graph will be stored
 *      new_graph: Memory location for the newly created graph
 */
void Fabric_create_graph(FILE *graph_file, Graph *new_graph) {
    int i;
    new_graph->graph_file = graph_file;

    // initialize header strings
    for (i = 0; i < 16; i++) {
        new_graph->fabric_header_string[i] = FABRIC_HEADER_STRING[i];
        new_graph->application_header_string[i] = '\0';
    }

    // Set initial header fields
    new_graph->fabric_version_number = FABRIC_VERSION_NUMBER;
    new_graph->application_version_number = 0;
    new_graph->file_change_counter = 1;
    new_graph->class_store.offset = FABRIC_HEADER_SIZE;
    new_graph->label_store.offset = new_graph->class_store.offset + MIN_PAGE_SIZE;
    new_graph->vertex_store.offset = new_graph->label_store.offset + MIN_PAGE_SIZE;
    new_graph->edge_store.offset = new_graph->vertex_store.offset + MIN_PAGE_SIZE;
    new_graph->property_store.offset = new_graph->edge_store.offset + MIN_PAGE_SIZE;
    new_graph->text_store.offset = new_graph->property_store.offset + MIN_PAGE_SIZE;
    new_graph->text_store.block_size = FABRIC_TEXT_BLOCK_SIZE;
    new_graph->index_store.offset = new_graph->text_store.offset + MIN_PAGE_SIZE;
    new_graph->index_store.page_size = INDEX_PAGE_SIZE;
    new_graph->index_store.page_count = 0;

    // Write header values to file
    Fabric_Graph_write_header (new_graph);

    // TODO: Initialize stores

}

void Fabric_load_graph(FILE *graph_file, Graph *graph) {
    Fabric_Graph_init(graph, graph_file);
}

void Fabric_dump_graph_header (Graph *graph) {
    int i;
    fprintf(stdout, "Fabric Header String: ");
    for (i = 0; i < 16; i++) {
        fputc(graph->fabric_header_string[i], stdout);
    }
    fputc('\n', stdout);
    fprintf(stdout, "Application Header String: ");
    for (i = 0; i < 16; i++) {
        fputc(graph->application_header_string[i], stdout);
    }
    fputc('\n', stdout);

    fprintf(stdout, "Fabric Version Number: %lu\n", (unsigned long)graph->fabric_version_number);
    fprintf(stdout, "Application Version Number: %lu\n", (unsigned long)graph->application_version_number);
    fprintf(stdout, "File Change Counter: %lu\n", (unsigned long)graph->file_change_counter);
    fprintf(stdout, "Class Store Offset: %lu\n", (unsigned long)graph->class_store.offset);
    fprintf(stdout, "Label Store Offset: %lu\n", (unsigned long)graph->label_store.offset);
    fprintf(stdout, "Vertex Store Offset: %lu\n", (unsigned long)graph->vertex_store.offset);
    fprintf(stdout, "Edge Store Offset: %lu\n", (unsigned long)graph->edge_store.offset);
    fprintf(stdout, "Property Store Offset: %lu\n", (unsigned long)graph->property_store.offset);
    fprintf(stdout, "Text Store Offset: %lu\n", (unsigned long)graph->text_store.offset);
    fprintf(stdout, "Text Block Size: %lu\n", (unsigned long)graph->text_store.block_size);
    fprintf(stdout, "Index Store Offset: %lu\n", (unsigned long)graph->index_store.offset);
    fprintf(stdout, "Index Page Size: %lu\n", (unsigned long)graph->index_store.page_size);
    fprintf(stdout, "Index Page Count: %lu\n", (unsigned long)graph->index_store.page_count);
}