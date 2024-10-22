#include "engine/game_loop.h"
#include "engine/img.h"
#include "engine/input.h"
#include "engine/render.h"
#include "engine/ttf.h"
#include "engine/util.h"

/* Game globals */
AmphoraImage *player;
AmphoraImage *heart;
AmphoraImage *rotating_heart;
AmphoraMessage *hello;
AmphoraMessage *timer;
AmphoraMessage *stationary;
Vector2 screen_size;

void
game_init(void) {
	const char *welcome_message = "Hello, and welcome to the Amphora demo!";
	const char *message = "I'm going to be fixed right here in place!";
	const SDL_Color font_color = { 0xff, 0xff, 0xff, 0xff };

	screen_size = get_resolution();

	init_sprite_slot(&player, Character, 96, 148, 2, false, false, 10);
	init_sprite_slot(&heart, Objects, -96, 32, 2, false, true, 11);
	init_sprite_slot(&rotating_heart, Objects, 128, 72, 3, false, false, -1);

	add_frameset(player, "WalkDown", 0, 5, 16, 24, 4, 30);
	add_frameset(player, "WalkRight", 0, 37, 16, 24, 4, 30);
	add_frameset(player, "WalkUp", 0, 69, 16, 24, 4, 30);
	add_frameset(player, "WalkLeft", 0, 101, 16, 24, 4, 30);
	add_frameset(heart, "Default", 63, 0, 16, 16, 1, 0);
	add_frameset(rotating_heart, "Rotate", 64, 129, 16, 16, 4, 15);

	create_string(&hello, Roboto, 32, 16, 16, font_color, welcome_message, true);
	create_string(&timer, Merriweather, 32, -16, 16, font_color, "0", true);
	create_string(&stationary, Merriweather, 16, 76, 132, font_color, message, false);
}

void
game_loop(Uint64 frame, const struct input_state_t *key_actions, SaveData *save_data) {
	static Vector2 camera_location = { 0, 0 };
	static char timer_string[128] = "0";
	static Uint8 hello_ticker = 0;
	Uint8 player_speed = 1;

	(void)save_data;

	camera_location = get_camera();

	if (key_actions->dash) {
		player_speed = 2;
	}
	if (key_actions->left) {
		set_frameset(player, "WalkLeft");
		move_sprite(player, -player_speed, 0);
	}
	if (key_actions->right) {
		set_frameset(player, "WalkRight");
		move_sprite(player, player_speed, 0);
	}
	if (key_actions->up) {
		set_frameset(player, "WalkUp");
		move_sprite(player, 0, -player_speed);
	}
	if (key_actions->down) {
		set_frameset(player, "WalkDown");
		move_sprite(player, 0, player_speed);
	}
	if (key_actions->quit) {
		quit_game();
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

	camera_location = get_sprite_center(player);
	camera_location.x -= (screen_size.x / 2);
	camera_location.y -= (screen_size.y / 2);
	set_camera(camera_location.x, camera_location.y);
}

void
game_shutdown(void) {
}
