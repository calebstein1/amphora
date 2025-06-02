#ifndef EVENTS_INTERNAL_H
#define EVENTS_INTERNAL_H

#include "engine/events.h"
#include "engine/internal/input.h"

#define EVENT_BLOCK_SIZE 16

void Amphora_InitEvents(void);
void Amphora_DeInitEvents(void);
Uint32 Amphora_ProcessEventLoop(SDL_Event *e);
void Amphora_ProcessRegisteredEvents(void);

#endif /* EVENTS_INTERNAL_H */
