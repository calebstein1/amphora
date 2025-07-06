#include "engine/internal/error.h"
#include "engine/internal/db.h"
#include "engine/internal/lib.h"

#include "config.h"

static sqlite3 *game_db;

sqlite3 *
Amphora_GetDB(void) {
	return game_db;
}

/*
 * Internal functions
 */

int
Amphora_InitDB(void) {
	char *path = SDL_GetPrefPath(GAME_AUTHOR, GAME_TITLE);

	Amphora_ConcatString(&path, "amphora.db");
	sqlite3_open_v2(path, &game_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	SDL_free(path);

	return AMPHORA_STATUS_OK;
}

void
Amphora_CloseDB(void) {
	sqlite3_close_v2(game_db);
}
