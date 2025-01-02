#include "engine/internal/db.h"
#include "engine/internal/img.h"
#include "engine/internal/input.h"
#include "engine/internal/render.h"
#include "engine/internal/ttf.h"

#include "config.h"

/* Private function prototypes */
Uint64 rotate_left(Uint64 n, Uint32 c); /* Rotate the bits of n to the left by c bits (MSB becomes LSB) */

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

void
load_keymap(void) {
	sqlite3 *db = get_db();
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
	sqlite3_bind_text(stmt, 4, #key, -1, NULL);				\
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

bool
object_clicked(void *obj, int button, void (*callback)(void)) {
	int x, y;
	Uint32 flags;
	SDL_FRect *rect;
	struct amphora_object_generic_t *obj_generic = (struct amphora_object_generic_t *)obj;
	Camera camera = get_camera();

	switch (obj_generic->type) {
		case AMPH_OBJ_SPR:
			rect = &((AmphoraImage *)obj)->rectangle;
			break;
		case AMPH_OBJ_TXT:
			rect = &((AmphoraString *)obj)->rectangle;
			break;
		default:
			break;
	}

	flags = SDL_GetMouseState(&x, &y);
	if (flags != SDL_BUTTON(button)) return false;

	if (SDL_PointInFRect(&(SDL_FPoint){ (float)x + camera.x, (float)y + camera.y }, rect)) {
		if (callback) callback();
		return true;
	} else {
		return false;
	}
}

/*
 * Internal functions
 */

int
init_input(void) {
	sqlite3 *db = get_db();
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
get_key_actions_state(void) {
	return &key_actions.state;
}

void
add_controller(Sint32 idx) {
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
remove_controller(SDL_JoystickID id) {
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
cleanup_controllers(void) {
	Uint32 i;

	for (i = 0; i < MAX_CONTROLLERS; i++) {
		if (SDL_IsGameController(SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controllers[i])))) {
			SDL_GameControllerClose(controllers[i]);
		}
	}
}

void
handle_keydown(const SDL_Event *e) {
	Uint32 i;

	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->key.keysym.sym == keys[i]) {
			key_actions.bits |= (1LL << i);
			return;
		}
	}
}

void
handle_keyup(const SDL_Event *e) {
	Uint32 i;

	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->key.keysym.sym == keys[i]) {
			key_actions.bits &= (rotate_left(MASK, i));
			return;
		}
	}
}

void
handle_gamepad_down(const SDL_Event *e) {
	Uint32 i;

	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->cbutton.button == controller_buttons[i]) {
			key_actions.bits |= (1LL << i);
			return;
		}
	}
}

void
handle_gamepad_up(const SDL_Event *e) {
	Uint32 i;

	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->cbutton.button == controller_buttons[i]) {
			key_actions.bits &= (rotate_left(MASK, i));
			return;
		}
	}
}

Vector2f
handle_gamepad_joystick(const SDL_Event *e) {
	Uint8 axis = e->caxis.axis;
	Sint16 val = e->caxis.value;

	(void)axis;
	(void)val;

	return (Vector2f){0, 0 };
}

/*
 * Private functions
 */

Uint64
rotate_left(Uint64 n, Uint32 c) {
	if (c == 0) return n;

	return (n << c) | (n >> (64 - c));
}
