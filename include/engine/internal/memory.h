#ifndef MEMORY_INTERNAL_H
#define MEMORY_INTERNAL_H

#include "SDL.h"

#define AMPHORA_NUM_MEM_BLOCKS 0x100

typedef char AmphoraMemBlock[0x10000];

struct amphora_mem_block_metadata {
	unsigned short addr;
	unsigned short allocations;
};

int Amphora_InitHeap(void);
void Amphora_DestroyHeap(void);

#endif /* MEMORY_INTERNAL_H */
