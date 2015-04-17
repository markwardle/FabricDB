/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 25, 2015
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifndef _FABRIC_TEST_ALL__
#include "Fabric.c"
#endif


void test_create_db() {
    FILE *db_file;
    Graph created_graph;
    Graph loaded_graph;
    Graph *test_graph;


    char *file_name = "test.fdb";
    db_file = fopen(file_name, "w+b");

    Fabric_create_graph(db_file, &created_graph);

    fclose(db_file);

    db_file = fopen(file_name, "r+b");
    Fabric_load_graph(db_file, &loaded_graph);

    // ensure the created graph has all the same values as the loaded graph
    assert(memcmp(&loaded_graph.fabric_header_string, &created_graph.fabric_header_string, 16) == 0);
    assert(memcmp(&loaded_graph.application_header_string, &created_graph.application_header_string, 16) == 0);
    assert(loaded_graph.fabric_version_number == created_graph.fabric_version_number);
    assert(loaded_graph.application_version_number == created_graph.application_version_number);
    assert(loaded_graph.file_change_counter == created_graph.file_change_counter);
    assert(loaded_graph.class_store.offset == created_graph.class_store.offset);
    assert(loaded_graph.label_store.offset == created_graph.label_store.offset);
    assert(loaded_graph.vertex_store.offset == created_graph.vertex_store.offset);
    assert(loaded_graph.property_store.offset == created_graph.property_store.offset);
    assert(loaded_graph.text_store.offset == created_graph.text_store.offset);
    assert(loaded_graph.text_store.block_size == created_graph.text_store.block_size);
    assert(loaded_graph.index_store.offset == created_graph.index_store.offset);
    assert(loaded_graph.index_store.page_size == created_graph.index_store.page_size);
    assert(loaded_graph.index_store.page_count == created_graph.index_store.page_count);

    // make sure each component of the graph can find its parent correctly
    test_graph = Fabric_ClassStore_get_graph(&loaded_graph.class_store);
    assert(test_graph = &loaded_graph);
    test_graph = Fabric_LabelStore_get_graph(&loaded_graph.label_store);
    assert(test_graph = &loaded_graph);
    test_graph = Fabric_VertexStore_get_graph(&loaded_graph.vertex_store);
    assert(test_graph = &loaded_graph);
    test_graph = Fabric_EdgeStore_get_graph(&loaded_graph.edge_store);
    assert(test_graph = &loaded_graph);
    test_graph = Fabric_PropertyStore_get_graph(&loaded_graph.property_store);
    assert(test_graph = &loaded_graph);
    test_graph = Fabric_TextStore_get_graph(&loaded_graph.text_store);
    assert(test_graph = &loaded_graph);
    test_graph = Fabric_IndexStore_get_graph(&loaded_graph.index_store);
    assert(test_graph = &loaded_graph);
    printf("All tests passed for db creation.\n");
    // clean up
    fclose(db_file);
    remove(file_name);
}

void test_graph() {
    test_create_db();
}

#ifndef _FABRIC_TEST_ALL__
int main() {
    test_graph();
    return 0;
}
#endif