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

#define NUM_BITMAP_CHUNK_SIZES 5

#define OS_ALLOCATION_SIZE_INDEX -1

typedef struct bitmap_allocator_position {

    int chunk_size_index;

    size_t index;

} bitmap_allocator_position;

typedef struct bitmap_allocator_storage {
    
    struct bitmap_alloc *memory;

    size_t num_pages;

    size_t size;

    size_t first_free;

} bitmap_allocator_storage;



// Not used
struct bitmap_alloc *bitmap_allocators = NULL;

// Not used
size_t num_bitmap_allocators = 0;

size_t bitmap_chunk_sizes[NUM_BITMAP_CHUNK_SIZES] = {
    8,
    16,
    32,
    64,
    128
};

bitmap_allocator_storage allocators [NUM_BITMAP_CHUNK_SIZES];


void init_bitmap_allocator_storage (bitmap_allocator_storage * storage) {
    storage->memory = alloc_from_os(BITMAP_PAGE_SIZE);
    memset(storage->memory, 0, BITMAP_PAGE_SIZE);
    storage->num_pages = 1;
    storage->size = BITMAP_PAGE_SIZE / sizeof(struct bitmap_alloc);
    storage->first_free = 0;
}

void expand_bitmap_allocator_storage (bitmap_allocator_storage * storage) {
    void *new_memory = alloc_from_os(storage->num_pages * BITMAP_PAGE_SIZE * 2);
    memset(new_memory, 0, storage->num_pages * BITMAP_PAGE_SIZE * 2);
    memcpy(new_memory, storage->memory, storage->num_pages * BITMAP_PAGE_SIZE);
    munmap(storage->memory, storage->num_pages * BITMAP_PAGE_SIZE);
    storage->memory = new_memory;
    storage->num_pages *= 2;
    storage->size = (storage->num_pages * BITMAP_PAGE_SIZE) / sizeof(struct bitmap_alloc);
}



void add_bitmap_allocator (int chunk_size_index, size_t index) {
    size_t allocator_size = MEMORY_SIZE_CHUNK(bitmap_chunk_sizes[chunk_size_index]);
    size_t num_allocators = allocator_size < BITMAP_PAGE_SIZE ? BITMAP_PAGE_SIZE / allocator_size : 1;
    if((index + num_allocators) > allocators[chunk_size_index].size)
        expand_bitmap_allocator_storage(allocators + chunk_size_index);
    void *memory_to_use = alloc_from_os(allocator_size < BITMAP_PAGE_SIZE ? BITMAP_PAGE_SIZE : allocator_size);
    for(size_t i = 0; i < num_allocators; i++) {
        struct bitmap_alloc *a = allocators[chunk_size_index].memory;
        a[index + i].memory = memory_to_use;
        a[index + i].chunk_size = bitmap_chunk_sizes[chunk_size_index];
        a[index + i].occupied_areas = 0llu;
        memory_to_use = ((char *) memory_to_use) + allocator_size;
    }
}



void balloc_setup(void) {
    //Init allocators.
    for(int i = 0; i < NUM_BITMAP_CHUNK_SIZES; i++) {
        init_bitmap_allocator_storage(allocators + i);
        add_bitmap_allocator(i, 0);
    }
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



void *alloc_bitmap_chunk(int chunk_size_index, bitmap_allocator_position * allocation_position) {
    bitmap_allocator_storage *curr_storage = allocators + chunk_size_index;
    size_t index = curr_storage->first_free;
    while(1) {
        if(index == curr_storage->size) {
            expand_bitmap_allocator_storage(curr_storage);
        }
        if(curr_storage->memory[index].memory == NULL) {
            add_bitmap_allocator(chunk_size_index, index);
        }
        void *found = alloc_block_in_bitmap(curr_storage->memory + index);
        if(found != NULL) {
            allocation_position->chunk_size_index = chunk_size_index;
            allocation_position->index = index;
            curr_storage->first_free = index;
            return found;
        }
        index++;
    }
}

void dealloc_bitmap_chunk(void *address, bitmap_allocator_position metadata) {
    struct bitmap_alloc *curr_alloc = allocators[metadata.chunk_size_index].memory + metadata.index;
    dealloc_block_in_bitmap(curr_alloc, address);
    if(success_dealloc_block_in_bitmap == BLOCK_IN_BITMAP_NOT_DEALLOCATED)
        return;
    allocators[metadata.chunk_size_index].first_free = metadata.index;
    //TODO Prune empty allocators if too many!
}

int retrieve_corresponding_chunk_size(size_t size) {
    if(size <= bitmap_chunk_sizes[0])
        return 0;
    if(size > bitmap_chunk_sizes[NUM_BITMAP_CHUNK_SIZES - 1])
        return OS_ALLOCATION_SIZE_INDEX;
    for(int i = 1; i < NUM_BITMAP_CHUNK_SIZES; i++) {
        if(size <= bitmap_chunk_sizes[i])
            return i;
    }
    return OS_ALLOCATION_SIZE_INDEX;
}

void *alloc(size_t size) {
    if(!size)
        return NULL;
    size_t total_size = size + sizeof(bitmap_allocator_position);
    bitmap_allocator_position hat;
    void * allocated_memory;
    int chunk_index = retrieve_corresponding_chunk_size(total_size);
    if(chunk_index == OS_ALLOCATION_SIZE_INDEX) {
        allocated_memory = alloc_from_os(total_size);
        hat.chunk_size_index = OS_ALLOCATION_SIZE_INDEX;
        hat.index = total_size;
    }else {
        allocated_memory = alloc_bitmap_chunk(chunk_index, &hat);
    }
    memcpy(allocated_memory, &hat, sizeof(bitmap_allocator_position));
    return ((char *) allocated_memory) + sizeof(bitmap_allocator_position);
}

void dealloc(void *memory) {
    if(!memory)
        return;
    bitmap_allocator_position *hat = (bitmap_allocator_position *) (((char *) memory) - sizeof(bitmap_allocator_position));
    if(hat ->chunk_size_index == OS_ALLOCATION_SIZE_INDEX) {
        munmap(hat, hat->index);
        return;
    }
    dealloc_bitmap_chunk(memory, *hat);
}
