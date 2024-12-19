#ifdef WIN32
#include <objbase.h>
#elif __APPLE__
#include <CoreFoundation/CFUUID.h>
#else
#endif

#include "engine/internal/config.h"
#include "engine/internal/db.h"

#include "config.h"

/* Prototypes for private functions */
SDL_GUID get_uuid(void);

/*
 * Internal functions
 */

int
init_config(void) {
	sqlite3 *db = get_db();
	const char *sql = "CREATE TABLE IF NOT EXISTS prefs("
			  "pref TEXT PRIMARY KEY NOT NULL,"
			  "value ANY);";
	char *err_msg;
	sqlite3_exec(db, sql, NULL, NULL, &err_msg);
	if (err_msg) {
		SDL_Log("%s\n", err_msg);
		return -1;
	}

	get_uuid();

	return 0;
}

int
save_window_x(int win_x) {
	sqlite3 *db = get_db();
	sqlite3_stmt *stmt;
	const char *sql = "INSERT OR REPLACE INTO prefs (pref, value) VALUES (?, ?);";

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, "win_x", -1, NULL);
	sqlite3_bind_int64(stmt, 2, win_x);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 0;
}

int
save_window_y(int win_y) {
	sqlite3 *db = get_db();
	sqlite3_stmt *stmt;
	const char *sql = "INSERT OR REPLACE INTO prefs (pref, value) VALUES (?, ?);";

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, "win_y", -1, NULL);
	sqlite3_bind_int64(stmt, 2, win_y);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 0;
}

int
save_win_flags(Uint64 win_flags) {
	sqlite3 *db = get_db();
	sqlite3_stmt *stmt;
	const char *sql = "INSERT OR REPLACE INTO prefs (pref, value) VALUES (?, ?);";

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, "win_flags", -1, NULL);
	sqlite3_bind_int64(stmt, 2, (Sint64)win_flags);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 0;
}

int
save_framerate(Uint32 framerate) {
	sqlite3 *db = get_db();
	sqlite3_stmt *stmt;
	const char *sql = "INSERT OR REPLACE INTO prefs (pref, value) VALUES (?, ?);";

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, "framerate", -1, NULL);
	sqlite3_bind_int64(stmt, 2, framerate);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 0;
}

Sint64
load_window_x(void) {
	sqlite3 *db = get_db();
	sqlite3_stmt *stmt;
	const char *sql = "SELECT value FROM prefs WHERE pref=?";
	Sint64 val;

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, "win_x", -1, NULL);
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return WINDOW_X;
	}
	val = sqlite3_column_int64(stmt, 0);
	sqlite3_finalize(stmt);

	return val;
}

Sint64
load_window_y(void) {
	sqlite3 *db = get_db();
	sqlite3_stmt *stmt;
	const char *sql = "SELECT value FROM prefs WHERE pref=?";
	Sint64 val;

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, "win_y", -1, NULL);
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return WINDOW_Y;
	}
	val = sqlite3_column_int64(stmt, 0);
	sqlite3_finalize(stmt);

	return val;
}

Uint64
load_win_flags(void) {
	sqlite3 *db = get_db();
	sqlite3_stmt *stmt;
	const char *sql = "SELECT value FROM prefs WHERE pref=?";
	Sint64 val;

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, "win_flags", -1, NULL);
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return WINDOW_MODE;
	}
	val = sqlite3_column_int64(stmt, 0);
	sqlite3_finalize(stmt);

	return (Uint64)val;
}

Sint64
load_framerate(void) {
	sqlite3 *db = get_db();
	sqlite3_stmt *stmt;
	const char *sql = "SELECT value FROM prefs WHERE pref=?";
	Sint64 val;

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, "framerate", -1, NULL);
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return FRAMERATE;
	}
	val = sqlite3_column_int64(stmt, 0);
	sqlite3_finalize(stmt);

	return val;
}

/*
 * Private functions
 */

SDL_GUID
get_uuid(void) {
	char *path = SDL_GetPrefPath(GAME_AUTHOR, GAME_TITLE);
	const char *filename = "uuid";
	SDL_RWops *rw;
	char *file_contents;
	size_t new_len = SDL_strlen(path) + SDL_strlen(filename) + 1;
	SDL_GUID guid;
	char guid_str[33];

	path = SDL_realloc(path, new_len);
	SDL_strlcat(path, filename, new_len);

	if ((rw = SDL_RWFromFile(path, "rb"))) {
#ifdef DEBUG
		SDL_Log("Loading UUID from file...\n");
#endif
		if (!((file_contents = SDL_malloc(SDL_RWsize(rw))))) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate space for UUID!\n");
			SDL_memset(&guid, 0, sizeof(guid));

			return guid;
		}
		SDL_RWread(rw, file_contents, SDL_RWsize(rw), 1);
		SDL_memcpy(&guid.data, file_contents, sizeof(guid.data));
		SDL_free(file_contents);
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
	GUID uuid;

	CoCreateGuid(&uuid);
	SDL_memcpy(&guid.data[0], &uuid.Data1, sizeof(uuid.Data1));
	SDL_memcpy(&guid.data[4], &uuid.Data2, sizeof(uuid.Data2));
	SDL_memcpy(&guid.data[6], &uuid.Data3, sizeof(uuid.Data3));
	SDL_memcpy(&guid.data[8], &uuid.Data4, sizeof(uuid.Data4));
#elif __APPLE__
	CFUUIDRef uuid_ref = CFUUIDCreate(NULL);
	CFUUIDBytes uuid_bytes = CFUUIDGetUUIDBytes(uuid_ref);
	CFRelease(uuid_ref);

	SDL_memcpy(&guid.data, &uuid_bytes, sizeof(guid.data));
#else
#endif

	rw = SDL_RWFromFile(path, "w+b");
	SDL_RWwrite(rw, &guid.data, sizeof(guid.data), 1);
	SDL_RWclose(rw);

#ifdef DEBUG
	SDL_GUIDToString(guid, guid_str, sizeof(guid_str));
	SDL_Log("UUID: %s\n", guid_str);
#endif

	return guid;
}
