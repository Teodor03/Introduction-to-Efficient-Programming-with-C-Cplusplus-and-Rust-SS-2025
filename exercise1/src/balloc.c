#define _GNU_SOURCE
#include "balloc.h"

/*!
 * \file
 * \brief implement the bitmap allocator interface
 */

#include <stddef.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#define BITMAP_CHUNK_MIN_SIZE 16
#define BITMAP_CHUNK_MAX_SIZE 512

#define BALLOC_METADATA_OS_ALLOCATION 0x8000000000000000ull

typedef size_t balloc_metadata;

//Header defined.
struct bitmap_alloc *bitmap_allocators = NULL;

//Header defined.
size_t num_bitmap_allocators = 0;

size_t max_num_bitmap_allocators;

size_t bitmap_allocators_num_pages_allocated;

void init_bitmap_allocators() {
    bitmap_allocators = alloc_from_os(BITMAP_PAGE_SIZE);
    num_bitmap_allocators = 0;
    max_num_bitmap_allocators = BITMAP_PAGE_SIZE / sizeof(struct bitmap_alloc);
    bitmap_allocators_num_pages_allocated = 1;
}

void expand_bitmap_allocators() {
    void * new_memory = alloc_from_os(BITMAP_PAGE_SIZE * bitmap_allocators_num_pages_allocated * 2);
    memcpy(new_memory, bitmap_allocators, BITMAP_PAGE_SIZE * bitmap_allocators_num_pages_allocated);
    munmap(bitmap_allocators, BITMAP_PAGE_SIZE * bitmap_allocators_num_pages_allocated);
    bitmap_allocators = new_memory;
    bitmap_allocators_num_pages_allocated *= 2;
    max_num_bitmap_allocators = (bitmap_allocators_num_pages_allocated * BITMAP_PAGE_SIZE) / sizeof(struct bitmap_alloc);
}

void add_bitmap_allocator(size_t chunk_size) {
    if(num_bitmap_allocators == max_num_bitmap_allocators)
        expand_bitmap_allocators();
    struct bitmap_alloc * to_add = bitmap_allocators + num_bitmap_allocators;
    to_add->chunk_size = chunk_size;
    to_add->occupied_areas = 0ull;
    to_add->memory = alloc_from_os(chunk_size * NUM_BITS_SIZE_T);
    num_bitmap_allocators++;
}

void *allocate_bitmap_chunk(size_t chunk_size, size_t *found_index) {
    for(size_t i = 0; i < num_bitmap_allocators; i++) {
        if(bitmap_allocators[i].chunk_size != chunk_size)
            continue;
        void *memory = alloc_block_in_bitmap(bitmap_allocators + i);
        if(!memory)
            continue;
        *found_index = i;
        return memory;
    }
    add_bitmap_allocator(chunk_size);
    *found_index = num_bitmap_allocators - 1;
    return alloc_block_in_bitmap(bitmap_allocators + (num_bitmap_allocators - 1));
}





void balloc_setup(void) {
    init_bitmap_allocators();
    for(size_t i = BITMAP_CHUNK_MIN_SIZE; i <= BITMAP_CHUNK_MAX_SIZE; i *= 2) {
        add_bitmap_allocator(i);
    }
}

void balloc_teardown(void) {
    for(size_t i = 0; i < num_bitmap_allocators; i++) {
        if(bitmap_allocators[i].chunk_size)
            munmap(bitmap_allocators[i].memory, bitmap_allocators[i].chunk_size * NUM_BITS_SIZE_T);
    }
    munmap(bitmap_allocators, BITMAP_PAGE_SIZE * bitmap_allocators_num_pages_allocated);
    bitmap_allocators = NULL;
    num_bitmap_allocators = 0ull;
}

void *alloc_block_in_bitmap(struct bitmap_alloc *alloc) {
    size_t pos = __builtin_ffsll(~(alloc->occupied_areas));
    if(!pos)
        return NULL;
    pos--;
    alloc->occupied_areas |= 1llu<<pos;
    return ((char *) alloc->memory) + pos * alloc->chunk_size;
}

#define BLOCK_IN_BITMAP_DEALLOCATED 1
#define BLOCK_IN_BITMAP_NOT_DEALLOCATED 0

int success_dealloc_block_in_bitmap;

void dealloc_block_in_bitmap(struct bitmap_alloc *alloc, void *object) {
    if(object < alloc->memory) {
        success_dealloc_block_in_bitmap = BLOCK_IN_BITMAP_NOT_DEALLOCATED;
        return;
    }
    size_t dist = (size_t) ((char *) object - (char *) alloc->memory);
    if(dist >= MEMORY_SIZE_CHUNK(alloc->chunk_size)) { 
        success_dealloc_block_in_bitmap = BLOCK_IN_BITMAP_NOT_DEALLOCATED;
        return;
    }
    if((dist % alloc->chunk_size) != 0) {
        success_dealloc_block_in_bitmap = BLOCK_IN_BITMAP_NOT_DEALLOCATED;
        return;
    }
    alloc->occupied_areas &= ~(1llu << (dist / alloc->chunk_size));
    success_dealloc_block_in_bitmap = BLOCK_IN_BITMAP_DEALLOCATED;
}

void *alloc_from_os(size_t size) {
    void *return_value = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(return_value == MAP_FAILED)
        return NULL;
    return return_value;
}

void dealloc_to_os(void *memory, size_t size) {
    munmap(memory, size);
}

void *alloc(size_t size) {
    if(!size)
        return NULL;
    size += sizeof(balloc_metadata);
    size_t chunk_size = (size_t)1 << (NUM_BITS_SIZE_T - __builtin_clzl(size - 1));
    void * memory;
    if(chunk_size > BITMAP_CHUNK_MAX_SIZE) {
        //OS Allocation
        memory = alloc_from_os(size);
        *((balloc_metadata *) memory) = BALLOC_METADATA_OS_ALLOCATION | size;
        return ((char *) memory) + sizeof(balloc_metadata);
    }
    //BitMap Allocation
    if(chunk_size < BITMAP_CHUNK_MIN_SIZE)
        chunk_size = BITMAP_CHUNK_MIN_SIZE;
    size_t found_index;
    memory = allocate_bitmap_chunk(chunk_size, &found_index);
    *((balloc_metadata *) memory) = (~BALLOC_METADATA_OS_ALLOCATION) & found_index;
    return ((char *) memory) + sizeof(balloc_metadata);
}

void dealloc(void *memory) {
    if(!memory)
        return;
    void *real_start = ((balloc_metadata *) memory) - 1;
    balloc_metadata hat = *((balloc_metadata *) real_start);
    if(hat & BALLOC_METADATA_OS_ALLOCATION) {
        //OS Allocation
        munmap(real_start, (~BALLOC_METADATA_OS_ALLOCATION) & hat);
        return;
    }
    //BitMap Allocation
    dealloc_block_in_bitmap(bitmap_allocators + hat, real_start);
}
