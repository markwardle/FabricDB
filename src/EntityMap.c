/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 28, 2015
 * Updated: March 29, 2015
 */

#ifndef _FABRIC_ENTITY_MAP_C__
#define _FABRIC_ENTITY_MAP_C__

#include "Internal.h"
#include "Hash.c"

#define FABRIC_ENTITYMAP_DEFAULT_CAPACITY 32
#define FABRIC_ENTITYMAP_MAXLOAD 0.6
#define FABRIC_ENTITYMAP_TOMBSTONE 0x11111111

typedef struct EnityMapEntry {
    uint32_t key;
    void* entity;
} EntityMapEntry;

/**
 * A map of uint32 ids to void* pointers
 *
 * Currently implemented as a simple hash table.
 */
typedef struct EntityMap {
    int count;                  // The number of entries in the map
    int cap;                    // The capacity of the underlying array
    EntityMapEntry *entries;    // The underlying data for the map
} EntityMap;

/**
 * Creates an entity map with a specified capacity
 *
 * Args:
 *      capacity: The initial capacity the map should have, < 1 for default
 *      satus: A pointer to where any errors will be stored
 *
 * Returns: A newly allocated and initialized entity map, or NULL on error
 */
EntityMap *Fabric_EntityMap_new_with_capacity(int capacity, error_t *status) {
    if (capacity < 1) {
        capacity = FABRIC_ENTITYMAP_DEFAULT_CAPACITY;
    }

    EntityMap *map = Fabric_memalloc(sizeof(EntityMap));
    if (NULL == map) {
        *status = Fabric_memerrno();
    } else {
        map->count = 0;
        map->cap = capacity;

        map->entries = Fabric_memalloc(capacity * sizeof(EntityMapEntry));
        if (NULL == map->entries) {
            // Clean up the memory
            Fabric_memfree(map, sizeof(EntityMapEntry));
            map = NULL;
            *status = Fabric_memerrno();
        } else {
            memset(map->entries, 0, capacity * sizeof(EntityMapEntry));
            *status = FABRIC_OK;
        }
    }

    return map;
}

/**
 * Creates an entity map with the default initial capacity
 *
 * Args:
 *      status: A pointer to where an error can be set
 *
 * Returns: A newly allocated and initialized entity map object
 */
EntityMap *Fabric_EntityMap_new(error_t *status) {
    return Fabric_EntityMap_new_with_capacity(0, status);
}

/**
 * Cleans up and deinitializes an entity map
 *
 * Args:
 *      self: The entity map being destroyed
 */
void Fabric_EntityMap_destroy(EntityMap *self) {
    Fabric_memfree(self->entries, self->cap * sizeof(EntityMapEntry));
    Fabric_memfree(self, sizeof(EntityMap));
}

/**
 * Gets the number of entries in an entity map
 */
int Fabric_EntityMap_get_count(EntityMap *self) {
    return self->count;
}

/**
 * Get's the current underlying capacity of an entity map
 */
int Fabric_EntityMap_get_capacity(EntityMap *self) {
    return self->cap;
}

uint32_t Fabric_EntityMap__pos_of_key(EntityMap *self, uint32_t key) {
    uint32_t pos = hash_uint32(key) % self->cap;
    while (self->entries[pos].key != 0) {
        if (self->entries[pos].key == key) {
            return pos;
        }
        pos = (pos + 1) % self->cap;
    }

    return FABRIC_ENTITYMAP_TOMBSTONE;
}

/**
 * Returns whether or not an entity map has an entry with a given key
 *
 * Args:
 *      self: The map being queried
 *      id: The id being tested for
 *
 * Returns: TRUE if the set contains the id, FALSE if not
 */
bool_t Fabric_EntityMap_has_key(EntityMap *self, uint32_t key) {
    return Fabric_EntityMap__pos_of_key(self, key) != FABRIC_ENTITYMAP_TOMBSTONE;
}

/**
 * Private method for adding ids to an entity map without performing
 * capacity or duplicate checks.
 */
void Fabric_EntityMap__add_no_checks(EntityMap *self, EntityMapEntry entry) {
    int pos = hash_uint32(entry.key) % self->cap;
    while (self->entries[pos].key != 0 && self->entries[pos].key != FABRIC_ENTITYMAP_TOMBSTONE) {
        pos = (pos + 1) % self->cap;
    }

    self->entries[pos] = entry;
    self->count++;
}

/**
 * Private function to resize an entity map
 */
error_t Fabric_EntityMap__resize(EntityMap *self, int new_cap) {

    EntityMapEntry *old_data = self->entries;
    EntityMapEntry *new_data = Fabric_memalloc(new_cap * sizeof(EntityMapEntry));
    int l = self->cap;
    int i, status;

    if (new_data == NULL) {
        return Fabric_memerrno();
    }

    memset(new_data, 0, new_cap * sizeof(EntityMapEntry));

    self->entries = new_data;
    self->count = 0;
    self->cap = new_cap;

    for (i = 0; i < l; i++) {
        if (old_data[i].key != 0 && old_data[i].key != FABRIC_ENTITYMAP_TOMBSTONE) {
            Fabric_EntityMap__add_no_checks(self, old_data[i]);
        }
    }

    Fabric_memfree(old_data, l * sizeof(EntityMapEntry));
    return FABRIC_OK;
}

/**
 * Adds or updates a key value pair in an entity map
 *
 * If an error occurs, the map should remain unchanged
 *
 * Args:
 *      self: The map that the new entry is being added to
 *
 * Returns: FABRIC_OK on success, other error code on failure
 */
error_t Fabric_EntityMap_set(EntityMap *self, uint32_t key, void* entity) {
    error_t status;
    EntityMapEntry entry = {key, entity};
    uint32_t pos = Fabric_EntityMap__pos_of_key(self, key);
    float load;

    if (pos != FABRIC_ENTITYMAP_TOMBSTONE) {
        self->entries[pos].entity = entity;
        return FABRIC_OK;
    }

    load = (float)(self->count + 1) / (float)(self->cap);

    if (load > FABRIC_ENTITYMAP_MAXLOAD) {
        status = Fabric_EntityMap__resize(self, self->cap * 2);
        if (FABRIC_OK != status) {
            return status;
        }
    }

    Fabric_EntityMap__add_no_checks(self, entry);

    return FABRIC_OK;
}

/**
 * Retrieves a value for a key in the map
 *
 * Args:
 *      self: The map the value is being retrieved from
 *      key: The key for the value that is being retrieved
 *
 * Returns: The value associated with the key, or NULL if it doesn't exist
 */
void* Fabric_EntityMap_get(EntityMap *self, uint32_t key) {
    uint32_t pos = Fabric_EntityMap__pos_of_key(self, key);
    if (pos == FABRIC_ENTITYMAP_TOMBSTONE) {
        return NULL;
    }
    return self->entries[pos].entity;
}

/**
 * Unsets a key value pair in an entity map
 *
 * Args:
 *      self: The map that is having the entry removed
 *      key: The key of the entry being removed
 */
void Fabric_EntityMap_unset(EntityMap *self, uint32_t key) {
    uint32_t pos = Fabric_EntityMap__pos_of_key(self, key);
    if (pos != FABRIC_ENTITYMAP_TOMBSTONE) {
        self->entries[pos].key = FABRIC_ENTITYMAP_TOMBSTONE;
        self->entries[pos].entity = NULL;
        self->count--;
    }
}

#endif