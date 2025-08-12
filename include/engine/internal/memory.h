#ifndef MEMORY_INTERNAL_H
#define MEMORY_INTERNAL_H

#define AMPHORA_HEAP_SIZE 0x10000
#define AMPHORA_NUM_MEM_BLOCKS 0x100
_Static_assert(AMPHORA_NUM_MEM_BLOCKS <= 0x100);

typedef uint8_t AmphoraMemBlock[AMPHORA_HEAP_SIZE];

typedef enum {
	MEM_UNASSIGNED,
	MEM_RENDERABLE,
	MEM_IMAGE,
	MEM_EMITTER,
	MEM_TILEMAPS,
	MEM_STRING,
	MEM_HASHTABLE,
	MEM_MISC,
	MEM_COUNT
} AmphoraMemBlockCategory;

struct amphora_mem_block_metadata_t {
	uint16_t addr;
	uint16_t allocations;
	AmphoraMemBlockCategory category;
};

int Amphora_InitHeap(void);
void Amphora_DestroyHeap(void);
void *Amphora_HeapAlloc(size_t size, AmphoraMemBlockCategory category);
void *Amphora_HeapAllocFrame(size_t size);
void *Amphora_HeapRealloc(void *ptr, size_t size, AmphoraMemBlockCategory category);
void *Amphora_HeapCalloc(size_t num, size_t size, AmphoraMemBlockCategory category);
char *Amphora_HeapStrdup(const char *str);
char *Amphora_HeapStrdupFrame(const char *str);
void Amphora_HeapFree(void *ptr);
void Amphora_HeapClearFrameHeap(void);

#endif /* MEMORY_INTERNAL_H */
