#if defined(__APPLE__) || defined(__linux__)
#include <sys/mman.h>
#elif defined(_WIN32)
#include <memoryapi.h>
#else
#include <stdlib.h>
#endif

#include "engine/internal/error.h"
#include "engine/internal/memory.h"

/* File-scoped variables */
static AmphoraMemBlock *amphora_heap;
static struct {
	AmphoraMemBlock data;
	Uint16 idx;
} amphora_frame_heap;
static struct amphora_mem_block_metadata_t heap_metadata[AMPHORA_NUM_MEM_BLOCKS];
static Uint8 current_block = 0;
static Uint8 current_block_categories[MEM_COUNT];

int
Amphora_InitHeap(void) {
#if defined(__APPLE__) || defined(__linux__)
	amphora_heap = mmap(NULL, sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
#elif defined(_WIN32)
	/*
	 * TODO: Implement large pages support for Windows
	 */
	amphora_heap = VirtualAlloc(NULL, sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
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
#elif defined(_WIN32)
	VirtualFree(amphora_heap, 0, MEM_RELEASE);
#else
	free(amphora_heap);
#endif
	amphora_heap = NULL;
}

void *
Amphora_HeapAlloc(size_t size, AmphoraMemBlockCategory category) {
	Uint8 *addr;
	int i = 0;
	size_t aligned_size = size + 7 & ~7;

	if (size + 2 > sizeof(AmphoraMemBlock)) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Allocation cannot exceed %d", sizeof(AmphoraMemBlock) - 2);
		return NULL;
	}
	current_block = current_block_categories[category];
	while ((heap_metadata[current_block].category != MEM_UNASSIGNED && heap_metadata[current_block].category != category)
		|| heap_metadata[current_block].addr + aligned_size + 8 >= sizeof(AmphoraMemBlock)) {
		current_block++;
#ifdef DEBUG
		SDL_Log("Changing current block to block %d\n", current_block);
#endif
		if (++i < AMPHORA_NUM_MEM_BLOCKS) continue;
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Heap full");
		return NULL;
	}
	current_block_categories[category] = current_block;
	heap_metadata[current_block].category = category;
	heap_metadata[current_block].addr += 8;
	amphora_heap[current_block][heap_metadata[current_block].addr - 2] = size & 0xFF;
	amphora_heap[current_block][heap_metadata[current_block].addr - 1] = size >> 8;
	addr = &amphora_heap[current_block][heap_metadata[current_block].addr];
	heap_metadata[current_block].addr += aligned_size;
	heap_metadata[current_block].allocations++;

	return addr;
}

void *
Amphora_HeapAllocFrame(size_t size) {
	Uint8 *addr;
	size_t aligned_size = size + 7 & ~7;

	if (amphora_frame_heap.idx + aligned_size < sizeof(AmphoraMemBlock)) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Allocation failed, per-frame heap full");
		return NULL;
	}
	addr = &amphora_frame_heap.data[amphora_frame_heap.idx];
	amphora_frame_heap.idx += aligned_size;

	return addr;
}

void *
Amphora_HeapRealloc(void *ptr, size_t size, AmphoraMemBlockCategory category) {
	Uint8 *addr = Amphora_HeapAlloc(size, category);

	if (addr == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to reallocate space on heap");
		return ptr;
	}
	if (ptr == NULL) return addr;

	(void)memcpy(addr, ptr, *((Uint8 *)ptr - 1) << 8 | *((Uint8 *)ptr - 2));
	Amphora_HeapFree(ptr);

	return addr;
}

void *
Amphora_HeapCalloc(size_t num, size_t size, AmphoraMemBlockCategory category) {
	Uint8 *addr = Amphora_HeapAlloc(num * size, category);

	if (addr == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to allocate space on heap");
		return NULL;
	}
	(void)memset(addr, 0, num * size);

	return addr;
}

char *
Amphora_HeapStrdup(const char *str) {
	char *addr = Amphora_HeapAlloc(strlen(str) + 1, MEM_STRING);

	if (addr == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to allocate space on heap");
		return NULL;
	}
	(void)strcpy(addr, str);

	return addr;
}

char *
Amphora_HeapStrdupFrame(const char *str) {
	char *addr = Amphora_HeapAllocFrame(strlen(str) + 1);

	if (addr == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to allocate space on heap");
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
	if (--heap_metadata[block].allocations == 0) {
		heap_metadata[block].addr = 0;
		heap_metadata[block].category = MEM_UNASSIGNED;
	}
}

void
Amphora_HeapClearFrameHeap(void) {
	amphora_frame_heap.idx = 0;
}
