/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 25, 2015
 * Updated: March 27, 2015
 */

#ifndef _FABRIC_DYNAMICLIST_C__
#define _FABRIC_DYNAMICLIST_C__

#include "Internal.h"

#define FABRIC_DYNAMIC_LIST_MIN_CAP 16

/**
 * A dynamic list is an generic container for holding a variable length
 * list of pointers.
 */
typedef struct DynamicList {
    int count;      // The number of items in the list
    int cap;        // The capacity of the list in
    void **list;     // Pointer to the items
} DynamicList;

/**
 * Allocates memory for a DynamicList
 *
 * This method does not initialize the list in any way
 *
 * Returns: A pointer to memory allocated for a dynamic list object
 *          may return null which indicates a memory error which can
 *          be checked with Fabric_memerrno()
 */
DynamicList *Fabric_DynamicList_allocate() {
    return Fabric_memalloc(sizeof(DynamicList));
}

/**
 * Deallocates memory for a DynamicList
 *
 * Args:
 *      list: The list object being deallocated
 */
void *Fabric_DynamicList_deallocate(DynamicList *list) {
    Fabric_memfree(list, sizeof(DynamicList));
}

/**
 * Initializes a Dynamic List with a specified capacity
 *
 * Args:
 *      self: A pointer to memory for the dynamic list
 *      capacity: The number of items to reserve space for.
 *                If less than FABRIC_DYNAMIC_LIST_MIN_CAP, the
 *                minimum capacity will be used instaed
 *
 * Returns: FABRIC_OK on success, other error number on failure
 */
error_t Fabric_DynamicList_init_with_capacity(DynamicList *self, int capacity) {
    self->cap = capacity > FABRIC_DYNAMIC_LIST_MIN_CAP ? capacity : FABRIC_DYNAMIC_LIST_MIN_CAP;
    self->count = 0;
    self->list = (void**)Fabric_memalloc(self->cap * sizeof(void*));
    if (self->list == NULL) {
        return Fabric_memerrno();
    }
    return FABRIC_OK;
}

/**
 * Initializes a dynamic list with the default capacity
 *
 * Args:
 *      self: A pointer to memory for the dynamic list
 *
 * Returns: FABRIC_OK on success, other error number on failure
 */
error_t Fabric_DynamicList_init(DynamicList *self) {
    return Fabric_DynamicList_init_with_capacity(self, FABRIC_DYNAMIC_LIST_MIN_CAP);
}

/**
 * Deinitializes the dynamic list's dynamically allocated memory.
 *
 * This method should always be called when done with a dynamic list.
 * Also, the dynamic list should not be used after this method has been called.
 *
 * Args:
 *      self: A pointer to the dynamic list that is being deinitialized
 */
void Fabric_DynamicList_deinit(DynamicList *self) {
    Fabric_memfree(self->list, self->cap * sizeof(void*));
}

/**
 * Creates a new dynamic list with the default capacity
 *
 * Args:
 *      status: A pointer to where an error will be stored
 *
 * Returns: A fully initialized dynamic list object or
 *          NULL in case of an error.
 *          status will be set to FABRIC_OK if no errors,
 *          otherwise another error number.
 */
DynamicList *Fabric_DynamicList_new(error_t *status) {
    DynamicList *list = Fabric_DynamicList_allocate();
    if (NULL == list) {
        *status = Fabric_memerrno();
    } else {
        *status = Fabric_DynamicList_init(list);
        if (FABRIC_OK != *status) {
            // Always return NULL on error,
            // but make sure we clean up the memory first
            Fabric_DynamicList_deallocate(list);
            list = NULL;
        }
    }

    return list;
}

/**
 * Deinitializes and frees a Dynamic List
 */
void Fabric_DynamicList_destroy(DynamicList *self) {
    Fabric_DynamicList_deinit(self);
    Fabric_DynamicList_deallocate(self);
}

/**
 * Frees the memory of all the items held by a dynamic list.
 *
 * This should only be called when it is known that none of the items
 * the list holds will need to be used again.
 *
 * Args:
 *      self: A pointer to the dynamic list whose items are being deinitialized
 *      member_size: The size of the objects held by the list
 */
void Fabric_DynamicList_free_items(DynamicList *self, size_t member_size) {
    int i;
    for (i = 0; i < self->count; i++) {
        Fabric_memfree(self->list[i], member_size);
    }
}

/**
 * Deinitializes the dynamic list's dynamically allocated memory as well
 * as the memory of all of its items.
 *
 * This method should only be called when done with a dynamic list and
 * when its items will definitely not need to be used again.
 *
 * This method is the same as calling Fabric_DynamicList_free_items(2)
 * followed by Fabric_DynamicList_deinit(1)
 *
 * Args:
 *      self: A pointer to the dynamic list whose items and self are being deinitialized
 *      member_size: The size of the objects held by the list
 */
void Fabric_DynamicList_free_items_and_deinit(DynamicList *self, size_t member_size) {
    Fabric_DynamicList_free_items(self, member_size);
    Fabric_DynamicList_deinit(self);
}

/**
 * Changes the capacity of the array list.
 *
 * If new_capacity is less than FABRIC_DYNAMIC_LIST_MIN_CAP, it does nothing.
 *
 * Args:
 *      self: The dynamic list object being resized
 *      new_capacity: The capacity to resize the dynamic list to
 *
 * Returns: FABRIC_OK on success, other error number on failure.
 */
error_t Fabric_DynamicList_change_capacity(DynamicList *self, int new_capacity) {
    if (new_capacity < FABRIC_DYNAMIC_LIST_MIN_CAP) {
        return FABRIC_OK;
    }
    void* new_list = Fabric_memrealloc(self->list, new_capacity * sizeof(void*), self->cap * sizeof(void*));
    if (new_list == NULL) {
        return Fabric_memerrno();
    }
    self->list = new_list;
    self->cap = new_capacity;
    return FABRIC_OK;
}

/**
 * Retrieves the pointer stored at a specified position in the ArrayList
 *
 * Args:
 *      self: The list the item is being retrieved from
 *      pos: The index of the item being retrieved
 *
 * Returns: A pointer to a stored item or NULL if index out of bounds
 */
void* Fabric_DynamicList_at(DynamicList *self, int pos) {
    if (pos >= self->count || pos < 0) {
        return NULL;
    }
    return self->list[pos];
}

/**
 * Appends an item at the end of the list, resizing the underlying data
 * if necessary.
 *
 * If an error occurs the list will be unchanged from its original state
 *
 * Args:
 *      self: The dynamic list being appended to
 *      item: The pointer being added to the list
 *
 * Returns: FABRIC_OK on success, other error number on failure
 */
error_t Fabric_DynamicList_append(DynamicList *self, void *item) {
    int status;
    if (self->count >= self->cap) {
        status = Fabric_DynamicList_change_capacity(self, self->cap * 2);
        if (FABRIC_OK != status) {
            return status;
        }
    }

    self->list[self->count] = item;
    self->count++;

    return FABRIC_OK;
}

/**
 * Removes the last item from the list
 *
 * If an error occurs the list will be unchanged from its original state
 *
 * Args:
 *      self: The list from which the last item is being removed
 *
 * Returns: FABRIC_OK on success or other error number when an error occurs
 */
error_t Fabric_DynamicList_remove_last(DynamicList *self) {
    int status;

    if (self->count <= 0) {
        return FABRIC_OK;
    }

    self->count--;

    // Reduce size automatically
    //if (self->cap / 4 <= self->count && self->cap >= FABRIC_DYNAMIC_LIST_MIN_CAP * 2) {
    //    status = Fabric_DynamicList_change_capacity(self, self->cap / 2);
    //   if (FABRIC_OK != status) {
    //        self->count++;   // on error no change should occur
    //    }
    //}

    return FABRIC_OK;
}

/**
 * Gets the number of elements in the dynamic list
 */
int Fabric_DynamicList_count(DynamicList *self) {
    return self->count;
}

/**
 * Gets the capacity of a dynamic list
 */
int Fabric_DynamicList_capacity(DynamicList *self) {
    return self->cap;
}

#endif