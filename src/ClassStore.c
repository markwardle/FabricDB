/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 31, 2015
 */

#ifndef _FABRIC_CLASSSTORE_C__
#define _FABRIC_CLASSSTORE_C__

#define FABRIC_CLASSSTORE_HEADER_SIZE 6

#include "Internal.h"

/**
 * The Class Store is the component of the graph that has the responsibility
 * of managing the storage of Class objects.
 *
 * For many of the functions to work, it is assumed that the Class Store
 * is embedded inside a Graph object.
 *
 * For a detailed description of Class objects, see the accompanying
 * Class.c file.
 */
typedef struct ClassStore {
    uint32_t offset;        // graph file offset (in bytes) for the class store
    uint16_t num_classes;   // The number of classes in the database
    uint16_t next_free_id;  // The next free id in the class store
                             // Free ids are stored as a link list
    uint16_t last_free_id;  // The last free id should point to a block that's never been used
    uint32_t size;          // the size of the class store in bytes
    EntityMap *cache;        // A cache of classes; Includes at least all classes in changed
    IdSet *changed;          // A set of classes that have changed since last write
} ClassStore;


/**
 * Initializes a class store object
 *
 * Args:
 *      self: The Class Store object being initialized.  Its offset should
 *            already be set by the Graph
 */
error_t Fabric_ClassStore_init(ClassStore *self) {
    error_t status;
    Graph *graph = Fabric_ClassStore_get_graph(self);
    self->size = Fabric_Graph_get_label_store_offset(graph) - self->offset;
    self->num_classes = Fabric_Graph_read_uint16(graph, self->offset);
    self->next_free_id = Fabric_Graph_read_uint16(graph, self->offset + 2);
    self->last_free_id = Fabric_Graph_read_uint16(graph, self->offset + 4);

    self->cache = Fabric_EntityMap_new(&status);
    if (FABRIC_OK != status) {
        return status;
    }
    self->changed = Fabric_IdSet_new(&status);
    return status;
}

/**
 * Internal function for calculating the file offset of a class
 */
static inline uint32_t Fabric_ClassStore__get_id_offset(ClassStore *self, classid_t class_id){
    return (class_id - 1) * FABRIC_CLASS_STORAGE_SIZE + self->offset + FABRIC_CLASSSTORE_HEADER_SIZE;
}

/**
 * Writes updates to the class store to file.
 *
 * Args:
 *      self: The class store whose data is being persisted
 *
 * Returns:
 *      FABRIC_OK if the write is successful
 *      A memory error if there is not enough memory to complete the action
 *      FABRIC_CLASSTORE_NEEDS_RESIZE if the class store must be resized
 *          before it can complete the write
 */
error_t Fabric_ClassStore_flush(ClassStore *self) {
    // If the changed set is empty, no changes have occurred
    // and no write is needed
    if (Fabric_IdSet_is_empty(self->changed)){
        return FABRIC_OK;
    }

    error_t status;
    uint32_t *changed_ids = Fabric_IdSet_to_array(self->changed, &status);
    uint32_t changed_id;
    if (FABRIC_OK != status) {
        return status;
    }
    int num_ids = Fabric_IdSet_get_count(self->changed);
    int i;
    Class *changed_class;
    uint16_t max_id = (self->size - FABRIC_CLASSSTORE_HEADER_SIZE ) / FABRIC_CLASS_STORAGE_SIZE;
    uint8_t write_data[FABRIC_CLASS_STORAGE_SIZE];
    uint32_t offset;
    Graph *graph = Fabric_ClassStore_get_graph(self);

    for (i= 0; i < num_ids; i++) {
        changed_id = changed_ids[i];
        // If the class has been changed, it MUST be in the cache
        changed_class = Fabric_EntityMap_get(self->cache, changed_id);
        // We have to make sure we have room for the class.
        // If not we return an error code indicating the need
        // for the class store to be resized.  This is a relatively
        // expensive operation and should be avoided.
        // It is possible that the class is not currently in use which
        // would allow the resize to be deferred.  However, there is not yet
        // a way to ensure that the class store's next id value will remain
        // accurate without writing it.
        if (changed_id > max_id) {
            return FABRIC_CLASSSTORE_NEEDS_RESIZE;
        }

        offset = Fabric_ClassStore__get_id_offset(self, changed_id);

        Fabric_Class_load_bytes(changed_class, write_data);
        // Write the class to file
        Fabric_Graph_write_bytes(graph, write_data, FABRIC_CLASS_STORAGE_SIZE, offset);
        // Remove the id from the set
        // If a resize is triggered later, we won't have to write this class again
        Fabric_IdSet_remove(self->changed, changed_id);
    }

    // Write the class store's header
    Fabric_Graph_write_uint16(graph, self->num_classes, self->offset);
    Fabric_Graph_write_uint16(graph, self->next_free_id, self->offset + 2);
    Fabric_Graph_write_uint16(Graph, self->next_free_id, self->offset + 4);

    return FABRIC_OK;
}

/**
 * Internal function for getting and updating the next id for a class
 */
static
classid_t Fabric_ClassStore__next_id(ClassStore *self) {
    uint32_t next_id_offset;
    Graph *g;
    classid_t next_free_id = self->next_free_id;
    if (self->next_free_id == self->last_free_id) {
        self->next_free_id++;
        self->last_free_id++;
    } else if (Fabric_EntityMap_has_key(self->cache, next_free_id)) {
        // The next free id for a free class is stored at the parent class offset
        self->next_free_id = Fabric_Class_get_parent_class_id(Fabric_EntityMap_get(self->cache, next_free_id));
        // TODO is there a case when such a class would not be in the changed set?
    }
    else {
        g = Fabric_ClassStore_get_graph(self);
        next_id_offset = Fabric_ClassStore__get_id_offset(self, next_free_id);
        self->next_free_id = Fabric_Graph_read_uint16(g, next_id_offset + sizeof(labelid_t));
    }
    return next_free_id;
}

static
inline
void Fabric_ClassStore__add_free_id(ClassStore *self, Class *class) {
    Fabric_Class_set_parent_class_id(class, self->next_free_id);
    self->next_free_id = Fabric_Class_get_id(class);
}

/**
 * Get's a class by id from the store
 *
 * Args:
 *      self: A graph's class store
 *      class_id: The id of the class being retrieved
 *      status: A pointer to where an error can be indicated
 *
 * Returns: The class object with the specified id, or NULL on failure
 */
Class *Fabric_ClassStore_get_class(ClassStore *self, classid_t class_id, error_t *status) {
    Class *c = Fabric_EntityMap_get(self->cache, class_id);
    uint32_t offset;
    uint8_t data[FABRIC_CLASS_STORAGE_SIZE];
    Graph *g;
    *status = FABRIC_OK;
    // If we have a cached copy of the class, we should use it.
    // The primary reason is that a cached copy may have been
    // updated, which may not yet be reflected in the database
    // file.  Secondary reasons are that it saves time and maintains
    // a canonical version of the class object.
    if (!c) {
        offset = Fabric_ClassStore__get_id_offset(self, class_id);
        if (offset > self->size + self->offset || class_id < 1) {
            *status = FABRIC_CLASSSTORE_INVALID_ID;
            return c;
        }
        g = Fabric_ClassStore_get_graph(self);
        Fabric_Graph_read_bytes(g, data, FABRIC_CLASS_STORAGE_SIZE, offset);
        c = Fabric_Class_new(class_id, status);
        if (*status != FABRIC_OK) {
            // c should be NULL here
            return c;
        }
        *status = Fabric_Class_init(c, data);
        // TODO decide what classes should be cached
        Fabric_EntityMap_set(self->cache, class_id, c);
    }

    // make sure the class is valid
    // if it's label is 0 then it is not in use
    if (!Fabric_Class_is_in_use(c)) {
        *status = FABRIC_CLASS_DOESNT_EXIST;
        // Fabric_Class_destroy(c);
        return NULL;
    }

    return c;
}

/**
 * Get's a class with a given name from the store
 *
 * Args:
 *      self: A graph's class store
 *      name: The name of the class being retrieved
 *      status: A pointer to where an error can be indicated
 *
 * Returns: The class object with the specified name, or NULL on failure
 */
Class *Fabric_ClassStore_get_class_by_name(ClassStore *self, text_t name, error_t *status) {
    Graph *g = Fabric_ClassStore_get_graph(self);
    IndexStore *is = Fabric_Graph_get_index_store(g);
    ClassIndex *ci = Fabric_IndexStore_get_class_index(is, status);
    if (FABRIC_OK != status) {
        return NULL;
    }

    classid_t id = Fabric_ClassIndex_get_class_id(ci, name, status);
    if (FABRIC_OK != status) {
        return NULL;
    } else if (id == 0) {
        *status = FABRIC_CLASS_DOESNT_EXIST;
        return NULL;
    }

    return Fabric_ClassStore_get_class(self, id, status);
}

/**
 * Creates a new class in the graph
 *
 * Args:
 *      self: The graph's class store
 *      extends: The class the one being created extends
 *      name: The name of the class, must be unique
 *      is_abstract: Whether or not the new class should be made abstract
 *      status: A pointer to where an error can be indicated
 *
 * Returns: The newly created class or NULL on failure
 */
Class *Fabric_ClassStore_create_class(
    ClassStore *self,
    Class *extends,
    text_t name,
    bool_t is_abstract,
    error_t *status) {

    Class *c;
    classid_t class_id;
    classid_t parent_class_id;
    labelid_t label_id;
    indexid_t index_id;
    LabelStore *ls;
    IndexStore *is;
    ClassIndex *ci;
    Graph *g;
    error_t stat;


    // Make sure the class doesn't already exist
    if(NULL != Fabric_ClassStore_get_class_by_name(self, name, status)) {
        *status = FABRIC_DUPLICATE_CLASSNAME;
        return NULL;
    }
    // Make sure we're getting the right error code from the lookup
    if (FABRIC_CLASS_DOESNT_EXIST != *status) {
        return NULL;
    }

    ci = Fabric_IndexStore_get_class_index(is, status);
    if (FABRIC_OK != *status) {
        return NULL;
    }

    class_id = Fabric_ClassStore__next_id(self);
    c = Fabric_Class_new(class_id, status);
    if (FABRIC_OK != *status) {
        Fabric_ClassStore__add_free_id(self, c);
        return NULL;
    }

    // Create or get an existing label for the class
    g = Fabric_ClassStore_get_graph(self);
    ls = Fabric_Graph_get_label_store(g);
    label_id = Fabric_LabelStore_add_label(ls, name, status);
    if (FABRIC_OK != *status) {
        Fabric_ClassStore__add_free_id(self, c);
        return NULL;
    }

    // Create the id index for the new class unless the class is abstract
    if (!is_abstract) {
        is = Fabric_Graph_get_index_store(g);
        index_id = Fabric_IndexStore_create_id_index(is, class_id, status);
        if (FABRIC_OK != *status) {
            Fabric_ClassStore__add_free_id(self, c);
            // The label should be cached, so an error shouldn't occur here
            Fabric_LabelStore_remove_label(ls, label_id);
            return NULL;
        }
    } else {
        index_id = 0;
    }

    parent_class_id = Fabric_Class_get_id(extends);

    Fabric_Class_set_label_id(c, label_id);
    Fabric_Class_set_parent_class_id(c, parent_class_id);
    Fabric_Class_set_next_child_class_id(c, Fabric_Class_get_first_child_class_id(extends));
    Fabric_Class_set_first_child_class_id(extends, class_id);
    Fabric_Class_set_first_index_id(c, index_id);
    Fabric_Class_set_count(c, 0);
    Fabric_Class_set_incrementer(c, 1);
    Fabric_Class_set_is_abstract(c, is_abstract);

    // Make sure we are keeping track of he new class as well as the parent class
    if (FABRIC_OK != (stat = Fabric_EntityMap_set(self->cache, class_id, c)) ||
        FABRIC_OK != (stat = Fabric_EntityMap_set(self->cache, parent_class_id, extends))||
        FABRIC_OK != (stat = Fabric_IdSet_add(self->changed, class_id)) ||
        FABRIC_OK != (stat = Fabric_IdSet_add(self->changed, parent_class_id))||
        FABRIC_OK != (stat = Fabric_IndexStore_add_class_to_index(is, c))) {

        *status = stat;

        Fabric_LabelStore_remove_label(ls, label_id);
        if (!is_abstract) {
            Fabric_IndexStore_delete_id_index(is, index_id);
        }
        Fabric_Class_set_first_child_class_id(extends, Fabric_Class_get_next_child_class_id(c));
        // Mark the class as not in useand add its id back into the pot
        Fabric_Class_set_label_id(c, 0);
        Fabric_ClassStore__add_free_id(self, c);
        return NULL;
    }

    self->num_classes++;
    return c;
}

/**
 * Deletes a class in the database
 *
 * Important:
 *      This method should not be used to delete a class that has existing
 *      vertices.
 *
 *      This method should also not be used to delete a class that has child classes.
 *
 * Args:
 *      self: A graph's class store
 *      class: The class that is being deleted from the graph
 *
 * Returns:
 *      FABRIC_OK if all goes well, otherwise any of the following error codes:
 *      FABRIC_CANT_DELETE_CLASS_HAS_CHILDREN if the class has child classes
 *      FABRIC_CANT_DELETE_CLASS_HAS_MEMBERS if the class has vertex classes
 *      A memory error
 */
error_t Fabric_ClassStore_delete_class(ClassStore *self, Class *c) {
    Class *parent_class;
    Class *child_class = NULL;
    classid_t class_id;
    Index *index;
    labelid_t label_id;

    LabelStore *ls;
    IndexStore *is;
    Graph *g;
    error_t status;

    if (!Fabric_Class_is_in_use(c)) {
        //TODO should this be an error instead???
        return FABRIC_OK;
    }

    if (Fabric_Class_has_child_classes(c)) {
        return FABRIC_CANT_DELETE_CLASS_HAS_CHILDREN;
    }

    if (Fabric_Class_has_members(c)) {
        return FABRIC_CANT_DELETE_CLASS_HAS_MEMBERS;
    }

    g = Fabric_ClassStore_get_graph(self);
    class_id = Fabric_Class_get_id(c);

    parent_class = Fabric_Class_get_parent_class(c, g, &status);
    if (FABRIC_OK != status) {
        return status;
    }

    // update the class hierarchy
    if (Fabric_Class_get_first_child_class_id(parent_class) == class_id) {
        Fabric_Class_set_first_child_class_id(parent_class, Fabric_Class_get_next_child_class_id(c));
        status = Fabric_EntityMap_set(self->cache, Fabric_Class_get_id(parent_class), child_class);
        if (FABRIC_OK == status) {
            status = Fabric_IdSet_add(self->changed, Fabric_Class_get_id(parent_class));
        }
    } else {
        child_class = Fabric_Class_get_first_child_class(parent_class, g, &status);
        if (FABRIC_OK != status) {
            return status;
        }
        while (Fabric_Class_get_next_child_class_id(child_class) != class_id) {
            child_class = Fabric_Class_get_next_child_class(child_class, g, &status);
            if (FABRIC_OK != status) {
                return status;
            }
        }
        Fabric_Class_set_next_child_class_id(child_class, class_id);
        status = Fabric_EntityMap_set(self->cache, Fabric_Class_get_id(child_class), child_class);
        if (FABRIC_OK == status) {
            status = Fabric_IdSet_add(self->changed, Fabric_Class_get_id(child_class));
        }
    }

    // Update the label and class index store and track all the changes
    label_id = Fabric_Class_get_label_id(c);
    ls = Fabric_Graph_get_label_store(g);
    is = Fabric_Graph_get_index_store(g);
    if (
        FABRIC_OK != status ||
        FABRIC_OK != (status = Fabric_IndexStore_remove_class_from_index(is, c)) ||
        FABRIC_OK != (status = Fabric_IdSet_add(self->changed, class_id)) ||
        FABRIC_OK != (status = Fabric_EntityMap_set(self->cache, class_id, c)) ||
        FABRIC_OK != (status = Fabric_IdSet_add(self->changed, class_id)) ||
        FABRIC_OK != (status = Fabric_LabelStore_remove_label(ls, label_id))) {
        // revert changes on error
        if (child_class == NULL) {
            Fabric_Class_set_first_child_class_id(parent_class, class_id);
        } else {
            Fabric_Class_set_next_child_class_id(child_class, class_id);
        }
        Fabric_IndexStore_add_class_to_index_if_not_exists(is, c);
        return status;
    }
    Fabric_ClassStore__add_free_id(self, c);
    self->num_classes--;
    return FABRIC_OK;
}

#endif