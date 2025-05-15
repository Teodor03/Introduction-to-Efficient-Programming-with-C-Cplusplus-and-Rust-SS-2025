#include "balloc.h"

/*!
 * \file
 * \brief implement the bitmap allocator interface
 */

#include <stddef.h>
#define BITS_IN_BITMAP (sizeof(size_t) * 8)

// Global array of bitmap allocators
struct bitmap_alloc *bitmap_allocators = NULL;

// Number of bitmap allocators in the global array
size_t num_bitmap_allocators = 0;

void balloc_setup(void) {
    // Optional: Place logic which should happen befor a benchmark
}
void balloc_teardown(void) {
    // Optional: Place logic which should happen after a benchmark
}

void *alloc_block_in_bitmap(struct bitmap_alloc *alloc) {
    size_t pos = __builtin_ffsll(~(alloc->occupied_areas));
    if(!pos)
        return NULL;
    pos--;
    alloc->occupied_areas |= 1llu<<pos;
    return ((char *) alloc->memory) + pos * alloc->chunk_size;
}

void dealloc_block_in_bitmap(struct bitmap_alloc *alloc, void *object) {
    if(object < alloc->memory)
        return;
    size_t dist = (size_t) ((char *) object - (char *) alloc->memory);
    if(dist >= (alloc->chunk_size * BITS_IN_BITMAP))
        return;
    if((dist % alloc->chunk_size) != 0)
        return;
    alloc->occupied_areas &= ~(1llu << (dist / alloc->chunk_size));
}

void *alloc_from_os(size_t size) {
    (void)size;
    // TODO: Implement
    return NULL;
}

void dealloc_to_os(void *memory, size_t size) {
    (void)memory;
    (void)size;
    // TODO: Implement
}

void *alloc(size_t size) {
    (void)size;
    // TODO: Implement
    return NULL;
}

void dealloc(void *memory) {
    (void)memory;
    // TODO: Implement
}
