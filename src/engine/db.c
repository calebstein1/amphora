#include "engine/internal/db.h"

#include "config.h"

#include "sqlite3.h"

sqlite3 *game_db;

int
save_number_value(const char *attribute, Sint64 value) {
	sqlite3_stmt *stmt;
	const char *sql = "INSERT OR REPLACE INTO save_data (attribute, value) VALUES (?, ?);";

	sqlite3_prepare_v2(game_db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, attribute, -1, NULL);
	sqlite3_bind_int64(stmt, 2, value);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 0;
}

int
save_string_value(const char *attribute, const char *value) {
	sqlite3_stmt *stmt;
	const char *sql = "INSERT OR REPLACE INTO save_data (attribute, value) VALUES (?, ?);";

	sqlite3_prepare_v2(game_db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, attribute, -1, NULL);
	sqlite3_bind_text(stmt, 2, value, -1, NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 0;
}

Sint64
get_number_value(const char *attribute, Sint64 default_value) {
	sqlite3_stmt *stmt;
	const char *sql = "SELECT value FROM save_data WHERE attribute=?";
	Sint64 val;

	sqlite3_prepare_v2(game_db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, attribute, -1, NULL);
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return default_value;
	}
	val = sqlite3_column_int64(stmt, 0);
	sqlite3_finalize(stmt);

	return val;
}

int
get_string_value(const char *attribute, char **out_string) {
	sqlite3_stmt *stmt;
	const char *sql = "SELECT value FROM save_data WHERE attribute=?";
	const unsigned char *val;

	sqlite3_prepare_v2(game_db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, attribute, -1, NULL);
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return -1;
	}
	val = sqlite3_column_text(stmt, 0);
	if (!(*out_string = SDL_malloc(sqlite3_column_bytes(stmt, 0) + 1))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not allocate space for string\n");
		sqlite3_finalize(stmt);
		return -1;
	}
	SDL_memcpy(*out_string, val, sqlite3_column_bytes(stmt, 0) + 1);
	sqlite3_finalize(stmt);

	return 0;
}

/*
 * Internal functions
 */

int
init_db(void) {
	char *path = SDL_GetPrefPath(GAME_AUTHOR, GAME_TITLE);
	const char *filename = "amphora.db";
	size_t new_len = SDL_strlen(path) + SDL_strlen(filename) + 1;
	const char *sql = "CREATE TABLE IF NOT EXISTS save_data("
			  "attribute TEXT PRIMARY KEY NOT NULL,"
			  "value ANY);"
			  "CREATE TABLE IF NOT EXISTS key_map("
			  "action TEXT PRIMARY KEY NOT NULL,"
			  "key INT,"
			  "gamepad INT);";
	char *err_msg;

	path = SDL_realloc(path, new_len);
	SDL_strlcat(path, filename, new_len);
	sqlite3_open_v2(path, &game_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	sqlite3_exec(game_db, sql, NULL, NULL, &err_msg);
	if (err_msg) SDL_Log("%s\n", err_msg);
	SDL_free(path);

	return 0;
}

void
cleanup_save(void) {
	sqlite3_close_v2(game_db);
}
