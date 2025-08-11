#ifndef AMPHORA_ERROR_INTERNAL_H
#define AMPHORA_ERROR_INTERNAL_H

#include "engine/error.h"

#define AMPHORA_MSG_BUFF_SIZE 256

void Amphora_SetError(AmphoraStatusCode status_code, const char *fmt, ...);
void Amphora_HandleCatastrophicFailure(void);

#endif /* AMPHORA_ERROR_INTERNAL_H */
