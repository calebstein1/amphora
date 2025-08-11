#ifdef __APPLE__
#include <sys/mman.h>
#else
#include <stdlib.h>
#endif

#include "engine/internal/error.h"
#include "engine/internal/memory.h"

/* File-scoped variables */
static AmphoraMemBlock *amphora_heap;
static struct amphora_mem_block_metadata_t heap_metadata[AMPHORA_NUM_MEM_BLOCKS];
static Uint8 current_block = 0;

int
Amphora_InitHeap(void) {
#if defined(__APPLE__) || defined(__linux__)
	amphora_heap = mmap(NULL, sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
#else
	amphora_heap = malloc(sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS);
#endif

	if (amphora_heap == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to initialize heap");
		return AMPHORA_STATUS_ALLOC_FAIL;
	}
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
	Uint8 *addr;
	int i = 0;
	size_t aligned_size = (size + 7) & ~7;

	if (size > sizeof(AmphoraMemBlock)) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Allocation cannot exceed %d", sizeof(AmphoraMemBlock));
		return NULL;
	}
	while (heap_metadata[current_block].addr + aligned_size >= sizeof(AmphoraMemBlock)) {
		current_block++;
		SDL_Log("Incrementing current block to %d\n", current_block);
		if (++i < AMPHORA_NUM_MEM_BLOCKS) continue;

		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Heap full");
		return NULL;
	}
	addr = &amphora_heap[current_block][heap_metadata[current_block].addr];
	heap_metadata[current_block].addr += aligned_size;
	heap_metadata[current_block].allocations++;

	return addr;
}

void *
Amphora_HeapAllocFrame(size_t size) {
	/*
	 * TODO: Like HeapAlloc but auto-free on the next render frame
	 */
	return NULL;
}

void *
Amphora_HeapRealloc(void *ptr, size_t nbytes, size_t size) {
	Uint8 *addr = Amphora_HeapAlloc(size);

	if (addr == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to reallocate space on heap");
		return ptr;
	}
	(void)memcpy(addr, ptr, nbytes);
	Amphora_HeapFree(ptr);

	return addr;
}

void *
Amphora_HeapCalloc(size_t num, size_t size) {
	Uint8 *addr = Amphora_HeapAlloc(num * size);

	if (addr == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to reallocate space on heap");
		return NULL;
	}
	(void)memset(addr, 0, num * size);

	return addr;
}

char *
Amphora_HeapStrdup(const char *str) {
	char *addr = Amphora_HeapAlloc(strlen(str) + 1);

	if (addr == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to reallocate space on heap");
		return NULL;
	}
	(void)strcpy(addr, str);

	return addr;
}

void
Amphora_HeapFree(void *ptr) {
	const long idx = (Uint8 *)ptr - &amphora_heap[0][0];
	unsigned int block;

	if (idx < 0 || idx > sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Address supplied outside heap range");
		return;
	}
	block = idx / sizeof(AmphoraMemBlock);
	if (--heap_metadata[block].allocations == 0)
		heap_metadata[block].addr = 0;
}
