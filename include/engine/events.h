#ifndef UNTITLED_PLATFORMER_EVENTS_H
#define UNTITLED_PLATFORMER_EVENTS_H

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "engine/input.h"

Uint32 event_loop(SDL_Event *e, unsigned short *pixel_size, union input_state_u *key_actions,
		int *win_size_x, int *win_size_y, SDL_Window *win);

#endif /* UNTITLED_PLATFORMER_EVENTS_H */
