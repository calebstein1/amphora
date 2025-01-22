#include "engine/util.h"
#include "engine/internal/error.h"

/* File-scoped variables */
static char err_buff[AMPHORA_MSG_BUFF_SIZE];

const char *
Amphora_GetError(void) {
	return err_buff;
}

/*
 * Internal functions
 */

void
Amphora_SetError(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	SDL_vsnprintf(err_buff, AMPHORA_MSG_BUFF_SIZE, fmt, args);
}
