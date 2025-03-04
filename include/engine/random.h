#ifndef AMPHORA_RANDOM_H
#define AMPHORA_RANDOM_H

#include "SDL.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Get a 16-bit random number less than n */
Uint16 Amphora_GetRandom(Uint16 n);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_RANDOM_H */
