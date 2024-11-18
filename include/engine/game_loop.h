#ifndef UNTITLED_PLATFORMER_GAME_LOOP_H
#define UNTITLED_PLATFORMER_GAME_LOOP_H

#include "SDL.h"

#include "engine/input.h"

#ifdef __cplusplus
extern "C" {
#endif
void game_init(void); /* One-time init tasks */
void game_loop(Uint64 frame, const struct input_state_t *key_actions); /* Main gameplay loop, runs once per frame */
void game_shutdown(void); /* One-time game shutdown tasks */
#ifdef __cplusplus
}
#endif

#endif /* UNTITLED_PLATFORMER_GAME_LOOP_H */
