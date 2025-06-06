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

#define AGGRESIVE_OPTIMIZATIONS

#ifdef AGGRESIVE_OPTIMIZATIONS
    #define REDUCE_MUNMAP
    #define ONE_CHUNK_SIZE
#endif

#ifdef ONE_CHUNK_SIZE
    #define BITMAP_CHUNK_SIZE_TOTAL 64ull
    #define BITMAP_CHUNK_MIN_SIZE 6
    #define BITMAP_CHUNK_MAX_SIZE 6
    #define INITIAL_NUMBER_BITMAP_ALLOCATORS_PER_SIZE 4
#else
    #define BITMAP_CHUNK_MIN_SIZE 6
    #define BITMAP_CHUNK_MAX_SIZE 11
    #define INITIAL_NUMBER_BITMAP_ALLOCATORS_PER_SIZE 1
#endif

#define DYNAMIC_ARRAY_INITIAL_NUMBER_PAGES 16


#define BALLOC_METADATA_OS_ALLOCATION 0x8000000000000000ull

typedef size_t balloc_metadata;

typedef struct _stack {

    size_t* mem;

    size_t num_elements;

    size_t max_num_elements;

} _stack;

struct bitmap_alloc *bitmap_allocators = NULL;

size_t num_bitmap_allocators = 0;

size_t max_num_bitmap_allocators;

size_t bitmap_allocators_num_pages_allocated;

_stack free_bitmaps [BITMAP_CHUNK_MAX_SIZE - BITMAP_CHUNK_MIN_SIZE + 1];

void init_bitmap_allocators() {
    bitmap_allocators = alloc_from_os(BITMAP_PAGE_SIZE * DYNAMIC_ARRAY_INITIAL_NUMBER_PAGES);
    num_bitmap_allocators = 0;
    max_num_bitmap_allocators = (BITMAP_PAGE_SIZE * DYNAMIC_ARRAY_INITIAL_NUMBER_PAGES) / sizeof(struct bitmap_alloc);
    bitmap_allocators_num_pages_allocated = DYNAMIC_ARRAY_INITIAL_NUMBER_PAGES;
}

void expand_bitmap_allocators() {
    void * new_memory = alloc_from_os(BITMAP_PAGE_SIZE * bitmap_allocators_num_pages_allocated * 2);
    memcpy(new_memory, bitmap_allocators, BITMAP_PAGE_SIZE * bitmap_allocators_num_pages_allocated);
    #ifndef REDUCE_MUNMAP
        munmap(bitmap_allocators, BITMAP_PAGE_SIZE * bitmap_allocators_num_pages_allocated);
    #endif
    bitmap_allocators = new_memory;
    bitmap_allocators_num_pages_allocated *= 2;
    max_num_bitmap_allocators = (bitmap_allocators_num_pages_allocated * BITMAP_PAGE_SIZE) / sizeof(struct bitmap_alloc);
}

void init_stack(_stack* s) {
    s->mem = alloc_from_os(BITMAP_PAGE_SIZE * DYNAMIC_ARRAY_INITIAL_NUMBER_PAGES);
    s->num_elements = 0;
    s->max_num_elements = BITMAP_PAGE_SIZE * DYNAMIC_ARRAY_INITIAL_NUMBER_PAGES / sizeof(size_t);
}

void expand_stack(_stack* s) {
    void * new_memory = alloc_from_os(s->max_num_elements * sizeof(size_t) * 2);
    memcpy(new_memory, s->mem, s->max_num_elements * sizeof(size_t));
    #ifndef REDUCE_MUNMAP
        munmap(s->mem, s->max_num_elements * sizeof(size_t));
    #endif
    s->mem = new_memory;
    s->max_num_elements *= 2;
}

#ifdef ONE_CHUNK_SIZE
    void add_bitmap_allocator() {
        if(num_bitmap_allocators == max_num_bitmap_allocators)
            expand_bitmap_allocators();
        bitmap_allocators[num_bitmap_allocators].chunk_size = BITMAP_CHUNK_SIZE_TOTAL;
        bitmap_allocators[num_bitmap_allocators].occupied_areas = 0ull;
        bitmap_allocators[num_bitmap_allocators].memory = mmap(NULL, BITMAP_CHUNK_SIZE_TOTAL * NUM_BITS_SIZE_T, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        free_bitmaps->mem[free_bitmaps->num_elements] = num_bitmap_allocators;
        free_bitmaps->num_elements++;
        num_bitmap_allocators++;
    }
#else
    void add_bitmap_allocator(int chunk_size_index) {
        if(num_bitmap_allocators == max_num_bitmap_allocators)
            expand_bitmap_allocators();
    
        size_t chunk_size = 1ull << (chunk_size_index + BITMAP_CHUNK_MIN_SIZE);
        struct bitmap_alloc * to_add = bitmap_allocators + num_bitmap_allocators;
        to_add->chunk_size = chunk_size;
        to_add->occupied_areas = 0ull;
        to_add->memory = alloc_from_os(chunk_size * NUM_BITS_SIZE_T);
        num_bitmap_allocators++;

        free_bitmaps[chunk_size_index].mem[free_bitmaps[chunk_size_index].num_elements] = num_bitmap_allocators - 1;
        free_bitmaps[chunk_size_index].num_elements++;
    }
#endif


#ifdef ONE_CHUNK_SIZE
    void balloc_setup(void) {
        init_bitmap_allocators();
        init_stack(free_bitmaps);
        for(int j = 0; j < INITIAL_NUMBER_BITMAP_ALLOCATORS_PER_SIZE; j++) {
            add_bitmap_allocator();
        }
    }
#else
    void balloc_setup(void) {
        init_bitmap_allocators();
        for(int i = 0; i < (BITMAP_CHUNK_MAX_SIZE - BITMAP_CHUNK_MIN_SIZE + 1); i++) {
            init_stack(free_bitmaps + i);
            for(int j = 0; j < INITIAL_NUMBER_BITMAP_ALLOCATORS_PER_SIZE; j++) {
                add_bitmap_allocator(i);
            }
        }
    }
#endif

#ifdef ONE_CHUNK_SIZE
    void balloc_teardown(void) {
        for(size_t i = 0; i < num_bitmap_allocators; i++) {
            munmap(bitmap_allocators[i].memory, BITMAP_CHUNK_SIZE_TOTAL * NUM_BITS_SIZE_T);
        }
        munmap(bitmap_allocators, BITMAP_PAGE_SIZE * bitmap_allocators_num_pages_allocated);
        #ifndef REDUCE_MUNMAP
            munmap(free_bitmaps->mem, free_bitmaps->max_num_elements * sizeof(size_t));
        #endif
        bitmap_allocators = NULL;
        num_bitmap_allocators = 0ull;
    }
#else
    void balloc_teardown(void) {
        for(size_t i = 0; i < num_bitmap_allocators; i++) {
            if(bitmap_allocators[i].chunk_size)
                munmap(bitmap_allocators[i].memory, bitmap_allocators[i].chunk_size * NUM_BITS_SIZE_T);
        }
        munmap(bitmap_allocators, BITMAP_PAGE_SIZE * bitmap_allocators_num_pages_allocated);
        #ifndef REDUCE_MUNMAP
            for(int i = 0; i < (BITMAP_CHUNK_MAX_SIZE - BITMAP_CHUNK_MIN_SIZE + 1); i++) {
                munmap(free_bitmaps[i].mem, free_bitmaps[i].max_num_elements * sizeof(size_t));
            }
        #endif
        bitmap_allocators = NULL;
        num_bitmap_allocators = 0ull;
    }
#endif

//Unused in final allocator!
void *alloc_block_in_bitmap(struct bitmap_alloc *alloc) {
    size_t pos = __builtin_ffsll(~(alloc->occupied_areas));
    if(!pos)
        return NULL;
    pos--;
    alloc->occupied_areas |= 1llu<<pos;
    return ((char *) alloc->memory) + pos * alloc->chunk_size;
}

//Unused in final allocator!
void dealloc_block_in_bitmap(struct bitmap_alloc *alloc, void *object) {
    if(object < alloc->memory)
        return;
    size_t dist = (size_t) ((char *) object - (char *) alloc->memory);
    if(dist >= MEMORY_SIZE_CHUNK(alloc->chunk_size))
        return;
    if((dist % alloc->chunk_size) != 0)
        return;
    alloc->occupied_areas &= ~(1llu << (dist / alloc->chunk_size));
}

//Unused in final allocator!
void *alloc_from_os(size_t size) {
    void *return_value = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if(return_value == MAP_FAILED)
        return NULL;
    return return_value;
}

//Unused in final allocator!
void dealloc_to_os(void *memory, size_t size) {
    munmap(memory, size);
}

#ifdef ONE_CHUNK_SIZE
void *alloc(size_t size) {
    if(!size)
        return NULL;
    size += sizeof(balloc_metadata);

    if(size > BITMAP_CHUNK_SIZE_TOTAL) {
        //OS Allocation
        void *memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        *((balloc_metadata *) memory) = BALLOC_METADATA_OS_ALLOCATION | size;
        return ((char *) memory) + sizeof(balloc_metadata);
    }

    //BitMap Allocation
    while(1) {
        if(!free_bitmaps->num_elements) {
            //Init a new bitmap allocator.
            add_bitmap_allocator();
            bitmap_allocators[num_bitmap_allocators - 1].occupied_areas = 1llu;
            *((balloc_metadata *) bitmap_allocators[num_bitmap_allocators - 1].memory) = (~BALLOC_METADATA_OS_ALLOCATION) & (num_bitmap_allocators - 1);
            return ((char *) bitmap_allocators[num_bitmap_allocators - 1].memory) + sizeof(balloc_metadata);
        }

        size_t curr_allocator_index = free_bitmaps->mem[free_bitmaps->num_elements - 1];
        if(!(~(bitmap_allocators[curr_allocator_index].occupied_areas))) {
            (free_bitmaps->num_elements)--;
            continue;
        }
        size_t pos_in_bitmap_allocator = __builtin_ffsll(~(bitmap_allocators[curr_allocator_index].occupied_areas)) - 1;
        bitmap_allocators[curr_allocator_index].occupied_areas |= 1llu << pos_in_bitmap_allocator;
        if(!(~(bitmap_allocators[curr_allocator_index].occupied_areas))) {
            (free_bitmaps->num_elements)--;
        }
        void *memory = ((char *) bitmap_allocators[curr_allocator_index].memory) + pos_in_bitmap_allocator * BITMAP_CHUNK_SIZE_TOTAL;
        *((balloc_metadata *) memory) = (~BALLOC_METADATA_OS_ALLOCATION) & curr_allocator_index;
        return ((char *) memory) + sizeof(balloc_metadata);
    }
}
#else
void *alloc(size_t size) {
    if(!size)
        return NULL;
    size += sizeof(balloc_metadata);
    int chunk_size_index = NUM_BITS_SIZE_T - __builtin_clzl(size - 1);
    void * memory;

    if(chunk_size_index > BITMAP_CHUNK_MAX_SIZE) {
        //OS Allocation
        memory = alloc_from_os(size);
        *((balloc_metadata *) memory) = BALLOC_METADATA_OS_ALLOCATION | size;
        return ((char *) memory) + sizeof(balloc_metadata);
    }

    //BitMap Allocation
    chunk_size_index = chunk_size_index < BITMAP_CHUNK_MIN_SIZE ? 0 : chunk_size_index - BITMAP_CHUNK_MIN_SIZE;
    _stack *free_bitmap = free_bitmaps + chunk_size_index;

    while(1) {
        if(!free_bitmap->num_elements) {
            //Init a new bitmap allocator.
            add_bitmap_allocator(chunk_size_index);
            bitmap_allocators[num_bitmap_allocators - 1].occupied_areas = 1llu;
            *((balloc_metadata *) bitmap_allocators[num_bitmap_allocators - 1].memory) = (~BALLOC_METADATA_OS_ALLOCATION) & (num_bitmap_allocators - 1);
            return ((char *) bitmap_allocators[num_bitmap_allocators - 1].memory) + sizeof(balloc_metadata);
        }

        size_t curr_allocator_index = free_bitmap->mem[free_bitmap->num_elements - 1];
        if(!(~(bitmap_allocators[curr_allocator_index].occupied_areas))) {
            (free_bitmap->num_elements)--;
            continue;
        }
        size_t pos_in_bitmap_allocator = __builtin_ffsll(~(bitmap_allocators[curr_allocator_index].occupied_areas)) - 1;
        bitmap_allocators[curr_allocator_index].occupied_areas |= 1llu << pos_in_bitmap_allocator;
        if(!(~(bitmap_allocators[curr_allocator_index].occupied_areas))) {
            (free_bitmap->num_elements)--;
        }
        memory = ((char *) bitmap_allocators[curr_allocator_index].memory) + pos_in_bitmap_allocator * bitmap_allocators[curr_allocator_index].chunk_size;
        *((balloc_metadata *) memory) = (~BALLOC_METADATA_OS_ALLOCATION) & curr_allocator_index;
        return ((char *) memory) + sizeof(balloc_metadata);
    }
}
#endif

#ifdef ONE_CHUNK_SIZE
void dealloc(void *memory) {
    if(!memory)
        return;
    void *real_start = ((balloc_metadata *) memory) - 1;
    balloc_metadata hat = *((balloc_metadata *) real_start);
    if(hat & BALLOC_METADATA_OS_ALLOCATION) {
        //OS Deallocation
        munmap(real_start, (~BALLOC_METADATA_OS_ALLOCATION) & hat);
        return;
    }
    //BitMap Deallocation
    bitmap_allocators[hat].occupied_areas &= ~(1llu << (((size_t) ((char *) real_start - (char *) bitmap_allocators[hat].memory)) / BITMAP_CHUNK_SIZE_TOTAL));
    if(free_bitmaps->num_elements == free_bitmaps->max_num_elements)
        expand_stack(free_bitmaps);
    free_bitmaps->mem[free_bitmaps->num_elements] = hat;
    (free_bitmaps->num_elements)++;
}
#else
void dealloc(void *memory) {
    if(!memory)
        return;
    void *real_start = ((balloc_metadata *) memory) - 1;
    balloc_metadata hat = *((balloc_metadata *) real_start);
    if(hat & BALLOC_METADATA_OS_ALLOCATION) {
        //OS Deallocation
        munmap(real_start, (~BALLOC_METADATA_OS_ALLOCATION) & hat);
        return;
    }
    //BitMap Deallocation
    bitmap_allocators[hat].occupied_areas &= ~(1llu << (((size_t) ((char *) real_start - (char *) bitmap_allocators[hat].memory)) / bitmap_allocators[hat].chunk_size));
    int chunk_size_index = NUM_BITS_SIZE_T - BITMAP_CHUNK_MIN_SIZE - 1 - __builtin_clzl(bitmap_allocators[hat].chunk_size);
    if(free_bitmaps[chunk_size_index].num_elements == free_bitmaps[chunk_size_index].max_num_elements)
        expand_stack(free_bitmaps + chunk_size_index);
    free_bitmaps[chunk_size_index].mem[free_bitmaps[chunk_size_index].num_elements] = hat;
    (free_bitmaps[chunk_size_index].num_elements)++;
}
#endif

