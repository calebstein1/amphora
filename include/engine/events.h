#ifndef UNTITLED_PLATFORMER_EVENTS_H
#define UNTITLED_PLATFORMER_EVENTS_H

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "engine/input.h"

void event_loop(SDL_Event *e, bool *running, unsigned short *pixel_size, input_state *key_actions,
		int *win_size_x, int *win_size_y, SDL_Window *win);

#endif /* UNTITLED_PLATFORMER_EVENTS_H */
