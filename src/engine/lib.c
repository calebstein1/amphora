#include "engine/internal/lib.h"

/*
 * Internal functions
 */

char *
Amphora_strcat(char *s1, const char *s2) {
	size_t n = SDL_strlen(s1) + SDL_strlen(s2) + 1;

	s1 = SDL_realloc(s1, n);
	SDL_strlcat(s1, s2, n);

	return s1;
}
