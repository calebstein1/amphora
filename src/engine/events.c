#include "engine/internal/events.h"
#include "engine/internal/input.h"
#include "engine/internal/render.h"

/*
 * Internal functions
 */

Uint32
event_loop(SDL_Event *e) {
	Vector2 window_size;

	while (SDL_PollEvent(e)) {
		switch (e->type) {
			case SDL_QUIT:
				return e->type;
			case SDL_KEYDOWN:
				handle_keydown(e);
				break;
			case SDL_KEYUP:
				handle_keyup(e);
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				handle_gamepad_down(e);
				break;
			case SDL_CONTROLLERBUTTONUP:
				handle_gamepad_up(e);
				break;
			case SDL_CONTROLLERDEVICEADDED:
				add_controller(e->cdevice.which);
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				remove_controller(e->cdevice.which);
				break;
			case SDL_WINDOWEVENT:
				if (e->window.event != SDL_WINDOWEVENT_RESIZED) break;

				window_size = get_resolution();
				SDL_RenderSetLogicalSize(get_renderer(), window_size.x, window_size.y);
		}
	}

	return 0;
}
