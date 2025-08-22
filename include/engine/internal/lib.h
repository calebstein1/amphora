#ifndef AMPHORA_TOOLS_INTERNAL_H
#define AMPHORA_TOOLS_INTERNAL_H

#include "SDL.h"

#define Amphora_ValidatePtrNotNull(param, ret) \
if (!param) { \
	Amphora_SetError(AMPHORA_STATUS_FAIL_UNDEFINED, "%s is NULL but shouldn't be!", #param); \
	return (ret); \
}

typedef struct {
	Uint16 timer;
	Uint16 frames;
	int idx;
	int idx_mod; /* Used to modify idx by arbitrary amounts, good for fade-in/fade-out (see scenes.c) */
	Uint8 *steps;
} AmphoraFader;

char *Amphora_ConcatString(char **s1, const char *s2);

#endif /* AMPHORA_TOOLS_INTERNAL_H */
