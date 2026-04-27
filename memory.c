#include "memory.h"
#include "math.h"
#include <stdlib.h>   /* allowed: only for the initial sbrk-like block */

/* Virtual memory management */
static unsigned char *vram = 0;

/* Block header for memory chunks */
typedef struct BlockHeader {
    int  size;
    int  free;
    struct BlockHeader *next;
} BlockHeader;

#define HEADER_SIZE  ((int)sizeof(BlockHeader))

static BlockHeader *heap_head = 0;

void mem_init(void) {
    vram = (unsigned char *)malloc(VRAM_SIZE);

    /* Initialize pool */
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
            /* Split block if there's enough room */
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
    return 0;
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
