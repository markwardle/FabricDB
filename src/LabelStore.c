/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 31, 2015
 */

#ifndef _FABRIC_LABELSTORE_C__
#define _FABRIC_LABELSTORE_C__

#include <stddef.h>
#include "Internal.h"

#define FABRIC_LABELSTORE_HEADER_SIZE 12

/**
 * The Label Store is the component of the graph that has the responsibility
 * of managing the storage of Label objects.
 *
 * For many of the functions to work, it is assumed that the Label Store
 * is embedded inside a Graph object.
 *
 * For a detailed description of Label objects, see the accompanying
 * Label.c file.
 */
typedef struct LabelStore {
    uint32_t offset;        // graph file offset for the label store
    uint32_t size;          // the size of the label store
    uint32_t num_labels;    // The number of labels used by the graph
    uint32_t next_free_id;  // The next available index id
    uint32_t last_free_id;  // The last label id available
                             // Always points to an previously unwritten portion of the file
    EntityMap *cache;        // A cache of Label objects
    IdSet *changed;          // A list of changed labels that need to be written
} LabelStore;

/**
 * Initializes a label store object
 *
 * Args:
 *      self: The Label Store object being initialized.  Its offset should
 *            already be set by the Graph
 *
 * Returns: FABRIC_OK on success or other error code on failure
 */
error_t Fabric_LabelStore_init(LabelStore *self) {
    Graph *graph = Fabric_LabelStore_get_graph(self);
    self->size = Fabric_Graph_get_vertex_store_offset(graph) - self->offset;
    self->num_labels = Fabric_Graph_read_uint16(graph, self->offset);
    self->next_free_id = Fabric_Graph_read_uint16(graph, self->offset + 4);
    self->last_free_id = Fabric_Graph_read_uint16(graph, self->offset + 8);

    self->cache = Fabric_EntityMap_new(&status);
    if (FABRIC_OK != status) {
        return status;
    }
    self->changed = Fabric_IdSet_new(&status);
    return status;
}

/**
 * Internal function for calculating the file offset of a label
 */
static inline uint32_t Fabric_LabelStore__get_id_offset(LabelStore *self, labelid_t label_id){
    return (label_id - 1) * FABRIC_LABEL_STORAGE_SIZE + self->offset + FABRIC_LABELSTORE_HEADER_SIZE;
}

error_t Fabric_LabelStore_flush(LabelStore *self) {
    // TODO: this is a stub
    return FABRIC_OK;
}

/**
 * Internal function for getting and updating the next id for a label
 */
static
labelid_t Fabric_LabelStore__next_id(LabelStore *self) {
    uint32_t next_id_offset;
    Graph *g;
    classid_t next_free_id = self->next_free_id;
    if (self->next_free_id == self->last_free_id) {
        self->next_free_id++;
        self->last_free_id++;
    } else if (Fabric_EntityMap_has_key(self->cache, next_free_id)) {
        // The next free id for a free label is stored at the refs offset
        self->next_free_id = Fabric_Label_get_refs(Fabric_EntityMap_get(self->cache, next_free_id));
        // TODO is there a case when such a label would not be in the changed set?
    }
    else {
        g = Fabric_LabelStore_get_graph(self);
        next_id_offset = Fabric_LabelStore__get_id_offset(self, next_free_id);
        self->next_free_id = Fabric_Graph_read_uint32(g, next_id_offset + sizeof(textid_t));
    }
    return next_free_id;
}

static
inline
void Fabric_LabelStore__add_free_id(LabelStore *self, Label *label) {
    Fabric_Label_set_refs(label, self->next_free_id);
    self->next_free_id = Fabric_Label_get_id(label);
}

Label* Fabric_LabelStore_get_label(LabelStore *self, uint32_t label_id, error_t *status) {
    Label *label = Fabric_EntityMap_get(self->cache, label_id);
    uint32_t offset;
    uint8_t data[FABRIC_LABEL_STORAGE_SIZE];
    Graph *g;
    *status = FABRIC_OK;

    if (!label) {
        offset = Fabric_LabelStore__get_id_offset(self, label_id);
        if (offset > self->size + self->offset || label_id < 1) {
            *status = FABRIC_LABELSTORE_INVALID_ID;
            return label;
        }
        g = Fabric_LabelStore_get_graph(self);
        Fabric_Graph_read_bytes(g, data, FABRIC_LABEL_STORAGE_SIZE, offset);
        label = Fabric_Label_new(label_id, status);
        if (*status != FABRIC_OK) {
            // label should be NULL here
            return label;
        }
        *status = Fabric_Label_init(label, data);
        // TODO decide what labels should be cached
        Fabric_EntityMap_set(self->cache, label_id, label);
    }

    // make sure the label is valid
    // if it's text id is 0 then it is not in use
    if (!Fabric_Label_is_in_use(label)) {
        *status = FABRIC_LABEL_DOESNT_EXIST;
        return NULL;
    }

    return label;
}

Label* Fabric_LabelStore_get_label_by_name(LabelStore *self, text_t name, error_t *status) {
    Graph *g = Fabric_LabelStore_get_graph(self);
    IndexStore *is = Fabric_Graph_get_index_store(g);
    LabelIndex *li = Fabric_IndexStore_get_label_index(is, status);
    if (FABRIC_OK != status) {
        return NULL;
    }

    labelid_t id = Fabric_LabelIndex_get_label_id(li, name, status);
    if (FABRIC_OK != status) {
        return NULL;
    } else if (id == 0) {
        *status = FABRIC_LABEL_DOESNT_EXIST;
        return NULL;
    }

    return Fabric_LabelStore_get_class(self, id, status);
}


labelid_t Fabric_LabelStore_add_label(LabelStore *self, text_t name, error_t *status) {
    // TODO: This is a stub
    Label *label = Fabric_LabelStore_get_label_by_name(self, name, status);
    Graph *g;
    TextStore *ts;
    IndexStore *is;
    labelid_t next_id;
    textid_t text_id = 0;
    error_t stat;

    if (FABRIC_LABEL_DOESNT_EXIST == *status) {
        // TODO: refactor into another method
        g = Fabric_LabelStore_get_graph(self);
        ts = Fabric_Graph_get_label_store(g);

        if (FABRIC_OK != *status) {
            return 0;
        }
        next_id = Fabric_LabelStore_next_id(self);
        label = Fabric_Label_new(next_id, status);
        if (FABRIC_OK != *status ||
            0 == (text_id = Fabric_TextStore_create_text(ts, name, status))) {
            Fabric_LabelStore__add_free_id(self, label);
            return 0;
        }
        Fabric_Label_set_text_id(label, text_id);
        Fabric_Label_set_refs(label, 0);
        is = Fabric_Graph_get_index_store(g);
        Fabric_IndexStore_add_label_to_index(is, label, status);
        if (FABRIC_OK != *status) {
            // clean up
            Fabric_Label_set_text_id(label, 0);
            Fabric_LabelStore__add_free_id(self, label);
            Fabric_TextStore_delete_text(ts, text_id);
            return 0;
        }

    } else if (FABRIC_OK != *status) {
        return 0;
    } else {
        Fabric_Label_add_ref(label);
        next_id = Fabric_Label_get_id(label);
    }
    
    if (FABRIC_OK != (stat = Fabric_EntityMap_set(self->cache, next_id, label)) ||
        FABRIC_OK != (stat = Fabric_IdSet_add(self->changed, next_id))) {
        if (text_id != 0) {
            Fabric_LabelStore__add_free_id(self, label);
            Fabric_Label_set_text_id(label, 0);
            Fabric_TextStore_delete_text(ts, text_id);
        }
        *status = stat;
        
    }



    return next_id;
}

error_t Fabric_LabelStore_remove_label(LabelStore *self, labelid_t label_id) {
    // TODO: This is a stub
    return FABRIC_OK;
}

#endif