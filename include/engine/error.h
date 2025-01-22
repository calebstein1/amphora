#ifndef AMPHORA_ERROR_H
#define AMPHORA_ERROR_H

#include "SDL.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Gets the last set error message */
const char *Amphora_GetError(void);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_ERROR_H */
