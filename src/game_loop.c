#include <stdbool.h>

#include "engine/game_loop.h"
#include "engine/input.h"
#include "engine/render.h"

#define NUM_PLAYER_IDLE_FRAMES 2

/* Game globals */
struct sprite_slot_t *p_char = NULL;
struct sprite_slot_t *building = NULL;

int player_idle_frames[NUM_PLAYER_IDLE_FRAMES] = { 1, 3 };
int player_idle_idx = 0;
bool walking = false;

void
game_init(void) {
	init_sprite_slot(&p_char, player_idle_frames[player_idle_idx], 2, 4, 24, 196, false, 10);
	init_sprite_slot(&building, 12, 4, 8, 96, 148, false, -1);
}

void
game_loop(Uint64 frame, const struct input_state_t *key_actions, struct save_data_t *save_data) {
	static Point camera_location;
	static Uint64 idle_anim = 0;
	Uint8 p_movement_speed = 4;

	(void)save_data;

	walking = false;
	if (key_actions->dash) {
		p_movement_speed = 8;
	}
	if (key_actions->left) {
		p_char->flip = true;
		p_char->x -= p_movement_speed;
		walking = true;
	}
	if (key_actions->right) {
		p_char->flip = false;
		p_char->x += p_movement_speed;
		walking = true;
	}

	if (frame - idle_anim > 30) {
		if (++player_idle_idx >= NUM_PLAYER_IDLE_FRAMES) player_idle_idx = 0;
		p_char->num = player_idle_frames[player_idle_idx];
		idle_anim = frame;
	}

	camera_location = get_sprite_center(p_char);
	camera_location.x -= (get_game_window_size().x / 2);
	camera_location.y = 0;
	set_camera(camera_location.x, camera_location.y);
}
