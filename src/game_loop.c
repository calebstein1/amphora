#include "engine/game_loop.h"
#include "engine/input.h"
#include "engine/sprites.h"

/* Game globals */
struct sprite_slot_t *p_char = NULL;

void
game_init(void) {
	get_spriteslot(&p_char);
	p_char->spr_num = 1;
	p_char->spr_x = 64;
	p_char->spr_y = 64;
	p_char->spr_pal = 0;
	p_char->display = true;
}

void
game_loop(unsigned long int frame, const input_state *key_actions) {
	(void)frame;
	if (key_actions->state.left) puts("You pressed left!");
	if (key_actions->state.right) puts("You pressed right!");
	if (key_actions->state.up) puts("You pressed up!");
	if (key_actions->state.down) puts("You pressed down!");
}
