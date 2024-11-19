#include "engine/internal/db.h"
#include "engine/internal/input.h"
#include "engine/internal/render.h"

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
static SDL_Keycode key[ACTION_COUNT];
static SDL_GameControllerButton controller_buttons[ACTION_COUNT];

void
load_keymap(void) {
	get_key_map_or_default(action_names, key, controller_buttons);
}

/*
 * Internal functions
 */

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
		if (e->key.keysym.sym == key[i]) {
			key_actions.bits |= (1LL << i);
			return;
		}
	}
}

void
handle_keyup(const SDL_Event *e) {
	Uint32 i;

	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->key.keysym.sym == key[i]) {
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

Vector2
handle_gamepad_joystick(const SDL_Event *e) {
	Uint8 axis = e->caxis.axis;
	Sint16 val = e->caxis.value;

	(void)axis;
	(void)val;

	return (Vector2){ 0, 0 };
}

/*
 * Private functions
 */

Uint64
rotate_left(Uint64 n, Uint32 c) {
	if (c == 0) return n;

	return (n << c) | (n >> (64 - c));
}
