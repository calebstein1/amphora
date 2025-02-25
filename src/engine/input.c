#include "engine/internal/db.h"
#include "engine/internal/img.h"
#include "engine/internal/input.h"
#include "engine/internal/render.h"
#include "engine/internal/ttf.h"

#include "config.h"

/* Private function prototypes */
static Uint32 rotate_mask_left(Uint32 c);

/* File-scoped variables */
static union input_state_u key_actions;
static SDL_GameController *controllers[MAX_CONTROLLERS];
static const char *action_names[] = {
#define KMAP(action, key, gamepad) #action,
	DEFAULT_KEYMAP
#undef KMAP
};
static SDL_Keycode keys[ACTION_COUNT];
static SDL_GameControllerButton controller_buttons[ACTION_COUNT];
static SDL_Keycode pressed_key;

void
Amphora_LoadKeymap(void) {
	sqlite3 *db = Amphora_GetDB();
	sqlite3_stmt *stmt;
	const char *sql_write = "INSERT INTO key_map (idx, action, key, key_name, gamepad, gamepad_name)"
				"VALUES (?, ?, ?, ?, ?, ?)";
	const char *sql_read = "SELECT key, gamepad FROM key_map ORDER BY idx";
	int sql_write_len = (int)SDL_strlen(sql_write);
	int i;

	/* Load the default keymap for any mappings that are missing */
#define KMAP(action, key, gamepad)						\
	sqlite3_prepare_v2(db, sql_write, sql_write_len, &stmt, NULL);		\
        sqlite3_bind_int(stmt, 1, ACTION_##action);				\
	sqlite3_bind_text(stmt, 2, #action, -1, NULL);				\
	sqlite3_bind_int(stmt, 3, SDLK_##key);					\
	sqlite3_bind_text(stmt, 4, SDL_GetKeyName(SDLK_##key), -1, NULL);	\
	sqlite3_bind_int(stmt, 5, SDL_CONTROLLER_BUTTON_##gamepad);		\
	sqlite3_bind_text(stmt, 6, #gamepad, -1, NULL);				\
	sqlite3_step(stmt);							\
	sqlite3_finalize(stmt);
	DEFAULT_KEYMAP
#undef KMAP

	sqlite3_prepare_v2(db, sql_read, (int)SDL_strlen(sql_read), &stmt, NULL);
	for (i = 0; i < ACTION_COUNT; i++) {
		if (sqlite3_step(stmt) != SQLITE_ROW) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to read keymap for action: %s\n", action_names[i]);
			sqlite3_finalize(stmt);
			continue;
		}
		keys[i] = sqlite3_column_int(stmt, 0);
		controller_buttons[i] = sqlite3_column_int(stmt, 1);
	}
	sqlite3_finalize(stmt);
}

void
Amphora_UpdateKeymap(const char *action, SDL_Keycode keycode) {
	sqlite3 *db = Amphora_GetDB();
	sqlite3_stmt *stmt;
	const char *sql = "UPDATE key_map SET key=?, key_name=? WHERE action=?;";
	const char *keyname = SDL_GetKeyName(keycode);

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, keycode);
	sqlite3_bind_text(stmt, 2, keyname, -1, NULL);
	sqlite3_bind_text(stmt, 3, action, -1, NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}

bool
Amphora_ObjectClicked(void *obj, int button, void (*callback)(void)) {
	int x, y;
	Uint32 flags;
	SDL_FRect *rect;
	IAmphoraObject *obj_generic = (IAmphoraObject *)obj;
	Camera camera = Ampohra_GetCamera();

	switch (obj_generic->type) {
		case AMPH_OBJ_SPR:
			rect = &((AmphoraImage *)obj)->rectangle;
			break;
		case AMPH_OBJ_TXT:
			rect = &((AmphoraString *)obj)->rectangle;
			break;
		default:
			return false;
	}

	flags = SDL_GetMouseState(&x, &y);
	if (flags != (Uint32)SDL_BUTTON(button)) return false;

	if (SDL_PointInFRect(&(SDL_FPoint){ (float)x + camera.x, (float)y + camera.y }, rect)) {
		if (callback) callback();
		return true;
	}

	return false;
}

bool
Amphora_ObjectHover(void *obj) {
	int x, y;
	SDL_FRect *rect;
	IAmphoraObject *obj_generic = (IAmphoraObject *)obj;
	Camera camera = Ampohra_GetCamera();

	switch (obj_generic->type) {
		case AMPH_OBJ_SPR:
			rect = &((AmphoraImage *)obj)->rectangle;
			break;
		case AMPH_OBJ_TXT:
			rect = &((AmphoraString *)obj)->rectangle;
			break;
		default:
			return false;
	}

	SDL_GetMouseState(&x, &y);

	return SDL_PointInFRect(&(SDL_FPoint){ (float)x + camera.x, (float)y + camera.y }, rect);
}

SDL_Keycode
Amphora_GetPressedKey(void) {
	return pressed_key;
}

char *
Amphora_GetActionKeyName(const char *action) {
	sqlite3 *db = Amphora_GetDB();
	sqlite3_stmt *stmt;
	const char *sql = "SELECT key_name FROM key_map WHERE action=?;";
	const char *key_name;
	char *key_name_r;

	sqlite3_prepare_v2(db, sql, (int)SDL_strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, action, -1, NULL);
	if (sqlite3_step(stmt) != SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return NULL;
	}
	key_name = (const char *)sqlite3_column_text(stmt, 0);
	if (!((key_name_r = SDL_malloc(SDL_strlen(key_name) + 1)))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate space for string\n");
		return NULL;
	}
	SDL_strlcpy(key_name_r, key_name, SDL_strlen(key_name) + 1);
	sqlite3_finalize(stmt);

	return key_name_r;
}

void
Amphora_ForEachAction(void (*callback)(const char *, int)) {
	int i;

	if (!callback) return;
	for (i = 0; i < ACTION_COUNT; i++) {
		callback(action_names[i], i);
	}
}

/*
 * Internal functions
 */

int
Amphora_InitInput(void) {
	sqlite3 *db = Amphora_GetDB();
	const char *sql = "CREATE TABLE IF NOT EXISTS key_map("
			  "idx INT NOT NULL PRIMARY KEY,"
			  "action TEXT NOT NULL,"
			  "key INT,"
			  "key_name TEXT,"
			  "gamepad INT,"
			  "gamepad_name TEXT);";
	char *err_msg;

	sqlite3_exec(db, sql, NULL, NULL, &err_msg);
	if (err_msg) {
		SDL_Log("%s\n", err_msg);
		return -1;
	}

	return 0;
}

struct input_state_t *
Amphora_GetKeyActionState(void) {
	return &key_actions.state;
}

void
Amphora_AddController(Sint32 idx) {
	Uint8 i;

	for (i = 0; i < MAX_CONTROLLERS; i++) {
		if (!controllers[i]) {
			controllers[i] = SDL_GameControllerOpen(idx);
			break;
		}
	}
#ifdef DEBUG
	SDL_Log("Added controller %d to slot %d\n", SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controllers[idx])), i);
#endif
}

void
Amphora_RemoveController(SDL_JoystickID id) {
	Uint8 i;

	for (i = 0; i < MAX_CONTROLLERS; i++) {
		if (controllers[i] && id == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controllers[i]))) {
			SDL_GameControllerClose(controllers[i]);
			controllers[i] = NULL;
			break;
		}
	}
#ifdef DEBUG
	SDL_Log("Removed joystick %d from slot %d\n", id, i);
#endif
}

void
Amphora_ReleaseControllers(void) {
	Uint32 i;

	for (i = 0; i < MAX_CONTROLLERS; i++) {
		if (SDL_IsGameController(SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controllers[i])))) {
			SDL_GameControllerClose(controllers[i]);
		}
	}
}

void
Amphora_HandleKeyDown(const SDL_Event *e) {
	Uint32 i;

	pressed_key = e->key.keysym.sym;
	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->key.keysym.sym == keys[i]) {
			key_actions.bits |= (1LL << i);
			return;
		}
	}
}

void
Amphora_HandleKeyUp(const SDL_Event *e) {
	Uint32 i;

	if (pressed_key == e->key.keysym.sym) pressed_key = 0;
	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->key.keysym.sym == keys[i]) {
			key_actions.bits &= (rotate_mask_left(i));
			return;
		}
	}
}

void
Amphora_HandleGamepadDown(const SDL_Event *e) {
	Uint32 i;

	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->cbutton.button == controller_buttons[i]) {
			key_actions.bits |= (1LL << i);
			return;
		}
	}
}

void
Amphora_HandleGamepadUp(const SDL_Event *e) {
	Uint32 i;

	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->cbutton.button == controller_buttons[i]) {
			key_actions.bits &= (rotate_mask_left(i));
			return;
		}
	}
}

Vector2f
Amphora_HandleJoystick(const SDL_Event *e) {
	Uint8 axis = e->caxis.axis;
	Sint16 val = e->caxis.value;

	(void)axis;
	(void)val;

	return (Vector2f){0, 0 };
}

/*
 * Private functions
 */

static Uint32
rotate_mask_left(Uint32 c) {
	if (c == 0) return MASK;

	return (MASK << c) | (MASK >> (32 - c));
}
