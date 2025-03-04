#include "engine/internal/random.h"

#include "config.h"

#ifndef RNG_SEED
#include <time.h>
#endif

static Uint32 rand_state;

Uint16
Amphora_GetRandom(Uint16 n) {
	Uint32 val;

	rand_state = rand_state * 0x41c64e6dul + 0x3c6ef35ful;
	val = rand_state ^ (rand_state >> 11);
	val = (val >> 16) * n;

	return val >> 16;
}

/*
 * Internal functions
 */

void
Amphora_InitRand(void) {
#ifdef RNG_SEED
	rand_state = RNG_SEED;
#else
	rand_state = time(0);
#endif
}
