#include "engine/game_loop.h"
#include "engine/img.h"
#include "engine/input.h"
#include "engine/render.h"
#include "engine/ttf.h"
#include "engine/util.h"

/* Game globals */
AmphoraMessage *hello;
AmphoraMessage *timer;
AmphoraMessage *stationary;

void
game_init(void) {
	const char *welcome_message = "Hello, and welcome to the Amphora demo!";
	const char *stationary_message = "I'm going to be fixed right here in place!";
	SDL_Color font_color = { 0xff, 0xff, 0xff, 0xff };

	create_string(&hello, Roboto, 32, 16, 16, font_color, welcome_message);
	create_string(&timer, Merriweather, 32, -16, 16, font_color, "0");
	create_stationary_string(&stationary, Merriweather, 16, 76, 132, font_color, stationary_message);
}

void
game_loop(Uint64 frame, const struct input_state_t *key_actions, SaveData *save_data) {
	static Vector2 camera_location = { 0, 0 };
	static char timer_string[128] = "0";
	static Uint8 hello_ticker = 0;

	(void)save_data;

	camera_location = get_camera();

	if (key_actions->left) {
		camera_location.x--;
	}
	if (key_actions->right) {
		camera_location.x++;
	}
	if (key_actions->up) {
		camera_location.y--;
	}
	if (key_actions->down) {
		camera_location.y++;
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

	set_camera(camera_location.x, camera_location.y);
}

void
game_shutdown(void) {
}
