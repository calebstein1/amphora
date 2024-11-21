#include "engine/internal/db.h"

#include "config.h"

static sqlite3 *game_db;

sqlite3 *
get_db(void) {
	return game_db;
}

/*
 * Internal functions
 */

void
init_db(void) {
	char *path = SDL_GetPrefPath(GAME_AUTHOR, GAME_TITLE);
	const char *filename = "amphora.db";
	size_t new_len = SDL_strlen(path) + SDL_strlen(filename) + 1;

	path = SDL_realloc(path, new_len);
	SDL_strlcat(path, filename, new_len);
	sqlite3_open_v2(path, &game_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	SDL_free(path);
}

void
cleanup_db(void) {
	sqlite3_close_v2(game_db);
}
