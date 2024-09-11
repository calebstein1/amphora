#ifndef UNTITLED_PLATFORMER_GAME_LOOP_H
#define UNTITLED_PLATFORMER_GAME_LOOP_H

#include "engine/input.h"

void game_init(void); /* One-time init tasks */
void game_loop(unsigned long int frame, const input_state *key_actions); /* Main gameplay loop */

#endif /* UNTITLED_PLATFORMER_GAME_LOOP_H */
