#include "engine/internal/events.h"
#include "engine/internal/input.h"
#include "engine/internal/render.h"

/*
 * Internal functions
 */

Uint32
event_loop(SDL_Event *e, union input_state_u *key_actions) {
	Vector2 window_size;

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
			case SDL_WINDOWEVENT:
				if (e->window.event != SDL_WINDOWEVENT_RESIZED) break;

				window_size = get_resolution();
				SDL_RenderSetLogicalSize(get_renderer(), window_size.x, window_size.y);
		}
	}

	return 0;
}
