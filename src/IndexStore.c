/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 30, 2015
 */

#ifndef _FABRIC_INDEXSTORE_C__
#define _FABRIC_INDEXSTORE_C__

#include "Internal.h"

/**
 * The Index Store is the component of the graph that has the responsibility
 * of managing the storage of Indices.
 *
 * For many of the functions to work, it is assumed that the Index Store
 * is embedded inside a Graph object.
 *
 * For a detailed description of Indices, see the accompanying
 * Index.c file.
 */
typedef struct IndexStore {
    uint32_t offset;        // graph file offset for the class store
    uint32_t size;          // the size of the class store
    uint32_t page_size;     // the size of each index page
    uint32_t page_count;    // the total number of index pages
} IndexStore;

/**
 * Initializes an Index Store object
 *
 * Args:
 *      self: The Index Store object being initialized.  Its offset, page_size,
 *            and page_count should already be set by the Graph
 */
void Fabric_IndexStore_init(IndexStore *self) {
    self->size = self->page_size * self->page_count;
}

/**
 * Retrieves an index from the database
 *
 * Args:
 *      self: The index store that is retrieving an index object
 *      index_id: The id of the index object being retrieved
 *      out: Pointer to where the loaded index will be stored
 *
 * Returns: FABRIC_OK on success, other error code on failure
 */
Index *Fabric_IndexStore_get_index(IndexStore *self, indexid_t index_id, error_t *status) {
    //TODO this is a stub
    *status = FABRIC_OK;
    return NULL;
}

ClassIndex *Fabric_IndexStore_get_class_index(IndexStore *self, error_t *status) {
    //TODO this is a stub
    *status = FABRIC_OK;
    return NULL;
}

LabelIndex *Fabric_IndexStore_get_label_index(IndexStore *self, error_t *status) {
    // TODO This is a stub
    *status = FABRIC_OK;
    return NULL;
}

indexid_t Fabric_IndexStore_create_id_index(IndexStore *self, classid_t class_id, error_t *status) {
    // TODO this is a stub
    *status = FABRIC_OK;
    return 0;
}

error_t Fabric_IndexStore_delete_id_index(IndexStore *self, indexid_t index_id) {
    // TODO this is a stub
    return FABRIC_OK;
}

error_t Fabric_IndexStore_add_class_to_index(IndexStore *self, Class *class) {
    // TODO This is a stub
    return FABRIC_OK;
}

error_t Fabric_IndexStore_add_class_to_index_if_not_exists(IndexStore *self, Class *class) {
    // TODO This is a stub
    return FABRIC_OK;
}

error_t Fabric_IndexStore_remove_class_from_index(IndexStore *self, Class *class) {
    // TODO This is a stub
    return FABRIC_OK;
}

error_t Fabric_IndexStore_add_label_to_index(IndexStore *self, Label *label) {
    // TODO This is a stub
    return FABRIC_OK;
}

#endif