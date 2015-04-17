/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 27, 2015
 */

#ifndef _FABRIC_CLASS_C__
#define _FABRIC_CLASS_C__

#include "Internal.h"

/**
 * A class is the primary method that Fabric provides for organizing
 * the types of vertices in a graph.
 *
 * Each class must have a label which is unique in the graph.
 *
 * Classes are organized in a hierarchical fashion similar to how most
 * object-oriented languages implement single-inheritance.  The root
 * class is the "Vertex" class which all other classes must be a descendant
 * of.
 *
 * A fundamental rule of for classes is that the hierarchy must create
 * an acyclic graph.  In other words, a class cannot be an ancestor of itself.
 *
 * Queries for a class include members of all its child classes.  As such,
 * a query against the "Vertex" class will include all the vertices in the
 * graph.
 *
 * A class can be declared to be abstract, meaning it can not have any
 * members.  Instead, such a class serves as a base class for descendent
 * classes.
 *
 * The "Vertex" class is not abstract by default, though it can be set as
 * such if it does not have any members at the time of creation.
 *
 * A class may have any number of child classes, but only one parent class.
 * A future version of this library may include a more complete type system
 * which will allow multiple inheritance.
 *
 * The database stores each class in 21 bytes
 *
 * +----+----+----+----+----+----+----+----+----+----+----+----+
 * |label_id           | p_id    | fc_id   | nc_id   | fi_id   |
 * +----+----+----+----+----+----+----+----+----+----+----+----+
 * |count              | ia | incrementer       |
 * +----+----+----+----+----+----+----+----+----+
 */
typedef struct Class {
    classid_t id;                // The internal id of this class
    labelid_t label_id;          // The id of the label entity for this class
    classid_t parent_id;         // the id of the parent class for this class
    classid_t first_child_id;    // The id of the first child class for this class
    classid_t next_child_id;     // The id of the next child class for this class's parent
    indexid_t first_index_id;    // The id of the first index for this class
    uint32_t count;             // The number of members of this specific class
    uint8_t is_abstract;        // Whether or not this is an abstract class
    uint32_t incrementer;       // An automatically incremented value that can be used
                                 // for creating unique ids
} Class;

/**
 * Gets a class's id
 */
classid_t Fabric_Class_get_id(Class *self) {
    return self->id;
}

/**
 * Sets a class's id
 */
void Fabric_Class_set_id(Class *self, classid_t id) {
    self->id = id;
}

/**
 * Creates a heap allocated class object and sets its id
 *
 * Args:
 *      id: The internal id of the class object being created
 *      status: A pointer to where an error can be indicated
 *
 * Returns: A heap allocated class object that is NOT initialized
 *          or NULL if there is a memory error
 */
Class *Fabric_Class_new(classid_t id, error_t *status) {
    Class *c = Fabric_memalloc(sizeof(Class));
    if (c == NULL) {
        *status = Fabric_memerrno();
        return c;
    }

    c->id = id;
    *status = FABRIC_OK;
    return c;
}

/**
 * Frees a class object
 */
void Fabric_Class_destroy(Class *self) {
    Fabric_memfree(self, sizeof(Class));
}

/**
 * Initializes a class from a block of data
 *
 * Args:
 *      self: A class object with its id initialized
 *      data: The bytes used to store the class object in the database (21 bytes)
 *
 * Returns: FABRIC_ON success or a different error number if an error occurs
 */
error_t Fabric_Class_init(Class *self, uint8_t *data) {
    // A Class's id must be set externally
    if (self->id < 1) {
        return FABRIC_CLASS_INVALID_ID;
    }

    self->label_id = betoh32(*(labelid_t*)data);
    self->parent_id = betoh16(*(classid_t*)(data+4));
    self->first_child_id = betoh16(*(classid_t*)(data+6));
    self->next_child_id = betoh16(*(classid_t*)(data+8));
    self->first_index_id = betoh16(*(indexid_t*)(data+10));
    self->count = betoh32(*(uint32_t*)(data+12));
    self->is_abstract = *(data+16);
    self->incrementer = betoh32(*(uint32_t*)(data+17));

    return FABRIC_OK;
}

void Fabric_Class_load_bytes(Class *self, uint8_t *dest) {
    *((labelid_t*)dest) = self->label_id;
    *((classid_t*)(dest + 4)) = self->parent_id;
    *((classid_t*)(dest + 6)) = self->first_child_id;
    *((classid_t*)(dest + 8)) = self->next_child_id;
    *((indexid_t*)(dest + 10)) = self->first_index_id;
    *((uint32_t*)(dest + 12)) = self->count;
    *(dest + 16) = self->is_abstract;
    *((uint32_t*)(dest + 17)) = self->incrementer;
}

/**
 * Gets the label id for a class
 */
labelid_t Fabric_Class_get_label_id(Class *self) {
    return self->label_id;
}

/**
 * Sets the label id for a class
 */
void Fabric_Class_set_label_id(Class *self, labelid_t label_id) {
    self->label_id = label_id;
}

/**
 * Returns whether or not this class is in use.
 *
 * A class is marked as not in use by setting its label_id to 0;
 */
bool_t Fabric_Class_is_in_use(Class *self) {
    return self->label_id != 0;
}

/**
 * Gets the label for a class
 *
 * Args:
 *      self: The class object whose label is being retrieved
 *      graph: The graph object the class belongs to
 *      out: A pointer to where the result will be stored
 *
 * Returns:
 *     The label object for the class
 *     Status will be set to FABRIC_OK on success, other error number on failure
 */
Label* Fabric_Class_get_label(Class *self, Graph *graph, error_t *status) {
    LabelStore *ls = Fabric_Graph_get_label_store(graph);
    return Fabric_LabelStore_get_label(ls, self->label_id, status);
}

/**
 * Get's a class object's parent class's id
 */
classid_t Fabric_Class_get_parent_class_id(Class *self) {
    return self->parent_id;
}

/**
 * Sets a class's parent class id
 */
void Fabric_Class_set_parent_class_id(Class *self, classid_t parent_class_id) {
    self->parent_id = parent_class_id;
}

/**
 * Gets the parent class of a class
 *
 * Args:
 *      self: The class object whose parent class is being retrieved
 *      graph: The graph object the class belongs to
 *      out: A pointer to where the result will be stored
 *
 * Returns:
 *      the parent class for this class
 *      status will be set to FABRIC_OK on success, other error number on failure
 */
Class* Fabric_Class_get_parent_class(Class *self, Graph *graph, error_t *status) {
    ClassStore *cs = Fabric_Graph_get_class_store(graph);
    return Fabric_ClassStore_get_class(cs, self->parent_id, status);
}

/**
 * Determines if a class has any child classes
 *
 * Args:
 *      self: The Class being tested for children
 *
 * Returns: TRUE if the class has at least one child class, FALSE if not
 */
bool_t Fabric_Class_has_child_classes(Class *self) {
    return self->first_child_id != 0;
}

/**
 * Gets the id of a class's first child class
 */
classid_t Fabric_Class_get_first_child_class_id(Class *self) {
    return self->first_child_id;
}

/**
 * Sets the class's first child class id
 */
void Fabric_Class_set_first_child_class_id(Class *self, classid_t child_class_id) {
    self->first_child_id = child_class_id;
}

/**
 * Gets the first child of a class
 *
 * Args:
 *      self: The class object whose child class is being retrieved
 *      graph: The graph object the class belongs to
 *      out: A pointer to where the result will be stored
 *
 * Returns:
 *     The first child class of this class
 *     status will be set to FABRIC_OK on success, other error number on failure
 */
Class* Fabric_Class_get_first_child_class(Class *self, Graph *graph, error_t *status) {
    ClassStore *cs = Fabric_Graph_get_class_store(graph);
    return Fabric_ClassStore_get_class(cs, self->first_child_id, status);
}

/**
 * Determines if a class has a next child class
 *
 * Args:
 *      self: The Class being tested for a next child
 *
 * Returns: TRUE if the class has a next child, FALSE if not
 */
bool_t Fabric_Class_has_next_child_class(Class *self) {
    return self->next_child_id != 0;
}

/**
 * Get's the id of a class's parent class's next child class
 */
classid_t Fabric_Class_get_next_child_class_id(Class *self) {
    return self->next_child_id;
}

void Fabric_Class_set_next_child_class_id(Class *self, classid_t next_child_id) {
    self->next_child_id = next_child_id;
}

/**
 * Gets the next child of the class's parent class
 *
 * Fabric_Class_has_next_child_class(1) should be used to make sure
 * There is a next child to retrieve.
 *
 * Args:
 *      self: The class object whose parent's next child class is being retrieved
 *      graph: The graph object the class belongs to
 *      out: A pointer to where the result will be stored
 *
 * Returns:
 *      This class's parent class's next child class
 *      status will be set to FABRIC_OK on success, other error number on failure
 */
Class* Fabric_Class_get_next_child_class(Class *self, Graph *graph, error_t *status) {
    ClassStore *cs = Fabric_Graph_get_class_store(graph);
    return Fabric_ClassStore_get_class(cs, self->next_child_id, status);
}

/**
 * Loads the descendent classes of a class to a specified depth
 *
 * The memory for the classes are dynamically allocated and should be
 * freed with Fabric_memfree(2) when they are no longer needed.
 * This is true even if this method returns an error code.
 *
 * Args:
 *      self: The class object whose child classes are being retrieved
 *      graph: The graph object the class belongs to
 *      list: A pointer to dynamic list that will store the result
 *      depth: The number of levels of children to load
 *             1 indicates only immediate children
 *             less than 1 indicates all levels
 *
 * Returns: FABRIC_OK on success, other error number on failure
 */
error_t Fabric_Class_load_descendent_classes(Class *self, Graph *graph, DynamicList *list, int depth) {
    error_t status;
    Class *current_child, *next_child;

    // make sure we have a child class
    if (!Fabric_Class_has_child_classes(self)) {
        return FABRIC_OK;
    }

    current_child = Fabric_Class_get_first_child_class(self, graph, &status);
    if (FABRIC_OK != status) {
        return status;
    }

    while (NULL != current_child) {
        status = Fabric_DynamicList_append(list, current_child);
        if (FABRIC_OK != status) {
            return status;
        }

        // Add all of the child's descendents to the list
        if (depth != 1) {
            status = Fabric_Class_load_descendent_classes(current_child, graph, list, depth - 1);
            if (FABRIC_OK != status) {
                return status;
            }
        }

        if (Fabric_Class_has_next_child_class(current_child)) {
            // get the next child class
            next_child = Fabric_Class_get_next_child_class(current_child, graph, &status);
            if (FABRIC_OK != status) {
                return status;
            }
            current_child = next_child;
        } else {
            // let the loop know we are done
            current_child = NULL;
        }
    }

    return FABRIC_OK;
}

/**
 * Loads all of a classes child classes into a list
 *
 * The memory for the dynamic list is heap allocated, which means
 * it should be cleaned up once it is no longer needed.
 *
 * Args:
 *      self: The class object whose child classes are being retrieved
 *      graph: The graph object the class belongs to
 *      list: A pointer to dynamic list that will store the result
 *
 * Returns:
 *      A dynamic list object containing all the class's immediate child classes
 *      status is set to FABRIC_OK on success, other error number on error
 */
DynamicList *Fabric_Class_get_child_classes(Class *self, Graph *graph, error_t *status) {
    DynamicList *list = Fabric_DynamicList_new(status);
    if (NULL != list) {
        *status = Fabric_Class_load_descendent_classes(self, graph, list, 1);
    }
    return list;
}

/**
 * Loads all of a classes descendent classes into a list
 *
 * The memory for the dynamic list is heap allocated, which means
 * it should be cleaned up once it is no longer needed.
 *
 * Args:
 *      self: The class object whose child classes are being retrieved
 *      graph: The graph object the class belongs to
 *      list: A pointer to dynamic list that will store the result
 *
 * Returns:
 *      A dynamic list of all the class's descendent classes
 *      status is set to FABRIC_OK on success, other error number on failure
 */
DynamicList *Fabric_Class_get_descendent_classes(Class *self, Graph *graph, error_t *status) {
    DynamicList *list = Fabric_DynamicList_new(status);
    if (list != NULL) {
        *status = Fabric_Class_load_descendent_classes(self, graph, list, 0);
    }
    return list;
}

/**
 * Gets the id of the classes first index.
 */
indexid_t Fabric_Class_get_first_index_id(Class *self) {
    return self->first_index_id;
}

void Fabric_Class_set_first_index_id(Class *self, indexid_t first_index_id) {
    self->first_index_id = first_index_id;
}

/**
 * Gets the first index for the class.
 *
 * The first index is always an id store, unless the class is abstract
 *
 * Args:
 *      self: The class object that is having its first index retrieved
 *      graph: The graph the class belongs to
 *      status: A pointer to a location to store an error code
 *
 * Returns:
 *      The first index of for the class
 *      status will be set to FABRIC_OK on success, other error number on failure
 */
Index *Fabric_Class_get_first_index(Class *self, Graph *graph, error_t *status) {
    IndexStore *is = Fabric_Graph_get_index_store(graph);
    return Fabric_IndexStore_get_index(is, self->first_index_id, status);
}

/**
 * Gets the number of vertices in a specific class (not including its
 * descendent classes)
 */
uint32_t Fabric_Class_get_count(Class *self) {
    return self->count;
}

/**
 * Sets a class's count
 */
void Fabric_Class_set_count(Class *self, uint32_t count) {
    self->count = count;
}

/**
 * Returns whether or not the class has any vertices that belong to it
 */
bool_t Fabric_Class_has_members(Class *self) {
    return self->count > 0;
}

/**
 * Gets the number of vertices belonging to this class and all of its
 * descendent classes.
 *
 * Args:
 *      self: The class whose members are being counted
 *      graph: The graph object the class belongs to
 *      status: A pointer to where any errors will be indicated
 *
 * Return:
 *      The count of all vertices of this class or of this class's descendents
 *      status is set to FABRIC_OK on success, other error number on failure
 */
uint32_t Fabric_Class_get_total_count(Class *self, Graph *graph, error_t *status) {
    int i, l;
    uint32_t total = self->count;
    Class *child;
    DynamicList *list;

    // load all the classes descendents and add their count to total
    list = Fabric_Class_get_descendent_classes(self, graph, status);
    if (status == FABRIC_OK) {
        for (i = 0, l = Fabric_DynamicList_count(list); i < l; i++) {
            child = (Class*) Fabric_DynamicList_at(list, i);
            total += child->count;
        }
    }

    // clean up
    if (NULL != list) {
        Fabric_DynamicList_destroy(list);
    }

    return total;
}

/**
 * Returns whether or not this class is abstract
 */
bool_t Fabric_Class_is_abstract(Class *self) {
    return self->is_abstract;
}

/**
 * Sets Whether or not this class is abstract
 */
void Fabric_Class_set_is_abstract(Class *self, bool_t is_abstract) {
    self->is_abstract = is_abstract;
}

/**
 * Sets a class's incrementer to a specific value
 */
void Fabric_Class_set_incrementer(Class *self, uint32_t value) {
    self->incrementer = value;
}

/**
 * Returns and updates the next increment value for the class
 */
uint32_t Fabric_Class_increment(Class *self) {
    uint32_t result = self->incrementer;
    self->incrementer++;
    return result;
}

#endif