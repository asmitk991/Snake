#ifndef MEMORY_H
#define MEMORY_H

/* memory manager - using our own heap */

#define VRAM_SIZE  65536   /* 64 KB virtual RAM */

void  mem_init(void);               /* must call once before any alloc */
void *my_alloc(int size);           /* allocate 'size' bytes, returns ptr or 0 */
void  my_dealloc(void *ptr);        /* free previously allocated block */
void  my_memset(void *ptr, int val, int size);  /* fill memory */
void  my_memcpy(void *dst, const void *src, int size);

#endif
