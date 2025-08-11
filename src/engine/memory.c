#ifdef __APPLE__
#include <sys/mman.h>
#else
#include <stdlib.h>
#endif

#include "engine/internal/error.h"
#include "engine/internal/memory.h"

/* File-scoped variables */
static AmphoraMemBlock *amphora_heap;

int
Amphora_InitHeap(void) {
#if defined(__APPLE__) || defined(__linux__)
	amphora_heap = mmap(NULL, sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
#else
	amphora_heap = malloc(sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS);
#endif
	return AMPHORA_STATUS_OK;
}

void
Amphora_DestroyHeap(void) {
#if defined(__APPLE__) || defined(__linux__)
	munmap(amphora_heap, sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS);
#else
	free(amphora_heap);
#endif
	amphora_heap = NULL;
}

void *
Amphora_HeapAlloc(size_t size) {
	return NULL;
}

void *
Amphora_HeapAllocFrame(size_t size) {
	return NULL;
}

void
Amphora_HeapFree(void *ptr) {

}
