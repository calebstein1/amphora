#include "engine/internal/error.h"
#include "engine/internal/db.h"
#include "engine/internal/lib.h"
#include "engine/internal/memory.h"

static sqlite3 *game_db;
static const char *game_author;
static const char *game_title;

sqlite3 *
Amphora_GetDB(void)
{
	return game_db;
}

/*
 * Internal functions
 */

int
Amphora_InitDB(void)
{
	char *path = Amphora_HeapStrdup(SDL_GetPrefPath(game_author, game_title));

	(void)Amphora_ConcatString(&path, "amphora.db");
	if (sqlite3_open_v2(path, &game_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) != SQLITE_OK)
	{
		Amphora_HeapFree(path);
		return AMPHORA_STATUS_ALLOC_FAIL;
	}
	Amphora_HeapFree(path);

	return AMPHORA_STATUS_OK;
}

void
Amphora_CloseDB(void)
{
	(void)sqlite3_close_v2(game_db);
}

/*
 * Dependency Injection functions
 */

void
Amphora_RegisterGameData(const char *author, const char *title)
{
	game_author = author;
	game_title = title;
}
