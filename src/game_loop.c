#include "engine/amphora.h"
#include "colors.h"

#define MAX_HEALTH 10
#define DEFAULT_HEALTH 3

enum player_state_e {
	idle,
	walk,
	atk,
	ko
};

/* Game globals */
AmphoraImage *player;
AmphoraImage *rotating_heart;
AmphoraImage *health_bar[MAX_HEALTH];
AmphoraString *hello;
AmphoraString *timer;
AmphoraString *stationary;
enum player_state_e player_state = idle;
int player_health;

void
end_player_attack(void) {
	player_state = idle;
	set_frameset(player, "Idle");
}

void
game_init(void) {
	int i;
	const char *welcome_message = "Hello, and welcome to the Amphora demo!";
	const char *message = "I'm going to be fixed right here in place!";
	const SDL_Color font_color = { 0, 0, 0, 0xff };

	player_health = (int)get_number_value("health", DEFAULT_HEALTH);

	set_bg(sky);
	set_map("Grassland", 2);

	create_sprite(&player, "Character", (Sint32)get_number_value("x", 96), (Sint32)get_number_value("y", 148), 2, get_number_value("flip", false), false, 10);
	create_sprite(&rotating_heart, "Objects", 128, 72, 3, false, false, -1);
	for (i = 0; i < MAX_HEALTH; i++) {
		create_sprite(&health_bar[i], "Objects", -96 - (32 * i), 24, 2, false, true, 11);
		add_frameset(health_bar[i], "Default", 63, 0, 16, 16, 0, 0, 1, 0);
	}

	add_frameset(player, "Idle", 0, 17, 32, 48, 0, 0, 1, 0);
	add_frameset(player, "Walk", 32, 17, 32, 48, 0, 0, 6, 15);
	add_frameset(player, "Attack", 223, 145, 32, 48, 0, 0, 2, 15);
	add_frameset(player, "KO", 81, 355, 48, 32, 8, -8, 2, 30);

	add_frameset(rotating_heart, "Rotate", 64, 129, 16, 16, 0, 0, 4, 15);

	create_string(&hello, "Roboto", 32, 16, 16, -1, font_color, welcome_message, true);
	create_string(&timer, "Merriweather", 32, -16, 16, -1, font_color, "0", true);
	create_string(&stationary, "Merriweather", 16, 76, 132, -1, font_color, message, false);
}

void
game_loop(Uint64 frame, const struct input_state_t *key_actions) {
	static Vector2 camera_location = { 0, 0 };
	static char timer_string[128] = "0";
	static Uint8 hello_ticker = 0;
	static Uint64 damage_cooldown = 0;
	Uint8 player_speed;
	Vector2 screen_size;
	int i;

	camera_location = get_camera();

	for (i = 0; i < MAX_HEALTH; i++) {
		if (i >= player_health) {
			hide_sprite(health_bar[i]);
		} else {
			show_sprite(health_bar[i]);
		}
	}
	if (player_health <= 0 && player_state != ko) {
		player_state = ko;
		play_oneshot(player, "KO", NULL);
	}

	set_frameset_delay(player, "Walk", key_actions->dash ? 10 : 15);
	if (key_actions->left && player_state != atk && player_state != ko) {
		player_state = walk;
		set_frameset(player, "Walk");
		player_speed = key_actions->dash ? 2 : 1;
		flip_sprite(player);
		move_sprite(player, -player_speed, 0);
	}
	if (key_actions->right && player_state != atk && player_state != ko) {
		player_state = walk;
		set_frameset(player, "Walk");
		player_speed = key_actions->dash ? 2 : 1;
		unflip_sprite(player);
		move_sprite(player, player_speed, 0);
	}
	if (key_actions->attack && player_state != atk) {
		if (player_state == ko) {
			player_health = DEFAULT_HEALTH;
			player_state = idle;
			set_frameset(player, "Idle");
		} else {
			player_state = atk;
			play_oneshot(player, "Attack", end_player_attack);
		}
	}
	if (!key_actions->left && !key_actions->right && player_state == walk) {
		player_state = idle;
		set_frameset(player, "Idle");
	}
	if (key_actions->damage && frame - damage_cooldown > 30) {
		damage_cooldown = frame;
		player_health--;
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
}

void
game_shutdown(void) {
	Vector2 player_pos = get_sprite_position(player);
	save_number_value("x", player_pos.x);
	save_number_value("y", player_pos.y);
	save_number_value("flip", is_flipped(player));
	save_number_value("health", player_health);
}
