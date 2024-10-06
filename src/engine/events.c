#include <stdbool.h>

#include "engine/events.h"
#include "engine/input.h"
#include "engine/render.h"
#include "engine/util.h"

Uint32
event_loop(SDL_Event *e, union input_state_u *key_actions,
	   int *win_size_x, int *win_size_y, SDL_Renderer *renderer) {
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

				SDL_GetRendererOutputSize(renderer, win_size_x, win_size_y);
				set_pixel_size(RESOLUTION_MODE ?
					     MAX_OF(*win_size_x, *win_size_y) / RESOLUTION :
					     MIN_OF(*win_size_x, *win_size_y) / RESOLUTION);
				set_window_size((Vector2){ *win_size_x, *win_size_y });
				break;
		}
	}

	return 0;
}
