#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct BlockMeta {
    size_t size;
    int available;
    void* start;
    struct BlockMeta* next;
} BlockMeta;

static void* global_pool = NULL;
static BlockMeta* block_list = NULL;
static size_t pool_total_size = 0;
static pthread_mutex_t mem_lock = PTHREAD_MUTEX_INITIALIZER;  // Mutex for thread safety

void mem_init(size_t pool_size) {
    global_pool = malloc(pool_size);
    if (global_pool == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory pool\n");
        exit(EXIT_FAILURE);
    }

    block_list = (BlockMeta*)malloc(sizeof(BlockMeta));
    if (block_list == NULL) {
        free(global_pool);
        fprintf(stderr, "Error: Metadata allocation failed\n");
        exit(EXIT_FAILURE);
    }

    block_list->size = pool_size;
    block_list->available = 1;
    block_list->start = global_pool;
    block_list->next = NULL;
    pool_total_size = pool_size;
}

void* mem_alloc(size_t request_size) {
    if (request_size == 0) return NULL;

    pthread_mutex_lock(&mem_lock);

    BlockMeta* curr = block_list;
    while (curr != NULL) {
        if (curr->available && curr->size >= request_size) {
            if (curr->size >= request_size + sizeof(BlockMeta) + 1) {
                BlockMeta* split = (BlockMeta*)malloc(sizeof(BlockMeta));
                if (split == NULL) {
                    pthread_mutex_unlock(&mem_lock);
                    return NULL;
                }

                split->size = curr->size - request_size;
                split->available = 1;
                split->start = (char*)curr->start + request_size;
                split->next = curr->next;

                curr->size = request_size;
                curr->available = 0;
                curr->next = split;

                pthread_mutex_unlock(&mem_lock);
                return curr->start;
            } else {
                curr->available = 0;
                pthread_mutex_unlock(&mem_lock);
                return curr->start;
            }
        }
        curr = curr->next;
    }

    pthread_mutex_unlock(&mem_lock);
    return NULL;
}

void mem_free(void* ptr) {
    if (ptr == NULL) return;

    pthread_mutex_lock(&mem_lock);

    BlockMeta* node = block_list;
    while (node != NULL) {
        if (node->start == ptr) {
            if (node->available) {
                pthread_mutex_unlock(&mem_lock);
                return; // already free
            }

            node->available = 1;

            // Coalescing adjacent free blocks
            while (node->next && node->next->available) {
                BlockMeta* to_merge = node->next;
                node->size += to_merge->size;
                node->next = to_merge->next;
                free(to_merge);
            }

            pthread_mutex_unlock(&mem_lock);
            return;
        }
        node = node->next;
    }

    fprintf(stderr, "Warning: Tried to free untracked memory at %p\n", ptr);
    pthread_mutex_unlock(&mem_lock);
}

void* mem_resize(void* ptr, size_t new_size) {
    if (new_size == 0) return NULL;
    if (ptr == NULL) return mem_alloc(new_size);

    pthread_mutex_lock(&mem_lock);

    BlockMeta* blk = block_list;
    while (blk != NULL) {
        if (blk->start == ptr) {
            if (blk->size >= new_size) {
                pthread_mutex_unlock(&mem_lock);
                return ptr;  // existing block is big enough
            }

            pthread_mutex_unlock(&mem_lock); // Unlock before calling alloc/free

            void* new_mem = mem_alloc(new_size);
            if (new_mem != NULL) {
                memcpy(new_mem, ptr, blk->size);
                mem_free(ptr);
            }
            return new_mem;
        }
        blk = blk->next;
    }

    fprintf(stderr, "Resize failed: memory at %p not tracked\n", ptr);
    pthread_mutex_unlock(&mem_lock);
    return NULL;
}

void mem_deinit() {
    pthread_mutex_lock(&mem_lock);

    free(global_pool);
    global_pool = NULL;

    BlockMeta* current = block_list;
    while (current != NULL) {
        BlockMeta* following = current->next;
        free(current);
        current = following;
    }

    block_list = NULL;
    pool_total_size = 0;

    pthread_mutex_unlock(&mem_lock);
    pthread_mutex_destroy(&mem_lock);
}
