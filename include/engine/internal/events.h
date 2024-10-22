#ifndef EVENTS_INTERNAL_H
#define EVENTS_INTERNAL_H

#include "engine/events.h"
#include "engine/internal/input.h"

Uint32 event_loop(SDL_Event *e, union input_state_u *key_actions);

#endif /* EVENTS_INTERNAL_H */
