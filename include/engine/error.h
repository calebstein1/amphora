#ifndef AMPHORA_ERROR_H
#define AMPHORA_ERROR_H

#include "SDL.h"

typedef enum {
	AMPHORA_STATUS_OK,
	AMPHORA_STATUS_ALLOC_FAIL,
	AMPHORA_STATUS_CORE_FAIL,
	AMPHORA_STATUS_FAIL_UNDEFINED
} AmphoraStatusCode;

#ifdef __cplusplus
extern "C" {
#endif
/* Gets the last set error message */
const char *Amphora_GetError(void);
/* Gets the last set error code */
AmphoraStatusCode Amphora_GetErrorCode(void);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_ERROR_H */
