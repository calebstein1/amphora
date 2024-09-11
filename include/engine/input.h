#ifndef UNTITLED_PLATFORMER_INPUT_H
#define UNTITLED_PLATFORMER_INPUT_H

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "config.h"

struct input_bitfield {
#define X(action, ...) bool action : 1;
	ACTIONS
#undef X
};

typedef union input_state {
	struct input_bitfield state; /* Individual flags for the named input actions */
	unsigned int bits; /* The raw input bitfield */
} input_state;

enum input_actions {
#define X(action, ...) action,
    	ACTIONS
#undef X
	ACTION_COUNT
};

void handle_keydown(input_state *key_actions, const SDL_Event *e);
void handle_keyup(input_state *key_actions, const SDL_Event *e);

#endif /* UNTITLED_PLATFORMER_INPUT_H */
