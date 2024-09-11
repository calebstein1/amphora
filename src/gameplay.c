#include "gameplay.h"
#include "sprites.h"

/* Game globals */
struct sprite_slot_t *p_char = NULL;

void
game_init(void) {
	get_spriteslot(&p_char);
	p_char->spr_num = 1;
	p_char->spr_x = 64;
	p_char->spr_y = 64;
	p_char->spr_pal = 0;
}

void
game_loop(unsigned long int frame) {
	(void)frame;
}
