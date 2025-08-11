#ifndef MEMORY_INTERNAL_H
#define MEMORY_INTERNAL_H

#include "SDL.h"

#define AMPHORA_NUM_MEM_BLOCKS 0x100

typedef Uint8 AmphoraMemBlock[0x10000];

struct amphora_mem_block_metadata_t {
	unsigned short addr;
	unsigned short allocations;
};

int Amphora_InitHeap(void);
void Amphora_DestroyHeap(void);
void *Amphora_HeapAlloc(size_t size);
void *Amphora_HeapAllocFrame(size_t size);
void *Amphora_HeapRealloc(void *ptr, size_t nbytes, size_t size);
void *Amphora_HeapCalloc(size_t num, size_t size);
char *Amphora_HeapStrdup(const char *str);
void Amphora_HeapFree(void *ptr);

#endif /* MEMORY_INTERNAL_H */
