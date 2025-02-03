#if defined(__AVX__)
#include <nmmintrin.h>
#elif __ARM_ARCH >= 8 && defined(__ARM_FEATURE_CRC32)
#include <arm_acle.h>
#endif
#include <stdint.h>

#include "engine/internal/lib.h"

/* Prototypes for private functions */
#if defined(__AVX__) || (defined(__ARM_ACLE) && defined(__ARM_FEATURE_CRC32))
static Uint32 hw_crc32_loop(Uint32 crc, const void *data, size_t len);
#endif

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
Amphora_crc32(const char *data) {
	Uint32 crc = 0xffffffff;
	size_t len;
	Uint32 (*crc_loop_func)(Uint32, const void *, size_t);

	if (!data || !*data) return 0;

	len = strlen(data);
#if defined (__AVX__) || (__ARM_ARCH >= 8 && defined(__ARM_FEATURE_CRC32))
#ifdef DEBUG
	SDL_Log("Using hardware crc32...\n");
#endif
	crc_loop_func = hw_crc32_loop;
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

#if defined(__AVX__) || (__ARM_ARCH >= 8 && defined(__ARM_FEATURE_CRC32))
Uint32
hw_crc32_loop(Uint32 crc, const void *data, size_t len) {
	char *d = (char *)data;
	size_t i;

	while (len && ((uintptr_t)d & 7) != 0) {
#if defined(__AVX__)
		crc = _mm_crc32_u8(crc, *d++);
#else
		crc = __crc32cb(crc, *d++);
#endif
		len--;
	}
	len >>= 2;
	for(i = 0; i < len; i++) {
#if defined(__AVX__)
		crc = _mm_crc32_u32(crc, *(Uint32 *)d);
#else
		crc = __crc32cw(crc, *(Uint32 *)d);
#endif
		d += sizeof(Uint32);
	}
	while (*d) {
#if defined(__AVX__)
		crc = _mm_crc32_u8(crc, *d++);
#else
		crc = __crc32cb(crc, *d++);
#endif
	}

	return crc;
}
#endif
