#include "engine/internal/random.h"

#include "config.h"

#ifndef RNG_SEED
#include <time.h>
#endif

/* File-scoped variables */
static Uint64 rand_state;

Uint32
Amphora_GetRandom(int n) {
	Uint64 val;

	rand_state = rand_state * 0xff1cd035ul + 0x05;
	val = (rand_state >> 32) * n;

	return (Uint32)(val >> 32);
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
