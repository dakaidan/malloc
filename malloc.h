#ifndef MALLOC_MALLOC_H
#define MALLOC_MALLOC_H

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct block_meta {
    size_t size;
    struct block_meta *next;
    int free;
    int magic; // For debugging only. TODO: remove this in non-debug mode.
};

// Align on 8 bytes boundary.
#define ALIGNMENT 8

#define META_SIZE sizeof(struct block_meta)

void *global_base = NULL;

struct block_meta *find_free_block(struct block_meta **last, size_t size);

struct block_meta *request_space(struct block_meta *last, size_t size);

void *malloc(size_t size);

struct block_meta *get_block_ptr(void *ptr);

void free(void *ptr);

void *realloc(void *ptr, size_t size);

void *calloc(size_t nelem, size_t elsize);

#endif
