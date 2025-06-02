#ifndef UNTITLED_PLATFORMER_EVENTS_H
#define UNTITLED_PLATFORMER_EVENTS_H

#include "SDL.h"

#ifdef __cplusplus
extern "C" {
#endif
int Amphora_RegisterEvent(const char *name, void (*func)(void));
int Amphora_UnregisterEvent(const char *name);
#ifdef __cplusplus
}
#endif

#endif /* UNTITLED_PLATFORMER_EVENTS_H */
