#include "engine/internal/save_data.h"

#include "config.h"

#include "sqlite3.h"

sqlite3 *save_db;

int
save_number_value(const char *attribute, Sint64 value) {
	sqlite3_stmt *stmt;
	const char *sql = "INSERT OR REPLACE INTO save_data (attribute, value) VALUES (?, ?);";

	sqlite3_prepare_v2(save_db, sql, (int)SDL_strlen(sql), &stmt, NULL);
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

	sqlite3_prepare_v2(save_db, sql, (int)SDL_strlen(sql), &stmt, NULL);
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

	sqlite3_prepare_v2(save_db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, attribute, -1, NULL);
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return default_value;
	}
	val = sqlite3_column_int64(stmt, 0);
	sqlite3_finalize(stmt);

	return val;
}

char *
get_string_value(const char *attribute) {
	return NULL;
}

/*
 * Internal functions
 */

int
init_save(void) {
	char *path = SDL_GetPrefPath(GAME_AUTHOR, GAME_TITLE);
	const char *filename = "save.db";
	size_t new_len = SDL_strlen(path) + SDL_strlen(filename) + 1;
	const char *sql = "CREATE TABLE IF NOT EXISTS save_data("
			  "attribute TEXT PRIMARY KEY NOT NULL,"
			  "value BLOB);";
	char *err_msg;

	path = SDL_realloc(path, new_len);
	SDL_strlcat(path, filename, new_len);
	sqlite3_open_v2(path, &save_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	sqlite3_exec(save_db, sql, NULL, NULL, &err_msg);
	if (err_msg) SDL_Log("%s\n", err_msg);
	SDL_free(path);

	return 0;
}

void
cleanup_save(void) {
	sqlite3_close_v2(save_db);
}
