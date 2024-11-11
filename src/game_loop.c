#include "engine/amphora.h"

enum player_state_e {
	idle,
	walk,
	atk,
	ko
};

/* Game globals */
AmphoraImage *player;
AmphoraImage *heart;
AmphoraImage *rotating_heart;
AmphoraMessage *hello;
AmphoraMessage *timer;
AmphoraMessage *stationary;
enum player_state_e p_state = idle;

void
end_player_attack(void) {
	p_state = idle;
	set_frameset(player, "Idle");
}

void
game_init(void) {
	const char *welcome_message = "Hello, and welcome to the Amphora demo!";
	const char *message = "I'm going to be fixed right here in place!";
	const SDL_Color font_color = { 0, 0, 0, 0xff };

	set_bg((SDL_Color){ 0x87, 0xce, 0xeb, 0xff });
	set_map("Overworld", 2);

	init_sprite_slot(&player, "Character", 96, 148, 2, false, false, 10);
	init_sprite_slot(&heart, "Objects", -96, 32, 2, false, true, 11);
	init_sprite_slot(&rotating_heart, "Objects", 128, 72, 3, false, false, -1);

	add_frameset(player, "Idle", 0, 17, 32, 48, 0, 0, 1, 0);
	add_frameset(player, "Walk", 32, 17, 32, 48, 0, 0, 6, 30);
	add_frameset(player, "Attack", 223, 145, 32, 48, 0, 0, 2, 15);
	add_frameset(player, "KO", 81, 355, 48, 32, 8, -8, 2, 30);

	add_frameset(heart, "Default", 63, 0, 16, 16, 0, 0, 1, 0);
	add_frameset(rotating_heart, "Rotate", 64, 129, 16, 16, 0, 0, 4, 15);

	create_string(&hello, "Roboto", 32, 16, 16, font_color, welcome_message, true);
	create_string(&timer, "Merriweather", 32, -16, 16, font_color, "0", true);
	create_string(&stationary, "Merriweather", 16, 76, 132, font_color, message, false);
}

void
game_loop(Uint64 frame, const struct input_state_t *key_actions, SaveData *save_data) {
	static Vector2 camera_location = {0, 0};
	static char timer_string[128] = "0";
	static Uint8 hello_ticker = 0;
	Uint8 player_speed;
	Vector2 screen_size;

	(void) save_data;

	camera_location = get_camera();

	if (key_actions->left && p_state != atk && p_state != ko) {
		p_state = walk;
		set_frameset(player, "Walk");
		player_speed = key_actions->dash ? 2 : 1;
		flip_sprite(player);
		set_frameset_delay(player, "Walk", key_actions->dash ? 15 : 30);
		move_sprite(player, -player_speed, 0);
	}
	if (key_actions->right && p_state != atk && p_state != ko) {
		p_state = walk;
		set_frameset(player, "Walk");
		player_speed = key_actions->dash ? 2 : 1;
		unflip_sprite(player);
		set_frameset_delay(player, "Walk", key_actions->dash ? 15 : 30);
		move_sprite(player, player_speed, 0);
	}
	if (key_actions->attack && p_state != atk) {
		if (p_state == ko) {
			p_state = idle;
			set_frameset(player, "Idle");
		} else {
			p_state = atk;
			play_oneshot(player, "Attack", end_player_attack);
		}
	}
	if (!key_actions->left && !key_actions->right && p_state == walk) {
		p_state = idle;
		set_frameset(player, "Idle");
	}
	if (key_actions->ko && p_state != ko) {
		p_state = ko;
		play_oneshot(player, "KO", NULL);
	}
	if (key_actions->zoom) {
		set_camera_zoom(150, 60);
	} else if (key_actions->expand) {
		set_camera_zoom(75, 60);
	} else {
		reset_camera_zoom(60);
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

	screen_size = get_render_logical_size();
	camera_location = get_sprite_center(player);
	camera_location.x -= (screen_size.x / 2);
	camera_location.y -= (screen_size.y / 2);
	set_camera(camera_location.x, camera_location.y);

	render_string(hello);
	render_string(timer);
	render_string(stationary);
}

void
game_shutdown(void) {
}
