#include <stdbool.h>
#include <stdio.h>

#include "engine/game_loop.h"
#include "engine/input.h"
#include "engine/sprites.h"

/* Game globals */
struct sprite_slot_t *p_char = NULL;
struct sprite_slot_t *p_char2 = NULL;

void
game_init(void) {
	init_sprite_slot(&p_char, 3, 2, 4, 64, 64, 0, false);
	init_sprite_slot(&p_char2, 1, 1, 1, 48, 32, 0, true);
}

void
game_loop(unsigned long int frame, const input_state *key_actions) {
	static unsigned long int anim_timer = 0;

	if (key_actions->state.left) puts("You pressed left!");
	if (key_actions->state.right) puts("You pressed right!");
	if (key_actions->state.up) puts("You pressed up!");
	if (key_actions->state.down) puts("You pressed down!");

	if (frame - anim_timer > 30) {
		p_char2->num = p_char2->num == 1 ? 2 : 1;
		anim_timer = frame;
	}
}
