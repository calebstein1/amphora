#include "engine/internal/input.h"
#include "engine/internal/render.h"

#include "config.h"

/* Private function prototypes */
Uint64 rotate_left(Uint64 n, Uint32 c); /* Rotate the bits of n to the left by c bits (MSB becomes LSB) */

/* File-scoped variables */
static SDL_GameController **controllers;
static Uint8 controllers_size;
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
find_controllers(void) {
	Sint32 i;

	if (SDL_NumJoysticks() > 0) {
		if (!((controllers = SDL_malloc(SDL_NumJoysticks() * sizeof(SDL_GameController *))))) {
			SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Unable to allocate space for game controllers\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Unable to allocate space for game controllers, attempting to continue without controller support", get_window());
			return;
		}
	}
	controllers_size = SDL_NumJoysticks();
	for (i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i)) {
			controllers[i] = SDL_GameControllerOpen(i);
		}
	}
}

void
add_controller(SDL_JoystickID id) {
	if (!((controllers = SDL_realloc(controllers, ++controllers_size * sizeof(SDL_GameController *))))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to reallocate space for controllers\n");
		return;
	}
	controllers[controllers_size - 1] = SDL_GameControllerOpen(id);
}

void
remove_controller(SDL_JoystickID id) {
	Sint32 i;

	for (i = 0; i < controllers_size; i++) {
		if (controllers[i] && id == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controllers[i]))) {
			SDL_GameControllerClose(controllers[i]);
			controllers[i] = NULL;
			controllers_size--;
			break;
		}
	}
	if (i < controllers_size) {
		controllers[i] = controllers[controllers_size];
		controllers[controllers_size] = NULL;
	}
	if (!((controllers = SDL_realloc(controllers, controllers_size * sizeof(SDL_GameController *))))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to reallocate space for controllers\n");
		return;
	}
}

void
cleanup_controllers(void) {
	Sint32 i;

	for (i = 0; i < controllers_size; i++) {
		if (SDL_IsGameController(SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controllers[i])))) {
			SDL_GameControllerClose(controllers[i]);
		}
	}
	SDL_free(controllers);
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
	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->cbutton.button == controller_buttons[i]) {
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
