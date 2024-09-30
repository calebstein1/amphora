#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "engine/game_loop.h"
#include "engine/input.h"
#include "engine/render.h"

/* Game globals */
struct sprite_slot_t *p_char = NULL;
struct sprite_slot_t *p_char2 = NULL;
bool walking = false;

void
game_init(void) {
	init_sprite_slot(&p_char, 1, 3, 4, 64, 64, false);
	init_sprite_slot(&p_char2, 16, 1, 1, 48, 32, false);
}

void
game_loop(Uint64 frame, const struct input_state_t *key_actions, struct save_data_t *save_data) {
	static Uint64 walk_timer = 0;
	static Vector2 camera_location;

	(void)save_data;

	walking=false;
	if (key_actions->left) {
		p_char2->flip = true;
		if (p_char2->x_subp <= 3) {
			p_char2->x--;
		}
		p_char2->x_subp -= 4;
		walking = true;
	}
	if (key_actions->right) {
		p_char2->flip = false;
		if(p_char2->x_subp >= 12) {
			p_char2->x++;
		}
		p_char2->x_subp += 4;
		walking = true;
	}

	if (frame - walk_timer > 15) {
		if (walking) {
			p_char2->num = p_char2->num == 16 ? 32 : 16;
		} else {
			p_char2->num = 16;
		}
		walk_timer = frame;
	}

	camera_location = get_sprite_center(p_char2);
	camera_location.x -= get_window_size().x / 2;
	camera_location.y = 0;
	set_camera(camera_location);
}
