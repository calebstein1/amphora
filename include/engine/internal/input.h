#ifndef INPUT_INTERNAL_H
#define INPUT_INTERNAL_H

#include "engine/input.h"

union input_state_u {
	struct input_state_t state; /* Individual flags for the named input actions */
	unsigned int bits; /* The raw input bitfield */
};

enum input_actions {
#define KMAP(action, ...) action,
	ACTIONS
#undef KMAP
	ACTION_COUNT
};

_Static_assert(ACTION_COUNT <= 64, "Cannot define more than 64 actions");

void find_controllers(void);
void add_controller(SDL_JoystickID id);
void remove_controller(SDL_JoystickID id);
void cleanup_controllers(void);
void handle_keydown(union input_state_u *key_actions, const SDL_Event *e);
void handle_keyup(union input_state_u *key_actions, const SDL_Event *e);

#endif /* INPUT_INTERNAL_H */
