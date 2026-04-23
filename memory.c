#include "memory.h"
#include "math.h"
#include <stdlib.h>   /* allowed: only for the initial sbrk-like block */

/*
 * This is our fake RAM. We grab one big chunk from the system at the start,
 * then manage it ourselves so we don't have to call real malloc anymore.
 */

static unsigned char *vram = 0;

/* this sits before every piece of memory we give out */
typedef struct BlockHeader {
    int  size;          /* how big this chunk is */
    int  free;          /* 1 if it's empty, 0 if used */
    struct BlockHeader *next;
} BlockHeader;

#define HEADER_SIZE  ((int)sizeof(BlockHeader))

static BlockHeader *heap_head = 0;  /* start of free-list */

void mem_init(void) {
    /* Allocate the one big pool via stdlib (the Hardware Abstraction Exception) */
    vram = (unsigned char *)malloc(VRAM_SIZE);

    /* Initialise the entire pool as a single free block */
    heap_head = (BlockHeader *)vram;
    heap_head->size = VRAM_SIZE - HEADER_SIZE;
    heap_head->free = 1;
    heap_head->next = 0;
}

/* First-fit allocator */
void *my_alloc(int size) {
    if (size <= 0 || !heap_head) return 0;

    BlockHeader *cur = heap_head;
    while (cur) {
        if (cur->free && cur->size >= size) {
            /* Split the block if there's enough room for a new header + 1 byte */
            int leftover = cur->size - size - HEADER_SIZE;
            if (leftover > 0) {
                BlockHeader *newblk = (BlockHeader *)((unsigned char *)cur + HEADER_SIZE + size);
                newblk->size = leftover;
                newblk->free = 1;
                newblk->next = cur->next;
                cur->next    = newblk;
                cur->size    = size;
            }
            cur->free = 0;
            return (void *)((unsigned char *)cur + HEADER_SIZE);
        }
        cur = cur->next;
    }
    return 0;  /* out of virtual memory */
}

/* Mark block free and merge adjacent free blocks (coalescing) */
void my_dealloc(void *ptr) {
    if (!ptr) return;

    BlockHeader *blk = (BlockHeader *)((unsigned char *)ptr - HEADER_SIZE);
    blk->free = 1;

    /* Coalesce consecutive free blocks */
    BlockHeader *cur = heap_head;
    while (cur && cur->next) {
        if (cur->free && cur->next->free) {
            cur->size += HEADER_SIZE + cur->next->size;
            cur->next  = cur->next->next;
        } else {
            cur = cur->next;
        }
    }
}

void my_memset(void *ptr, int val, int size) {
    unsigned char *p = (unsigned char *)ptr;
    for (int i = 0; i < size; i++) p[i] = (unsigned char)val;
}

void my_memcpy(void *dst, const void *src, int size) {
    unsigned char       *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    for (int i = 0; i < size; i++) d[i] = s[i];
}
