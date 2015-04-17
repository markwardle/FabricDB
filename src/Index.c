/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 25, 2015
 * Updated: March 29, 2015
 */

#ifndef _FABRIC_INDEX_C__
#define _FABRIC_INDEX_C__

#include "Internal.h"

/**
 * An index is a redundant data store which speeds up the lookup of
 * other entities.
 *
 * Indices come in multiple types.
 *
 * Every class has at least one index which keeps track of the ids of
 * all the vertices that belong to it.
 *
 * 0x00 not in use
 * 0x01 class index         indexes class's by their name
 * 0x02 label index         indexes labels by their text value
 * 0x03 vertex index        indexes vertices by their label and start and end classes
 * 0x04 id index            indexes the vertices that belong to a class
 * 0x05 property index      indexes the vertices in a class by a property
 */
typedef struct Index {
    indexid_t id;               // Internal id of index
    uint8_t type;              // The type of index this is
} Index;

typedef struct UnusedIndex {
    Index base;
    indexid_t next_unused_index;
} UnusedIndex;

typedef struct ClassIndex {
    Index base;
    classid_t count;
} ClassIndex;

/**
 * Return's the id of a class with a given name
 *
 * Args:
 *      self: The class index
 *      name: The name of the class being searched for
 *      status: A pointer to where an error can be stored
 *
 * Returns: The id of the class being searched for or 0 if not found or error occurs
 */
classid_t Fabric_ClassIndex_get_class_id(ClassIndex *self, text_t name, error_t *status) {
    // TODO this is a stub
    *status = FABRIC_OK;
    return 0;
}

#endif