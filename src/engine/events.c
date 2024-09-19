#include <stdbool.h>

#include <SDL2/SDL.h>

#include "engine/events.h"
#include "engine/input.h"
#include "engine/util.h"

Uint32
event_loop(SDL_Event *e, unsigned short *pixel_size, input_state *key_actions,
	   int *win_size_x, int *win_size_y, SDL_Window *win) {
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

				SDL_GetWindowSize(win, win_size_x, win_size_y);
				*pixel_size = RESOLUTION_MODE ?
					     MAX_OF(*win_size_x, *win_size_y) / RESOLUTION :
					     MIN_OF(*win_size_x, *win_size_y) / RESOLUTION;
				if (!*pixel_size) *pixel_size = 1;
				break;
		}
	}

	return 0;
}
