#ifndef MEMORY_H
#define MEMORY_H

/* Memory manager using custom heap */

#define VRAM_SIZE  65536   /* 64 KB RAM */

void  mem_init(void);
void *my_alloc(int size);
void  my_dealloc(void *ptr);
void  my_memset(void *ptr, int val, int size);
void  my_memcpy(void *dst, const void *src, int size);

#endif
