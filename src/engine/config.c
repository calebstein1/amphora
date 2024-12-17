#include "engine/internal/config.h"
#include "engine/internal/db.h"

#include "config.h"

/*
 * TODO: Save preferences with a specific system ID so save files can be shared
 */

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
