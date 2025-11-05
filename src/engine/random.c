#include "engine/internal/random.h"

#include <time.h>

static Uint32 rand_state;

Uint16
Amphora_GetRandom(Uint16 n)
{
	rand_state ^= (rand_state << 13);
	rand_state ^= (rand_state >> 17);
	rand_state ^= (rand_state << 5);
	return ((rand_state >> 16) * n) >> 16;
}

float
Amphora_GetRandomF(void)
{
	return (float)Amphora_GetRandom(UINT16_MAX) / (float)UINT16_MAX;
}

/*
 * Internal functions
 */

void
Amphora_InitRand(void)
{
	rand_state = time(0);
}
