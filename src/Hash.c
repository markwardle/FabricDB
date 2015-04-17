/**
 * Jenkins one at a time hash
 *
 * https://en.wikipedia.org/wiki/Jenkins_hash_function
 */

#ifndef _FABRIC_HASH_C__
#define _FABRIC_HASH_C__

#include <stdint.h>

uint32_t hash(uint8_t *key, size_t len) {
    uint32_t hash, i;
    for(hash = i = 0; i < len; ++i) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

uint32_t hash_uint32(uint32_t key) {
    return hash((uint8_t*)(&key), 4);
}

#endif