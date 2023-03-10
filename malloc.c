#include "malloc.h"
#include <errno.h>

struct block_meta *find_free_block(struct block_meta **last, size_t size) {
    struct block_meta *current = global_base;
    while (current && !(current->free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

struct block_meta *request_space(struct block_meta *last, size_t size) {
    struct block_meta *block;
    block = sbrk(0);
    void *request = sbrk(size + META_SIZE + ALIGNMENT - 1);
    assert((void *) block == request); // Not thread safe.
    if (request == (void *) -1) {
        return NULL; // sbrk failed.
    }

    // align block if needed
    block = (struct block_meta *) (((size_t) block + ALIGNMENT - 1) & ~(ALIGNMENT - 1));

    if (last) { // NULL on first request.
        last->next = block;
    }
    block->size = size;
    block->next = NULL;
    block->free = 0;
    block->magic = 0x77777777;
    return block;
}

void *malloc(size_t size) {
    struct block_meta *block;

    if (size <= 0) {
        return NULL;
    }

    if (!global_base) { // First call.
        block = request_space(NULL, size);
        if (!block) {
            return NULL; // failed to get space.
        }
        global_base = block;
    } else {
        struct block_meta *last = global_base;
        block = find_free_block(&last, size);
        if (!block) { // Failed to find free block in existing space.
            block = request_space(NULL, size); // Request more space.
            if (!block) {
                return NULL;
            }
        } else { // Found free block
            // TODO: consider splitting block here.
            block->free = 0;
            block->magic = 0x77777777;
        }
    }

    return (block + 1);
}

struct block_meta *get_block_ptr(void *ptr) {
    return (struct block_meta *) ptr - 1;
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }

    // TODO: consider merging blocks once splitting blocks is implemented.
    struct block_meta *block_ptr = get_block_ptr(ptr);

    if (!block_ptr) {
        return;
    }

    assert(block_ptr->free == 0);
    assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
    block_ptr->free = 1;
    block_ptr->magic = 0x55555555;
}

void *realloc(void *ptr, size_t size) {
    if (!ptr) {
        // NULL ptr. realloc should act like malloc.
        return malloc(size);
    }

    struct block_meta *block_ptr = get_block_ptr(ptr);
    if (block_ptr->size >= size) {
        // We have enough space. Could free some once we implement split.
        return ptr;
    }

    // Need to really realloc. Malloc new space and free old space.
    // Then copy old data to new space.
    void *new_ptr;
    new_ptr = malloc(size);
    if (!new_ptr) {
        errno = ENOMEM;
        return NULL;
    }
    memcpy(new_ptr, ptr, block_ptr->size);
    free(ptr);
    return new_ptr;
}

void *calloc(size_t nelem, size_t elsize) {
    size_t size = nelem * elsize;

    if( elsize != 0 && (size / elsize != nelem || (size % elsize) != 0)) {
        return NULL;
    }

    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}