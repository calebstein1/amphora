#ifndef UNTITLED_PLATFORMER_CONFIG_H
#define UNTITLED_PLATFORMER_CONFIG_H

#include "engine/util.h"

#define GAME_TITLE "Amphora Demo"

#define FRAMERATE 60
#define WINDOW_X 800
#define WINDOW_Y 600

/*
 * fullscreen: display fullscreen
 * fixed: window size fixed to WINDOW_X * WINDOW_Y
 * resizable: default window size of WINDOW_X * WINDOW_Y
 */
#define WINDOW_MODE resizable

/*
 * min: the smaller window dimension will contain RESOLUTION pixels
 * max: the larger window dimension will contain RESOLUTION pixels
 */
#define RESOLUTION_MODE min
#define RESOLUTION 256

/* Content paths */
#define SPRITESHEET_PATH "content/img/sprites.chr"
#define USER_PALETTES_PATH "content/palettes/user_palettes.dat"
#define COLOR_TABLE_PATH "content/palettes/table.pal"

#define ACTIONS				\
	/* X(action, key1, key2) */	\
	KMAP(left, SDLK_a, SDLK_LEFT)	\
	KMAP(right, SDLK_d, SDLK_RIGHT)	\
	KMAP(up, SDLK_w, SDLK_UP)	\
	KMAP(down, SDLK_s, SDLK_DOWN)

/* The fields to be saved to the game's save file */
struct save_data_t {
	char player_name[256];
	unsigned int high_score;
};

#endif /* UNTITLED_PLATFORMER_CONFIG_H */
