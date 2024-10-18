#include "engine/game_loop.h"
#include "engine/input.h"
#include "engine/render.h"
#include "engine/ttf.h"
#include "engine/util.h"

#define NUM_PLAYER_IDLE_FRAMES 2

/* Game globals */
SpriteSlot *p_char = NULL;
SpriteSlot *building = NULL;
AmphoraMessage *hello;
AmphoraMessage *timer;
AmphoraMessage *stationary;

int player_idle_frames[NUM_PLAYER_IDLE_FRAMES] = { 1, 3 };
int player_idle_idx = 0;
bool walking = false;

void
game_init(void) {
	const char *welcome_message = "Hello, and welcome to the Amphora demo!";
	const char *stationary_message = "I'm going to be fixed right here above the tower!";

	init_sprite_slot(&p_char, player_idle_frames[player_idle_idx], 2, 4, 12, 120, false, 10);
	init_sprite_slot(&building, 12, 4, 8, 96, 148, false, -1);
	create_string(&hello, Roboto, 32, 16, 16, welcome_message);
	create_string(&timer, Merriweather, 32, -16, 16, "0");
	create_stationary_string(&stationary, Merriweather, 16, 76, 132, stationary_message);
}

void
game_loop(Uint64 frame, const struct input_state_t *key_actions, SaveData *save_data) {
	static Vector2 camera_location;
	static Uint64 idle_anim = 0;
	Sint32 p_movement_speed = 1;
	static char timer_string[128] = "0";
	static Uint8 hello_ticker = 0;

	(void)save_data;

	walking = false;
	if (key_actions->dash) {
		p_movement_speed = 2;
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
	if (key_actions->quit) {
		quit_game();
	}

	if (frame - idle_anim > 30) {
		if (++player_idle_idx >= NUM_PLAYER_IDLE_FRAMES) player_idle_idx = 0;
		p_char->num = player_idle_frames[player_idle_idx];
		idle_anim = frame;
	}

	if (frame % 60 == 0) {
		snprintf(timer_string, 128, "%d", SDL_atoi(timer_string) + 1);
		update_string_text(&timer, timer_string);
	}

	if (IS_EVEN(frame) && hello_ticker < get_string_length(hello)) {
		update_string_n(&hello, ++hello_ticker);
	}

	render_string(hello);
	render_string(timer);
	render_string(stationary);

	camera_location = get_sprite_center(p_char);
	camera_location.x -= (get_resolution().x / 2);
	camera_location.y = 0;
	set_camera(camera_location.x, camera_location.y);
}

void
game_shutdown(void) {
}
