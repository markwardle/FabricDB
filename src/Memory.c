/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 25, 2015
 * Updated: March 25, 2015
 */

#ifndef _FABRIC_MEMORY_C__
#define _FABRIC_MEMORY_C__

/**
 * This file exists to provide memory utilities for fabric.
 *
 * At the moment, it doesn't do anything special.  It is really just
 * a wrapper for built in c memory functions.  However, having all
 * memory functions separate allows the possibility of later updates
 * with a more sophisticated memory model (possibly a region-based
 * memory management system).
 *
 * It also allows Fabric to track its memory usage
 */

#include <stdlib.h>
#include "Internal.h"

int _fabric_mem_errno = FABRIC_OK;
size_t _fabric_mem_used;

/**
 * Initializes Fabric's memory system.
 *
 * Right now it doesn't do anything
 *
 * Returns: FABRIC_OK on success, otherwise an error code
 */
int Fabric_meminit() {
    _fabric_mem_used = 0;
    return FABRIC_OK;
}

/**
 * Allocates memory and returns a pointer to it
 *
 * Args:
 *      size: The number of bytes to allocate
 *
 * Returns: A pointer to the allocated memory on success
 *          NULL on failure with fabric_mem_errno set to a value
 *              other than FABRIC_OK
 */
void *Fabric_memalloc(size_t size) {
    _fabric_mem_used += size;
    if (size < 1) {
        return NULL;
    }
    void *ptr = malloc(size);
    if (!ptr) {
        _fabric_mem_errno = FABRIC_OUT_OF_MEMORY;
    }
    return ptr;
}

/**
 * Reallocates a block of memory
 *
 * Args:
 *      old_ptr: The current point being reallocated
 *      new_size: The number of bytes to allocate
 *      old_size: The size of the memory block pointed to by old_ptr
 *
 * Returns: A pointer to the allocated memory on success
 *          NULL on failure with fabric_mem_errno set to a value
 *              other than FABRIC_OK
 */
void *Fabric_memrealloc(void *old_ptr, size_t new_size, size_t old_size) {
    _fabric_mem_used += new_size - old_size;
    void *new_ptr = realloc(old_ptr, new_size);
    if (!new_ptr && new_size != 0) {
        _fabric_mem_errno = FABRIC_OUT_OF_MEMORY;
    }
    return new_ptr;
}

/**
 * Frees the memory pointed to by ptr
 *
 * Args:
 *      ptr: A pointer to memory being freed
 *      size: The size of the data being freed
 */
void Fabric_memfree(void* ptr, size_t size) {
    _fabric_mem_used -= size;
    free(ptr);
}

/**
 * Returns the amount of dynamically allocated memory fabric is using
 */
size_t Fabric_memused() {
    return _fabric_mem_used;
}

/**
 * Returns the fabric memory error number
 */
int Fabric_memerrno() {
    return _fabric_mem_errno;
}

#endif