#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "engine/game_loop.h"
#include "engine/input.h"
#include "engine/render.h"

/* Game globals */
struct sprite_slot_t *p_char = NULL;
struct sprite_slot_t *p_char2 = NULL;

void
game_init(void) {
	set_bg(0x2e, 0x34, 0x40);
	init_sprite_slot(&p_char, 3, 3, 4, 64, 64, 2, false);
	init_sprite_slot(&p_char2, 0, 2, 3, 48, 32, 0, false);
}

void
game_loop(Uint64 frame, const input_state *key_actions, struct save_data_t *save_data) {
	static Uint64 wave_timer = 0;
	static int last_pspr = 3;

	(void)save_data;

	if (key_actions->state.left) puts("You pressed left!");
	if (key_actions->state.right) puts("You pressed right!");
	if (key_actions->state.up) puts("You pressed up!");
	if (key_actions->state.down) puts("You pressed down!");

	if (frame - wave_timer > 15) {
		switch (p_char->num) {
			case 3:
				p_char->num = 6;
				last_pspr = 3;
				break;
			case 6:
				p_char->num = last_pspr == 9 ? 3 : 9;
				last_pspr = 6;
				break;
			case 9:
				p_char->num = 6;
				last_pspr = 9;
				break;
		}
		wave_timer = frame;
	}
}
