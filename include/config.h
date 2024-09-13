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
#define RESOLUTION 128

#define ACTIONS				\
	/* X(action, key1, key2) */	\
	X(left, SDLK_a, SDLK_LEFT)	\
	X(right, SDLK_d, SDLK_RIGHT)	\
	X(up, SDLK_w, SDLK_UP)		\
	X(down, SDLK_s, SDLK_DOWN)

#endif /* UNTITLED_PLATFORMER_CONFIG_H */
