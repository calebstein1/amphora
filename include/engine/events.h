#ifndef UNTITLED_PLATFORMER_EVENTS_H
#define UNTITLED_PLATFORMER_EVENTS_H

#include "SDL.h"

#include "engine/input.h"

Uint32 event_loop(SDL_Event *e, union input_state_u *key_actions);

#endif /* UNTITLED_PLATFORMER_EVENTS_H */
