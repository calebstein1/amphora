#if defined(__APPLE__) || defined(__linux__)
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#elif defined(_WIN32)
#include <memoryapi.h>
#else
#include <stdlib.h>
#endif

#include <stdint.h>

#include "engine/internal/error.h"
#include "engine/internal/memory.h"


/* File-scoped variables */
static AmphoraMemBlock *amphora_heap;
static struct {
	AmphoraMemBlock data;
	uint16_t idx;
} amphora_frame_heap;
static struct amphora_mem_block_metadata_t *heap_metadata;
static uint8_t current_block_categories[MEM_COUNT];
static const char *category_names[] = {
#define X(cat) #cat,
	AMPHORA_MEM_CATEGORIES
#undef X
};

void
Amphora_HeapPtrToBlkIdx(void *ptr, int *blk, int *idx) {
	const long raw_idx = (intptr_t)ptr - (intptr_t)&amphora_heap[0][0];

	if (raw_idx < 0 || raw_idx > (long)sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Address supplied outside heap range");
		return;
	}
	*blk = (int)raw_idx / (int)sizeof(AmphoraMemBlock);
	*idx = (int)raw_idx & (int)sizeof(AmphoraMemBlock) - 1;
}

void *
Amphora_HeapBlkIdxToPtr(int blk, int idx) {
	return &amphora_heap[blk][idx];
}

void
Amphora_HeapDumpBlock(uint8_t blk) {
	unsigned int i;

	(void)printf("Memory block %d:\nCategory: %s\nAllocations: %d", blk, category_names[heap_metadata[blk].category], heap_metadata[blk].allocations);
	for (i = 0; i < sizeof(AmphoraMemBlock); i++) {
		if ((i & 15) == 0) (void)printf("\n%5d: ", i);
		(void)printf("%02X ", amphora_heap[blk][i]);
	}
	(void)fputs("\n", stdout);
}

uint8_t
Amphora_HeapPeek(uint8_t blk, uint16_t idx) {
	return amphora_heap[blk][idx];
}

void
Amphora_HeapPoke(uint8_t blk, uint16_t idx, uint8_t val) {
	amphora_heap[blk][idx] = val;
}

/*
 * Internal functions
 */

int
Amphora_InitHeap(void) {
#if defined(__APPLE__) || defined(__linux__)
	int fd = shm_open("/amphora_heap", O_CREAT | O_RDWR, 0666);
	if (ftruncate(fd, sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS) < 0) {
		(void)fputs("Failed to resize shared memory region, attempting to continue with private memory\n", stderr);
		amphora_heap = mmap(NULL, sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	} else {
		amphora_heap = mmap(NULL, sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	}
	(void)close(fd);
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
#if defined(__APPLE__) || defined(__linux__)
		(void)shm_unlink("/amphora_heap");
#endif
		return AMPHORA_STATUS_ALLOC_FAIL;
	}

	/* This bootstraps a metadata structure without metadata so that we can allocate one properly with metadata */
	current_block_categories[MEM_META] = AMPHORA_NUM_MEM_BLOCKS - 1;
	heap_metadata = (struct amphora_mem_block_metadata_t *)&amphora_heap[AMPHORA_NUM_MEM_BLOCKS - 1][8];

	heap_metadata = Amphora_HeapAlloc(sizeof(struct amphora_mem_block_metadata_t) * AMPHORA_NUM_MEM_BLOCKS, MEM_META);
	if (heap_metadata != NULL)  return AMPHORA_STATUS_OK;

	/* We should never hit this code path */
	Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to initialize heap metadata");
	Amphora_DestroyHeap();
	return AMPHORA_STATUS_ALLOC_FAIL;
}

void
Amphora_DestroyHeap(void) {
	Amphora_HeapFree(heap_metadata);
#if defined(__APPLE__) || defined(__linux__)
	(void)munmap(amphora_heap, sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS);
	(void)shm_unlink("/amphora_heap");
#elif defined(_WIN32)
	VirtualFree(amphora_heap, 0, MEM_RELEASE);
#else
	free(amphora_heap);
#endif
	amphora_heap = NULL;
}

void *
Amphora_HeapAlloc(size_t size, AmphoraMemBlockCategory category) {
	uint8_t *addr;
	uint8_t current_block;
	int i = 0;
	size_t aligned_size = size + 7 & ~7;
	struct amphora_mem_allocation_header_t header = {
		.magic = MAGIC,
		.scope = 0,
		.free = 0,
		.large = 0,
		.size = aligned_size
	};

	if (aligned_size + 8 > sizeof(AmphoraMemBlock)) {
		/*
		 * TODO: support large allocations
		 */
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Allocation cannot exceed %d", sizeof(AmphoraMemBlock) - 8);
		return NULL;
	}
	current_block = current_block_categories[category];
	while ((heap_metadata[current_block].category != MEM_UNASSIGNED && heap_metadata[current_block].category != category)
		|| heap_metadata[current_block].addr + aligned_size + 8 >= sizeof(AmphoraMemBlock)) {
		current_block++;
		if (++i < AMPHORA_NUM_MEM_BLOCKS) continue;
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Heap full");
		return NULL;
	}
	current_block_categories[category] = current_block;
	heap_metadata[current_block].category = category;
	(void)memcpy(&amphora_heap[current_block][heap_metadata[current_block].addr], &header, sizeof(header));
	heap_metadata[current_block].addr += sizeof(header);
	addr = &amphora_heap[current_block][heap_metadata[current_block].addr];
	heap_metadata[current_block].addr += aligned_size;
	heap_metadata[current_block].allocations++;

	return addr;
}

void *
Amphora_HeapAllocFrame(size_t size) {
	uint8_t *addr;
	size_t aligned_size = size + 7 & ~7;

	if (amphora_frame_heap.idx + aligned_size > sizeof(AmphoraMemBlock)) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Allocation failed, per-frame heap full");
		return NULL;
	}
	addr = &amphora_frame_heap.data[amphora_frame_heap.idx];
	amphora_frame_heap.idx += aligned_size;

	return addr;
}

void *
Amphora_HeapRealloc(void *ptr, size_t size, AmphoraMemBlockCategory category) {
	uint8_t *addr = Amphora_HeapAlloc(size, category);
	struct amphora_mem_allocation_header_t *header;

	if (addr == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to reallocate space on heap");
		return ptr;
	}
	if (ptr == NULL) return addr;

	header = (struct amphora_mem_allocation_header_t *)ptr - 1;
	if (header->magic != MAGIC) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Invalid allocation header");
		return ptr;
	}

	(void)memcpy(addr, ptr, header->size);
	Amphora_HeapFree(ptr);

	return addr;
}

void *
Amphora_HeapCalloc(size_t num, size_t size, AmphoraMemBlockCategory category) {
	uint8_t *addr = Amphora_HeapAlloc(num * size, category);

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
	const long idx = (intptr_t)ptr - (intptr_t)&amphora_heap[0][0];
	unsigned int block;
	struct amphora_mem_allocation_header_t *header;

	if (ptr == NULL) return;

	if (idx < 0 || idx > (long)sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Address supplied outside heap range");
		return;
	}
	header = (struct amphora_mem_allocation_header_t *)ptr - 1;
	if (header->magic != MAGIC) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Invalid allocation header");
		return;
	}
	if (header->free) return;

	header->free = 1;
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
