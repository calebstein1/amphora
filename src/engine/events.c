#include "engine/internal/events.h"
#include "engine/internal/input.h"

/*
 * Internal functions
 */

Uint32
event_loop(SDL_Event *e, union input_state_u *key_actions) {
	while (SDL_PollEvent(e)) {
		switch (e->type) {
			case SDL_QUIT:
				return e->type;
			case SDL_KEYDOWN:
				handle_keydown(key_actions, e);
				break;
			case SDL_KEYUP:
				handle_keyup(key_actions, e);
				break;
		}
	}

	return 0;
}
