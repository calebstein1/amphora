#ifndef AMPHORA_RANDOM_H
#define AMPHORA_RANDOM_H

#include "SDL.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Get a random number less than n */
Uint32 get_random(int n);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_RANDOM_H */
