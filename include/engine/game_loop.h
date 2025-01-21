#ifndef UNTITLED_PLATFORMER_GAME_LOOP_H
#define UNTITLED_PLATFORMER_GAME_LOOP_H

#include "SDL.h"

#include "engine/input.h"

#ifdef __cplusplus
extern "C" {
#endif
void Amphora_GameInit(void); /* One-time init tasks */
void Amphora_GameLoop(Uint64 frame, const struct input_state_t *key_actions); /* Main gameplay loop, runs once per frame */
void Amphora_GameShutdown(void); /* One-time game shutdown tasks */
#ifdef __cplusplus
}
#endif

#endif /* UNTITLED_PLATFORMER_GAME_LOOP_H */
