#ifndef UNTITLED_PLATFORMER_CONFIG_H
#define UNTITLED_PLATFORMER_CONFIG_H

#include "engine/util.h"

#define GAME_TITLE "Amphora Demo"

#define FRAMERATE 60
#define WINDOW_X 1280
#define WINDOW_Y 720

/*
 * fullscreen: display fullscreen
 * fixed: window size fixed to WINDOW_X * WINDOW_Y
 * resizable: default window size of WINDOW_X * WINDOW_Y
 */
#define WINDOW_MODE resizable

/* Map actions to keys here following the example in the comment */
#define ACTIONS										\
	/* KMAP(action, key1, key2, controller) */		\
	KMAP(left, a, LEFT, DPAD_LEFT)					\
	KMAP(right, d, RIGHT, DPAD_RIGHT)				\
	KMAP(up, w, UP, DPAD_UP)						\
	KMAP(down, s, DOWN, DPAD_DOWN)					\
	KMAP(dash, LSHIFT, RSHIFT, B)		\
	KMAP(attack, SPACE, SPACE, A)					\
	KMAP(ko, x, x, INVALID)					\
	KMAP(zoom, m, m, LEFTSHOULDER)              \
	KMAP(expand, n, n, RIGHTSHOULDER)                         \
	KMAP(quit, ESCAPE, ESCAPE, INVALID)

#endif /* UNTITLED_PLATFORMER_CONFIG_H */
