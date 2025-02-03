#ifndef AMPHORA_TOOLS_INTERNAL_H
#define AMPHORA_TOOLS_INTERNAL_H

#include "SDL.h"

#define Amphora_ValidatePtrNotNull(param, ret) \
if (!param) { \
	Amphora_SetError(AMPHORA_STATUS_FAIL_UNDEFINED, "%s is NULL but shouldn't be!", #param); \
	return (ret); \
}
char *Amphora_strcat(char *s1, const char *s2);
Uint32 Amphora_crc32(const char *data);

#endif /* AMPHORA_TOOLS_INTERNAL_H */
