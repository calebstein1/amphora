#include "engine/internal/error.h"

/* File-scoped variables */
static char err_buff[AMPHORA_MSG_BUFF_SIZE];
static AmphoraStatusCode err_code;

const char *
Amphora_GetError(void) {
	return err_buff;
}

AmphoraStatusCode
Amphora_GetErrorCode(void) {
	return err_code;
}

/*
 * Internal functions
 */

void
Amphora_SetError(AmphoraStatusCode status_code, const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	SDL_vsnprintf(err_buff, AMPHORA_MSG_BUFF_SIZE, fmt, args);
	err_code = status_code;
}
