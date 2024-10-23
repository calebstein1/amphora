#include "engine/amphora.h"

/* Game globals */
AmphoraImage *player;
AmphoraImage *heart;
AmphoraImage *rotating_heart;
AmphoraMessage *hello;
AmphoraMessage *timer;
AmphoraMessage *stationary;

enum player_state_e {
	idle,
	walk,
	atk
};

enum player_facing_e {
	n, s, e, w
};

void
game_init(void) {
	const char *welcome_message = "Hello, and welcome to the Amphora demo!";
	const char *message = "I'm going to be fixed right here in place!";
	const SDL_Color font_color = { 0, 0, 0, 0xff };

	set_bg((SDL_Color){ 0x87, 0xce, 0xeb, 0xff });
	init_sprite_slot(&player, Character, 96, 148, 2, false, false, 10);
	init_sprite_slot(&heart, Objects, -96, 32, 2, false, true, 11);
	init_sprite_slot(&rotating_heart, Objects, 128, 72, 3, false, false, -1);

	add_frameset(player, "IdleDown", 0, 5, 16, 24, 1, 0);
	add_frameset(player, "WalkDown", 0, 5, 16, 24, 4, 30);
	add_frameset(player, "IdleRight", 0, 37, 16, 24, 1, 0);
	add_frameset(player, "WalkRight", 0, 37, 16, 24, 4, 30);
	add_frameset(player, "IdleUp", 0, 69, 16, 24, 1, 0);
	add_frameset(player, "WalkUp", 0, 69, 16, 24, 4, 30);
	add_frameset(player, "IdleLeft", 0, 101, 16, 24, 1, 0);
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
	Vector2 screen_size = get_resolution();
	static enum player_state_e p_state = idle;
	static enum player_facing_e p_facing = s;

	(void)save_data;

	camera_location = get_camera();

	if (key_actions->dash) {
		player_speed = 2;
	}
	if (key_actions->left) {
		p_state = walk;
		set_frameset(player, "WalkLeft");
		move_sprite(player, -player_speed, 0);
		p_facing = w;
	}
	if (key_actions->right) {
		p_state = walk;
		set_frameset(player, "WalkRight");
		move_sprite(player, player_speed, 0);
		p_facing = e;
	}
	if (key_actions->up) {
		p_state = walk;
		set_frameset(player, "WalkUp");
		move_sprite(player, 0, -player_speed);
		p_facing = n;
	}
	if (key_actions->down) {
		p_state = walk;
		set_frameset(player, "WalkDown");
		move_sprite(player, 0, player_speed);
		p_facing = s;
	}
	if (!key_actions->left && !key_actions->right && !key_actions->up && !key_actions->down) {
		p_state = idle;
		switch (p_facing) {
			case n:
				set_frameset(player, "IdleUp");
				break;
			case s:
				set_frameset(player, "IdleDown");
				break;
			case e:
				set_frameset(player, "IdleRight");
				break;
			case w:
				set_frameset(player, "IdleLeft");
				break;
		}
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
