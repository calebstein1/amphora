#include "engine/internal/input.h"
#include "engine/internal/render.h"

#include "config.h"

/* Private function prototypes */
Uint64 rotate_left(Uint64 n, Uint32 c); /* Rotate the bits of n to the left by c bits (MSB becomes LSB) */

/* File-scoped variables */
static SDL_GameController *controllers[MAX_CONTROLLERS];
static SDL_Keycode key1[] = {
#define KMAP(action, key1, key2, controller) key1,
    	ACTIONS
#undef KMAP
};
static SDL_Keycode key2[] = {
#define KMAP(action, key1, key2, controller) key2,
	ACTIONS
#undef KMAP
};
static SDL_GameControllerButton controller_buttons[] = {
#define KMAP(action, key1, key2, controller) controller,
	ACTIONS
#undef KMAP
};

/*
 * Internal functions
 */

void
add_controller(Sint32 idx) {
	Uint8 i;

	for (i = 0; i < MAX_CONTROLLERS; i++) {
		if (!controllers[i]) {
			controllers[i] = SDL_GameControllerOpen(idx);
			break;
		}
	}
	SDL_Log("Added controller %d to slot %d\n", SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controllers[idx])), i);
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
	SDL_Log("Removed joystick %d from slot %d\n", id, i);
}

void
cleanup_controllers(void) {
	Sint32 i;

	for (i = 0; i < MAX_CONTROLLERS; i++) {
		if (SDL_IsGameController(SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controllers[i])))) {
			SDL_GameControllerClose(controllers[i]);
		}
	}
}

void
handle_keydown(union input_state_u *key_actions, const SDL_Event *e) {
	Uint32 i;

	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->key.keysym.sym == key1[i]) {
			key_actions->bits |= (1 << i);
			return;
		}
	}
	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->key.keysym.sym == key2[i]) {
			key_actions->bits |= (1 << i);
			return;
		}
	}
}

void
handle_keyup(union input_state_u *key_actions, const SDL_Event *e) {
	Uint32 i;
	Uint64 mask = 0xfffffffffffffffe;

	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->key.keysym.sym == key1[i]) {
			key_actions->bits &= (rotate_left(mask, i));
			return;
		}
	}
	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->key.keysym.sym == key2[i]) {
			key_actions->bits &= (rotate_left(mask, i));
			return;
		}
	}
}

void
handle_gamepad_down(union input_state_u *key_actions, const SDL_Event *e) {
	Uint32 i;

	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->cbutton.button == controller_buttons[i]) {
			key_actions->bits |= (1 << i);
			return;
		}
	}
}

void
handle_gamepad_up(union input_state_u *key_actions, const SDL_Event *e) {
	Uint32 i;
	Uint64 mask = 0xfffffffffffffffe;

	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->cbutton.button == controller_buttons[i]) {
			key_actions->bits &= (rotate_left(mask, i));
			return;
		}
	}
}

/*
 * Private functions
 */

Uint64
rotate_left(Uint64 n, Uint32 c) {
	if (c == 0) return n;

	return (n << c) | (n >> (64 - c));
}
