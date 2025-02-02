#if defined(__SSE4_2__)
#include <immintrin.h>
#elif defined(__ARM_ACLE) && defined(__ARM_FEATURE_CRC32)
#include <arm_acle.h>
#endif
#include <stdint.h>

#include "engine/internal/tools.h"

/* Prototypes for private functions */
static Uint32 hw_crc32c_loop(Uint32 crc, const void *data, size_t len);

/*
 * Internal functions
 */

char *
Amphora_strcat(char *s1, const char *s2) {
	size_t n = SDL_strlen(s1) + SDL_strlen(s2) + 1;

	s1 = SDL_realloc(s1, n);
	SDL_strlcat(s1, s2, n);

	return s1;
}

/*
 * TODO: Test on x86
 */
Uint32
Amphora_crc32c(const char *data) {
	Uint32 crc = 0xffffffff;
	size_t len;
	Uint32 (*crc_loop_func)(Uint32, const void *, size_t);

	if (!data || !*data) return 0;

	len = strlen(data);
#if (defined(__ARM_ACLE) && defined(__ARM_FEATURE_CRC32)) || defined(__SSE4_2__)
#ifdef DEBUG
	SDL_Log("Using hardware crc32c...\n");
#endif
	crc_loop_func = hw_crc32c_loop;
#else
#ifdef DEBUG
	SDL_Log("Using software crc32...\n");
#endif
	crc_loop_func = SDL_crc32;
#endif

	return crc_loop_func(crc, data, len) ^ 0xffffffff;
}

/*
 * Private functions
 */

Uint32
hw_crc32c_loop(Uint32 crc, const void *data, size_t len) {
	Uint32 (*crc_u8)(Uint32, Uint8) = NULL;
	Uint32 (*crc_u32)(Uint32, Uint32) = NULL;
	char *d = (char *)data;
	size_t i;

#if defined(__ARM_ACLE) && defined(__ARM_FEATURE_CRC32)
	crc_u8 = __crc32cb;
	crc_u32 = __crc32cw;
#elif __SSE4_2__
	crc_u8 = _mm_crc32_u8;
	crc_u32 = _mm_crc32_u32;
#else
#ifdef DEBUG
	SDL_Log("Falling back to software crc32");
#endif
	return SDL_crc32(crc, data, len);
#endif

	while (len && ((uintptr_t)d & 7) != 0) {
		crc = crc_u8(crc, *d++);
		len--;
	}
	len >>= 2;
	for(i = 0; i < len; i++) {
		crc = crc_u32(crc, *(Uint32 *)d);
		d += sizeof(Uint32);
	}
	while (*d) {
		crc = crc_u8(crc, *d++);
	}

	return crc;
}
