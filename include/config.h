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
#define WINDOW_MODE fullscreen

/*
 * min: the smaller window dimension will contain RESOLUTION pixels
 * max: the larger window dimension will contain RESOLUTION pixels
 */
#define RESOLUTION_MODE min
#define RESOLUTION 256

/* Colors used for drawing in 24-bit rgb format */
#define BLACK { 0x1c, 0x0f, 0x00 }
#define WHITE { 0xfc, 0xd5, 0xb3 }

/* Content paths */
#define SPRITESHEET_PATH "../content/img/sprites.asht"

#define ACTIONS					\
	/* KMAP(action, key1, key2) */		\
	KMAP(left, SDLK_a, SDLK_LEFT)		\
	KMAP(right, SDLK_d, SDLK_RIGHT)		\
	KMAP(up, SDLK_w, SDLK_UP)		\
	KMAP(down, SDLK_s, SDLK_DOWN)		\
	KMAP(dash, SDLK_LSHIFT, SDLK_RSHIFT)	\
	KMAP(quit, SDLK_ESCAPE, SDLK_ESCAPE)

#endif /* UNTITLED_PLATFORMER_CONFIG_H */
