/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 28, 2015
 * Updated: March 29, 2015
 */

#ifndef _FABRIC_IDSET_C__
#define _FABRIC_IDSET_C__

#include "Internal.h"
#include "Hash.c"

#define FABRIC_IDSET_DEFAULT_CAPACITY 32
#define FABRIC_IDSET_MAXLOAD 0.6
#define FABRIC_IDSET_TOMBSTONE 0x11111111

/**
 * A set of uint32 ids.
 *
 * Currently implemented as a simple hash table.
 */
typedef struct IdSet {
    int count;              // The number of ids in the set
    int cap;                // The capacity of the underlying array
    uint32_t *ids;         // The underlying data for the set
} IdSet;


uint32_t *Fabric_IdSet_to_array(IdSet *self, error_t *status);

/**
 * Creates an id set with a specified capacity
 *
 * Args:
 *      capacity: The initial capacity the set should have, < 1 for default
 *      satus: A pointer to where any errors will be stored
 *
 * Returns: A newly allocated and initialized id set, or NULL on error
 */
IdSet *Fabric_IdSet_new_with_capacity(int capacity, error_t *status) {
    if (capacity < 1) {
        capacity = FABRIC_IDSET_DEFAULT_CAPACITY;
    }

    IdSet *set = Fabric_memalloc(sizeof(IdSet));
    if (NULL == set) {
        *status = Fabric_memerrno();
    } else {
        set->count = 0;
        set->cap = capacity;

        set->ids = Fabric_memalloc(capacity * sizeof(uint32_t));
        if (NULL == set->ids) {
            // Clean up the memory
            Fabric_memfree(set, sizeof(IdSet));
            set = NULL;
            *status = Fabric_memerrno();
        } else {
            memset(set->ids, 0, capacity * sizeof(uint32_t));
            *status = FABRIC_OK;
        }
    }

    return set;
}

/**
 * Creates an id set with the default initial capacity
 *
 * Args:
 *      status: A pointer to where an error can be set
 *
 * Returns: A newly allocated and initialized id set object
 */
IdSet *Fabric_IdSet_new(error_t *status) {
    return Fabric_IdSet_new_with_capacity(0, status);
}

/**
 * Cleans up and deinitializes an id set
 *
 * Args:
 *      self: The id set being destroyed
 */
void Fabric_IdSet_destroy(IdSet *self) {
    Fabric_memfree(self->ids, self->cap * sizeof(uint32_t));
    Fabric_memfree(self, sizeof(IdSet));
}

/**
 * Gets the number of ids in the set
 */
int Fabric_IdSet_get_count(IdSet *self) {
    return self->count;
}

/**
 * Returns TRUE if the set has no items
 */
bool_t Fabric_IdSet_is_empty(IdSet *self) {
    return self->count <= 0;
}

/**
 * Get's the current underlying capacity of the set
 */
int Fabric_IdSet_get_capacity(IdSet *self) {
    return self->cap;
}

/**
 * Returns whether or not the set has an id
 *
 * Args:
 *      self: The set being queried
 *      id: The id being tested for
 *
 * Returns: TRUE if the set contains the id, FALSE if not
 */
bool_t Fabric_IdSet_has(IdSet *self, uint32_t id) {
    uint32_t pos = hash_uint32(id) % self->cap;
    while (self->ids[pos] != 0) {
        if (self->ids[pos] == id) {
            return TRUE;
        }
        pos = (pos + 1) % self->cap;
    }
    return FALSE;
}

/**
 * Private method for adding ids to the set without performing
 * capacity or other checks.
 */
void Fabric_IdSet__add_no_checks(IdSet *self, uint32_t id) {
    uint32_t pos = hash_uint32(id) % self->cap;
    while (self->ids[pos] != 0 && self->ids[pos] != FABRIC_IDSET_TOMBSTONE) {
        pos = (pos + 1) % self->cap;
    }

    self->ids[pos] = id;
    self->count++;
}

/**
 * Private function to resize an id set
 */
error_t Fabric_IdSet__resize(IdSet *self, int new_cap) {

    uint32_t *old_data = self->ids;
    uint32_t *new_data = Fabric_memalloc(new_cap * sizeof(uint32_t));
    int l = self->cap;
    int i, status;

    if (new_data == NULL) {
        return Fabric_memerrno();
    }

    memset(new_data, 0, new_cap * sizeof(uint32_t));

    self->ids = new_data;
    self->count = 0;
    self->cap = new_cap;

    for (i = 0; i < l; i++) {
        if (old_data[i] != 0 && old_data[i] != FABRIC_IDSET_TOMBSTONE) {
            Fabric_IdSet__add_no_checks(self, old_data[i]);
        }
    }

    Fabric_memfree(old_data, l * sizeof(uint32_t));
    return FABRIC_OK;
}

/**
 * Adds an id to the set
 *
 * If an error occurs, the set should remain unchanged
 *
 * Args:
 *      self: The set that the id is being added to
 */
error_t Fabric_IdSet_add(IdSet *self, uint32_t id) {
    // Don't add a duplicate
    if (Fabric_IdSet_has(self, id)) {
        return FABRIC_OK;
    }

    error_t status;
    float load = (float)(self->count + 1) / (float)(self->cap);
    if (load > FABRIC_IDSET_MAXLOAD) {
        status = Fabric_IdSet__resize(self, self->cap * 2);
        if (FABRIC_OK != status) {
            return status;
        }
    }

    Fabric_IdSet__add_no_checks(self, id);

    return FABRIC_OK;
}

/**
 * Removes an id from a set
 *
 * Args:
 *      self: The set that is having the id removed
 *      id: The id being removed from the set
 */
void Fabric_IdSet_remove(IdSet *self, uint32_t id) {
    uint32_t pos = hash_uint32(id) % self->cap;
    while (self->ids[pos] != 0) {
        if (self->ids[pos] == id) {
            self->ids[pos] = FABRIC_IDSET_TOMBSTONE;
            self->count--;
            break;
        }
        pos = (pos + 1) % self->cap;
    }
}

/**
 * Gets the ids in the set as an unordered array
 *
 * The array is dynamically allocated and should be freed when
 * it is no longer needed.  The length of the array is equal
 * to the count of the set.
 *
 * Args:
 *      self: The set being converted to an array
 *      status: A pointer to where an error can be stored
 *
 * Returns: A heap allocated array containing the ids in the set
 */
uint32_t *Fabric_IdSet_to_array(IdSet *self, error_t *status) {
    uint32_t *array = Fabric_memalloc(sizeof(uint32_t) * self->count);
    if (NULL == array) {
        *status = Fabric_memerrno();
    } else {
        int array_pos = 0;
        int internal_pos = 0;
        int arr_l = self->count;
        for (;array_pos < arr_l; array_pos++) {
            while (self->ids[internal_pos] == 0 || self->ids[internal_pos] == FABRIC_IDSET_TOMBSTONE) {
                internal_pos++;
            }
            array[array_pos] = self->ids[internal_pos];
            internal_pos++;
        }
        *status = FABRIC_OK;
    }

    return array;
}

#endif