#include <stdint.h>
#if defined(__APPLE__) || defined(__linux__)
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#elif defined(_WIN32)
#include <memoryapi.h>
#else
#include <stdlib.h>
#endif

#include "engine/internal/error.h"
#include "engine/internal/memory.h"
#include "engine/util.h"

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

int
Amphora_HeapPtrToBlkIdx(void *ptr, int *blk, int *idx) {
	const ptrdiff_t raw_idx = (intptr_t)ptr - (intptr_t)&amphora_heap[0][0];
	const int b = (int)raw_idx / (int)sizeof(AmphoraMemBlock);
	const int i = (int)raw_idx & (int)sizeof(AmphoraMemBlock) - 1;

	if (raw_idx < 0 || raw_idx >= (long)sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Address supplied outside heap range");
		return -1;
	}
	if (blk) *blk = b;
	if (idx) *idx = i;

	return b;
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
	int i;
	struct amphora_mem_allocation_header_t *header;
#if defined(__APPLE__) || defined(__linux__)
	int fd = shm_open("/amphora_heap", O_CREAT | O_RDWR, 0666);
	if (fd == -1 || ftruncate(fd, sizeof(AmphoraMemBlock) * AMPHORA_NUM_MEM_BLOCKS) == -1) {
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

	for (i = 0; i < AMPHORA_NUM_MEM_BLOCKS; i++) {
		header = (struct amphora_mem_allocation_header_t *)&amphora_heap[i][0];
		header->magic = MAGIC;
		header->scope = 0;
		header->free = 1;
		header->large = 0;
		header->off_f = sizeof(AmphoraMemBlock) - sizeof(struct amphora_mem_allocation_header_t);
		header->off_b = 0;
	}

	/* This bootstraps a metadata structure without metadata so that we can allocate one properly with metadata */
	current_block_categories[MEM_META] = AMPHORA_NUM_MEM_BLOCKS - 1;
	heap_metadata = (struct amphora_mem_block_metadata_t *)&amphora_heap[AMPHORA_NUM_MEM_BLOCKS - 1][8];
	heap_metadata[AMPHORA_NUM_MEM_BLOCKS - 1].largest_free = sizeof(AmphoraMemBlock) - sizeof(struct amphora_mem_allocation_header_t);

	heap_metadata = Amphora_HeapAlloc(sizeof(struct amphora_mem_block_metadata_t) * AMPHORA_NUM_MEM_BLOCKS, MEM_META);
	if (heap_metadata == NULL) {
		/* We should never hit this code path */
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to initialize heap metadata");
		Amphora_DestroyHeap();
		return AMPHORA_STATUS_ALLOC_FAIL;
	}
	for (i = 0; i < AMPHORA_NUM_MEM_BLOCKS; i++) {
		heap_metadata[i].largest_free = sizeof(AmphoraMemBlock) - sizeof(struct amphora_mem_allocation_header_t);
	}

	return AMPHORA_STATUS_OK;
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
	struct amphora_mem_allocation_header_t *header, *next_header, *next_next_header;
	bool split = false;
	bool recovery_success = false;

	if (aligned_size + 8 > sizeof(AmphoraMemBlock)) {
		/*
		 * TODO: support large allocations
		 */
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Allocation cannot exceed %d", sizeof(AmphoraMemBlock) - 8);
		return NULL;
	}
	current_block = current_block_categories[category];
	while ((heap_metadata[current_block].category != MEM_UNASSIGNED && heap_metadata[current_block].category != category)
		|| heap_metadata[current_block].largest_free < aligned_size + sizeof(struct amphora_mem_allocation_header_t)
		|| heap_metadata[current_block].corrupted) {
		current_block++;
		if (++i < AMPHORA_NUM_MEM_BLOCKS) continue;
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Heap full");
		return NULL;
	}
	current_block_categories[category] = current_block;
	heap_metadata[current_block].category = category;

	header = (struct amphora_mem_allocation_header_t *)&amphora_heap[current_block][0];
	while (header->free == 0 || header->off_f < aligned_size) {
		if ((uintptr_t)header > (uintptr_t)amphora_heap[current_block] + sizeof(AmphoraMemBlock)) {
			/* If we hit this path, we're likely in a state of utter pandemonium and there's no sense in continuing */
			goto corrupt_fail;
		}
		/* Since sizeof(header) == 8, this is faster than division and safe because of our 8-byte alignment */
		next_header = header + 1 + (header->off_f >> 3);
		if (next_header->magic != MAGIC) {
			/* It's a disaster if we're trying this */
#ifdef DEBUG
			(void)fprintf(stderr, "HEAP CORRUPTED: attempting recovery on block %d... don't hold your breath\n", current_block);
#endif
			while ((uintptr_t)next_header < (uintptr_t)amphora_heap[current_block] + sizeof(AmphoraMemBlock)) {
				next_header++;
				if (next_header->magic != MAGIC ||
					(uintptr_t)next_header - (uintptr_t)header != next_header->off_b)
					continue;
				header->off_f = next_header->off_b - sizeof(struct amphora_mem_allocation_header_t);
				recovery_success = true;
				break;
			}
			if (!recovery_success) goto corrupt_fail;
		}
		header = next_header;
		continue;

		corrupt_fail:
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Heap corrupted: unrecoverable, lost block %d", current_block);
		heap_metadata[current_block].corrupted = 1;
		return NULL;
	}
	/* If we have room for a next header in the current block */
	if ((uintptr_t)header < (uintptr_t)amphora_heap[current_block] + sizeof(AmphoraMemBlock) - 2 * sizeof(struct amphora_mem_allocation_header_t)) {
		/* If there's room to split the block, let's */
		if (header->off_f > aligned_size + sizeof(struct amphora_mem_allocation_header_t)) split = true;
		next_header = split ? header + 1 + (aligned_size >> 3) : header + 1 + (header->off_f >> 3);
		if (split) {
			(void)memset(next_header, 0, sizeof(struct amphora_mem_allocation_header_t));
			next_header->magic = MAGIC;
			next_header->off_f = header->off_f - aligned_size - sizeof(struct amphora_mem_allocation_header_t);
			next_header->free = 1;
			next_header->off_b = aligned_size + sizeof(struct amphora_mem_allocation_header_t);
			next_next_header = next_header + 1 + (next_header->off_f >> 3);
			if ((uintptr_t)next_next_header < (uintptr_t)amphora_heap[current_block] + sizeof(AmphoraMemBlock))
				next_next_header->off_b = next_header->off_f + sizeof(struct amphora_mem_allocation_header_t);
		}
	}
	/* We take care of this calculation properly in the housekeeping tasks, this is quick and dirty */
	if (heap_metadata[current_block].largest_free == header->off_f)
		heap_metadata[current_block].largest_free -= aligned_size - sizeof(struct amphora_mem_allocation_header_t);

	header->magic = MAGIC;
	header->scope = 0;
	header->free = 0;
	header->large = 0;
	header->off_f = split ? aligned_size : header->off_f;
	addr = (uint8_t *)header + sizeof(struct amphora_mem_allocation_header_t);
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
		return NULL;
	}
	if (ptr == NULL) return addr;

	header = (struct amphora_mem_allocation_header_t *)ptr - 1;
	if (header->magic != MAGIC) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Invalid allocation header");
		return NULL;
	}

	(void)memcpy(addr, ptr, header->off_f < size ? header->off_f : size);
	Amphora_HeapFree(ptr);

	return addr;
}

void *
Amphora_HeapCalloc(size_t num, size_t size, AmphoraMemBlockCategory category) {
	uint8_t *addr = Amphora_HeapAlloc(num * size, category);
	struct amphora_mem_allocation_header_t *header;

	if (addr == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to allocate space on heap");
		return NULL;
	}
	header = (struct amphora_mem_allocation_header_t *)addr - 1;
	(void)memset(addr, 0, header->off_f);

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
	const  ptrdiff_t idx = (intptr_t)ptr - (intptr_t)&amphora_heap[0][0];
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

	block = idx / sizeof(AmphoraMemBlock);
	if (header->off_f > heap_metadata[block].largest_free) heap_metadata[block].largest_free = header->off_f;
	header->free = 1;
	if (--heap_metadata[block].allocations == 0) {
		heap_metadata[block].largest_free = sizeof(AmphoraMemBlock) - sizeof(struct amphora_mem_allocation_header_t);
		heap_metadata[block].category = MEM_UNASSIGNED;
		heap_metadata[block].corrupted = 0;
	}
}

void
Amphora_HeapClearFrameHeap(void) {
	amphora_frame_heap.idx = 0;
}

uint32_t
Amphora_HeapHousekeeping(uint32_t ms) {
	static uint8_t blk = 0, blk_last_update = 0;
	static struct amphora_mem_allocation_header_t *header = NULL;

	struct amphora_mem_allocation_header_t *next_header;
	uint32_t start_time = SDL_GetTicks();

	if (header == NULL)
		header = (struct amphora_mem_allocation_header_t *)&amphora_heap[0][0];

	if (ms == 0) return 0;

	while (SDL_GetTicks() - start_time < ms) {
		next_header = header + 1 + (header->off_f >> 3);
		if (heap_metadata[blk].category == MEM_UNASSIGNED
			|| (uintptr_t)next_header > (uintptr_t)amphora_heap[blk] + sizeof(AmphoraMemBlock) - sizeof(struct amphora_mem_allocation_header_t)
			|| heap_metadata[blk].corrupted) {
			if (blk == blk_last_update - 1) {
				blk_last_update = blk;
				return ms - (SDL_GetTicks() - start_time);
			}
			blk++;
			header = (struct amphora_mem_allocation_header_t *)&amphora_heap[blk][0];
			continue;
		}
		/* Update largest_free */
		if (header->off_f > heap_metadata[blk].largest_free) {
			heap_metadata[blk].largest_free = header->off_f;
			blk_last_update = blk;
		}
		/* Coalesce free blocks */
		if (header->free && next_header->free) {
			next_header->magic = 0;
			header->off_f += next_header->off_f + sizeof(struct amphora_mem_allocation_header_t);
			next_header = header + 1 + (header->off_f >> 3);
			next_header->off_b = header->off_f + sizeof(struct amphora_mem_allocation_header_t);
			blk_last_update = blk;
			continue;
		}
		/* Remove zero-size headers */
		if (next_header->off_f == 0) {
			header->off_f += sizeof(struct amphora_mem_allocation_header_t);
			next_header = header + 1 + (header->off_f >> 3);
			next_header->off_b = header->off_f + sizeof(struct amphora_mem_allocation_header_t);
			blk_last_update = blk;
			continue;
		}
		header = next_header;
	}

	return 0;
}
