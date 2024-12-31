#include "engine/internal/db.h"
#include "engine/internal/save_data.h"

int
save_number_value(const char *attribute, double value) {
	sqlite3 *db = get_db();
	sqlite3_stmt *stmt;
	const char *sql = "INSERT OR REPLACE INTO save_data (attribute, value) VALUES (?, ?);";

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, attribute, -1, NULL);
	sqlite3_bind_double(stmt, 2, value);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 0;
}

int
save_string_value(const char *attribute, const char *value) {
	sqlite3 *db = get_db();
	sqlite3_stmt *stmt;
	const char *sql = "INSERT OR REPLACE INTO save_data (attribute, value) VALUES (?, ?);";

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, attribute, -1, NULL);
	sqlite3_bind_text(stmt, 2, value, -1, NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	return 0;
}

double
get_number_value(const char *attribute, double default_value) {
	sqlite3 *db = get_db();
	sqlite3_stmt *stmt;
	const char *sql = "SELECT value FROM save_data WHERE attribute=?";
	double val;

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, attribute, -1, NULL);
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return default_value;
	}
	val = sqlite3_column_double(stmt, 0);
	sqlite3_finalize(stmt);

	return val;
}

int
get_string_value(const char *attribute, char **out_string) {
	sqlite3 *db = get_db();
	sqlite3_stmt *stmt;
	const char *sql = "SELECT value FROM save_data WHERE attribute=?";
	const unsigned char *val;

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
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
init_save(void) {
	sqlite3 *db = get_db();
	const char *sql = "CREATE TABLE IF NOT EXISTS save_data("
			  "attribute TEXT PRIMARY KEY NOT NULL,"
			  "value ANY);";
	char *err_msg;
	sqlite3_exec(db, sql, NULL, NULL, &err_msg);
	if (err_msg) {
		SDL_Log("%s\n", err_msg);
		return -1;
	}

	return 0;
}
