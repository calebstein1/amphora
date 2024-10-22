#ifndef UNTITLED_PLATFORMER_INPUT_H
#define UNTITLED_PLATFORMER_INPUT_H

#include "SDL.h"

#include "config.h"

struct input_state_t {
#define KMAP(action, ...) bool action : 1;
	ACTIONS
#undef KMAP
};

#endif /* UNTITLED_PLATFORMER_INPUT_H */
