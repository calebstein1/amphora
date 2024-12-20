#include "engine/amphora.h"
#include "colors.h"

#include <sstream>
#include <string>
#include <vector>

#define DEFAULT_HEALTH 3

class HealthBar {
private:
	std::vector<AmphoraImage *> health_bar;
	int current_health;
public:
	HealthBar() {
		current_health = 0;
	}

	void increase_health() {
		current_health++;
		health_bar.push_back(nullptr);
		create_sprite(&health_bar.back(), "Objects", -96 - (Sint32)(32 * (health_bar.size() - 1)), 24, 2, false, true, 1000);
		add_frameset(health_bar.back(), "Default", 63, 0, 16, 16, 0, 0, 1, 0);
	}

	void decrease_health() {
		if (current_health == 0) return;

		current_health--;
		free_sprite(&health_bar.back());
		health_bar.pop_back();
	}

	[[nodiscard]] int get_health() const {
		return current_health;
	}
};

enum player_state_e {
	idle,
	walk,
	atk,
	ko
};

/* Game globals */
AmphoraImage *player;
AmphoraImage *rotating_heart;
HealthBar health_bar;
AmphoraString *hello;
AmphoraString *timer;
AmphoraString *stationary;
AmphoraString *coords;
enum player_state_e player_state = idle;

void
game_init() {
	int i;
	const std::string welcome_message = "Hello, and welcome to the Amphora demo!";
	const std::string message = "I'm going to be fixed right here in place!";

	set_bg(sky);
	set_map("Grassland", 2);
	set_music("forest");

	create_sprite(&player, "Character", (Sint32)get_number_value("x", 96), (Sint32)get_number_value("y", 148), 2, get_number_value("flip", false), false, 101);
	create_sprite(&rotating_heart, "Objects", 128, 72, 3, false, false, -1);
	for (i = 0; i < (int)get_number_value("health", DEFAULT_HEALTH); i++) {
		health_bar.increase_health();
	}

	add_frameset(player, "Idle", 0, 17, 32, 48, 0, 0, 1, 0);
	add_frameset(player, "Walk", 32, 17, 32, 48, 0, 0, 6, 250);
	add_frameset(player, "Attack", 223, 145, 32, 48, 0, 0, 2, 250);
	add_frameset(player, "KO", 81, 355, 48, 32, 8, -8, 2, 500);

	add_frameset(rotating_heart, "Rotate", 64, 129, 16, 16, 0, 0, 4, 15);

	create_string(&hello, "Roboto", 32, 16, 16, 1000, black, welcome_message.c_str(), true);
	create_string(&timer, "Merriweather", 32, -16, 16, 1000, black, "0", true);
	create_string(&stationary, "Merriweather", 16, 76, 132, -1, black, message.c_str(), false);
	create_string(&coords, "Roboto", 32, 16, -16, 1000, black, "0, 0", true);

	set_camera_target(player);
}

void
game_loop(Uint64 frame, const input_state_t *key_actions) {
	static Uint8 hello_ticker = 0;
	static Uint64 damage_cooldown = 0;
	std::stringstream timer_stream;
	std::stringstream coords_stream;
	Uint8 player_speed;

	play_music(500);

	if (health_bar.get_health() <= 0 && player_state != ko) {
		player_state = ko;
		play_oneshot(player, "KO", nullptr);
	}

	set_frameset_delay(player, "Walk", key_actions->dash ? 166 : 250);
	if (key_actions->left && player_state != atk && player_state != ko) {
		player_state = walk;
		set_frameset(player, "Walk");
		player_speed = key_actions->dash ? 2 : 1;
		flip_sprite(player);
		move_sprite(player, -player_speed, 0);
		play_sfx("leaves01", 1, 0);
	}
	if (key_actions->right && player_state != atk && player_state != ko) {
		player_state = walk;
		set_frameset(player, "Walk");
		player_speed = key_actions->dash ? 2 : 1;
		unflip_sprite(player);
		move_sprite(player, player_speed, 0);
		play_sfx("leaves02", 1, 0);
	}
	if (key_actions->up && player_state != atk && player_state != ko) {
		player_state = walk;
		set_frameset(player, "Walk");
		player_speed = key_actions->dash ? 2 : 1;
		move_sprite(player, 0, -player_speed);
		play_sfx("leaves01", 1, 0);
	}
	if (key_actions->down && player_state != atk && player_state != ko) {
		player_state = walk;
		set_frameset(player, "Walk");
		player_speed = key_actions->dash ? 2 : 1;
		move_sprite(player, 0, player_speed);
		play_sfx("leaves02", 1, 0);
	}
	if (key_actions->attack && player_state != atk && player_state != ko) {
		player_state = atk;
		play_oneshot(player, "Attack", []{
			player_state = idle;
			set_frameset(player, "Idle");
		});
	}
	if (!key_actions->left && !key_actions->right && !key_actions->up && !key_actions->down && player_state == walk) {
		player_state = idle;
		set_frameset(player, "Idle");
	}
	if (key_actions->damage && frame - damage_cooldown > 30) {
		damage_cooldown = frame;
		health_bar.decrease_health();
	}
	if (key_actions->heal && frame - damage_cooldown > 30) {
		damage_cooldown = frame;
		health_bar.increase_health();
		if (player_state == ko) {
			player_state = idle;
			set_frameset(player, "Idle");
		}
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

	coords_stream << get_sprite_position(player).x << ", " << get_sprite_position(player).y;
	update_string_text(&coords, coords_stream.str().c_str());

	if (get_sprite_position(player).y > 730) {
		reorder_sprite(player, 301);
	} else if (get_sprite_position(player).y > 200) {
		reorder_sprite(player, 201);
	} else {
		reorder_sprite(player, 101);
	}

	if (frame % get_framerate() == 0) {
		timer_stream << frame / get_framerate();
		update_string_text(&timer, timer_stream.str().c_str());
	}

	if (IS_EVEN(frame) && hello_ticker < get_string_length(hello)) {
		update_string_n(&hello, ++hello_ticker);
	}
}

void
game_shutdown() {
	Vector2 player_pos = get_sprite_position(player);
	save_number_value("x", player_pos.x);
	save_number_value("y", player_pos.y);
	save_number_value("flip", is_flipped(player));
	save_number_value("health", health_bar.get_health());
}
