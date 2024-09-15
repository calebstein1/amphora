#ifndef UNTITLED_PLATFORMER_GAME_LOOP_H
#define UNTITLED_PLATFORMER_GAME_LOOP_H

#include "engine/input.h"

#include "config.h"

void game_init(void); /* One-time init tasks */
void game_loop(unsigned long int frame, const input_state *key_actions, struct save_data_t *save_data); /* Main gameplay loop, runs once per frame */

#endif /* UNTITLED_PLATFORMER_GAME_LOOP_H */
