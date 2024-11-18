#include "engine/internal/save_data.h"

#include "config.h"

#include "sqlite3.h"

sqlite3 *save_db;

int
save_value(const char *attribute, void *value) {
	return 0;
}

void
*get_value(const char *attribute) {
	return 0;
}

/*
 * Internal functions
 */

int
init_save(void) {
	char *path = SDL_GetPrefPath(GAME_AUTHOR, GAME_TITLE);
	const char *filename = "save.db";
	size_t new_len = SDL_strlen(path) + SDL_strlen(filename) + 1;

	path = SDL_realloc(path, new_len);
	SDL_strlcat(path, filename, new_len);
	sqlite3_open_v2(path, &save_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
	SDL_free(path);

	return 0;
}

void
cleanup_save(void) {
	sqlite3_close_v2(save_db);
}
