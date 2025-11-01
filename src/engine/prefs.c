#ifdef WIN32
#include <objbase.h>
#elif __APPLE__
#include <CoreFoundation/CFUUID.h>
#else
#include "uuid/uuid.h"
#endif

#include "engine/internal/db.h"
#include "engine/internal/lib.h"
#include "engine/internal/memory.h"
#include "engine/internal/prefs.h"

#include "config.h"

/* Prototypes for private functions */
static SDL_GUID Amphora_GetUUID(void);

/* File-scoped variables */
static char uuid[33];

/*
 * Internal functions
 */

int
Amphora_InitConfig(void) {
	sqlite3 *db = Amphora_GetDB();
	const char *sql = "CREATE TABLE IF NOT EXISTS prefs("
			  "uuid TEXT PRIMARY KEY NOT NULL,"
			  "win_x INT,"
			  "win_y INT,"
			  "win_flags INT,"
			  "framerate INT);";
	const char *sql_create_row = "INSERT INTO prefs (uuid) VALUES (?);";
	sqlite3_stmt *stmt;
	char *err_msg;

	sqlite3_exec(db, sql, NULL, NULL, &err_msg);
	if (err_msg) {
		SDL_Log("%s\n", err_msg);
		return -1;
	}
	SDL_GUIDToString(Amphora_GetUUID(), uuid, sizeof(uuid));
	sqlite3_prepare_v2(db, sql_create_row, (int)SDL_strlen(sql_create_row), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, uuid, -1, NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 0;
}

int
Amphora_SaveWinX(int win_x) {
	sqlite3 *db = Amphora_GetDB();
	sqlite3_stmt *stmt;
	const char *sql = "UPDATE prefs SET win_x=? WHERE uuid=?;";

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_int64(stmt, 1, win_x);
	sqlite3_bind_text(stmt, 2, uuid, -1, NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 0;
}

int
Amphora_SaveWinY(int win_y) {
	sqlite3 *db = Amphora_GetDB();
	sqlite3_stmt *stmt;
	const char *sql = "UPDATE prefs SET win_y=? WHERE uuid=?;";

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_int64(stmt, 1, win_y);
	sqlite3_bind_text(stmt, 2, uuid, -1, NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 0;
}

int
Amphora_SaveWinFlags(Uint32 win_flags) {
	sqlite3 *db = Amphora_GetDB();
	sqlite3_stmt *stmt;
	const char *sql = "UPDATE prefs SET win_flags=? WHERE uuid=?;";

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_int64(stmt, 1, (Sint64)win_flags);
	sqlite3_bind_text(stmt, 2, uuid, -1, NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 0;
}

int
Amphora_SaveFPS(Uint32 framerate) {
	sqlite3 *db = Amphora_GetDB();
	sqlite3_stmt *stmt;
	const char *sql = "UPDATE prefs SET framerate=? WHERE uuid=?;";

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_int64(stmt, 1, framerate);
	sqlite3_bind_text(stmt, 2, uuid, -1, NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 0;
}

Sint32
Amphora_LoadWinX(void) {
	sqlite3 *db = Amphora_GetDB();
	sqlite3_stmt *stmt;
	const char *sql = "SELECT win_x FROM prefs WHERE uuid=?";
	Sint32 val;

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, uuid, -1, NULL);
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return WINDOW_X;
	}
	val = (Sint32)sqlite3_column_int64(stmt, 0);
	sqlite3_finalize(stmt);

	return val ? val : WINDOW_X;
}

Sint32
Amphora_LoadWinY(void) {
	sqlite3 *db = Amphora_GetDB();
	sqlite3_stmt *stmt;
	const char *sql = "SELECT win_y FROM prefs WHERE uuid=?";
	Sint32 val;

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, uuid, -1, NULL);
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return WINDOW_Y;
	}
	val = (Sint32)sqlite3_column_int64(stmt, 0);
	sqlite3_finalize(stmt);

	return val ? val : WINDOW_Y;
}

Uint32
Amphora_LoadWinFlags(void) {
	sqlite3 *db = Amphora_GetDB();
	sqlite3_stmt *stmt;
	const char *sql = "SELECT win_flags FROM prefs WHERE uuid=?";
	Sint32 val;

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, uuid, -1, NULL);
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return WINDOW_MODE;
	}
	val = (Sint32)sqlite3_column_int64(stmt, 0);
	sqlite3_finalize(stmt);

	return val ? (Uint32)val : WINDOW_MODE;
}

Sint32
Amphora_LoadFPS(void) {
	sqlite3 *db = Amphora_GetDB();
	sqlite3_stmt *stmt;
	const char *sql = "SELECT framerate FROM prefs WHERE uuid=?";
	Sint32 val;

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, uuid, -1, NULL);
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return FRAMERATE;
	}
	val = (Sint32)sqlite3_column_int64(stmt, 0);
	sqlite3_finalize(stmt);

	return val ? val : FRAMERATE;
}

/*
 * Private functions
 */

static SDL_GUID
Amphora_GetUUID(void) {
	char *path = Amphora_HeapStrdup(SDL_GetPrefPath(GAME_AUTHOR, GAME_TITLE));
	SDL_RWops *rw;
	char *file_contents;
	SDL_GUID guid;
	char guid_str[33];

	Amphora_ConcatString(&path, "uuid");
	if ((rw = SDL_RWFromFile(path, "rb"))) {
#ifdef DEBUG
		SDL_Log("Loading UUID from file...\n");
#endif
		if (!((file_contents = Amphora_HeapAlloc(SDL_RWsize(rw), MEM_STRING)))) {
			Amphora_HeapFree(path);
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate space for UUID!\n");
			SDL_memset(&guid, 0, sizeof(guid));

			return guid;
		}
		SDL_RWread(rw, file_contents, SDL_RWsize(rw), 1);
		SDL_memcpy(&guid.data, file_contents, sizeof(guid.data));
		Amphora_HeapFree(file_contents);
		SDL_RWclose(rw);

#ifdef DEBUG
		SDL_GUIDToString(guid, guid_str, sizeof(guid_str));
		SDL_Log("UUID: %s\n", guid_str);
#endif

		return guid;
	}

#ifdef DEBUG
	SDL_Log("Generating new UUID...\n");
#endif
#ifdef WIN32
	GUID uuid_bytes;

	CoCreateGuid(&uuid_bytes);
	SDL_memcpy(&guid.data[0], &uuid_bytes.Data1, sizeof(uuid_bytes.Data1));
	SDL_memcpy(&guid.data[4], &uuid_bytes.Data2, sizeof(uuid_bytes.Data2));
	SDL_memcpy(&guid.data[6], &uuid_bytes.Data3, sizeof(uuid_bytes.Data3));
	SDL_memcpy(&guid.data[8], &uuid_bytes.Data4, sizeof(uuid_bytes.Data4));
#elif __APPLE__
	CFUUIDRef uuid_ref = CFUUIDCreate(NULL);
	CFUUIDBytes uuid_bytes = CFUUIDGetUUIDBytes(uuid_ref);
	CFRelease(uuid_ref);

	SDL_memcpy(&guid.data, &uuid_bytes, sizeof(guid.data));
#else
	uuid_generate(guid.data);
#endif

	rw = SDL_RWFromFile(path, "w+b");
	SDL_RWwrite(rw, &guid.data, sizeof(guid.data), 1);
	SDL_RWclose(rw);
	Amphora_HeapFree(path);

#ifdef DEBUG
	SDL_GUIDToString(guid, guid_str, sizeof(guid_str));
	SDL_Log("UUID: %s\n", guid_str);
#endif

	return guid;
}
