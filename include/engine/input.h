#ifndef UNTITLED_PLATFORMER_INPUT_H
#define UNTITLED_PLATFORMER_INPUT_H

#include "SDL.h"

#include "config.h"

struct input_state_t {
#define KMAP(action, ...) SDL_bool action : 1;
	ACTIONS
#undef KMAP
};

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

void handle_keydown(union input_state_u *key_actions, const SDL_Event *e);
void handle_keyup(union input_state_u *key_actions, const SDL_Event *e);

#endif /* UNTITLED_PLATFORMER_INPUT_H */
