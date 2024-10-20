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

/* Content paths */
#define SPRITESHEET_PATH "../content/img/sprites.asht"

/* Map actions to keys here following the example in the comment */
#define ACTIONS					\
	/* KMAP(action, key1, key2) */		\
	KMAP(left, SDLK_a, SDLK_LEFT)		\
	KMAP(right, SDLK_d, SDLK_RIGHT)		\
	KMAP(up, SDLK_w, SDLK_UP)		\
	KMAP(down, SDLK_s, SDLK_DOWN)		\
	KMAP(dash, SDLK_LSHIFT, SDLK_RSHIFT)	\
	KMAP(quit, SDLK_ESCAPE, SDLK_ESCAPE)

/* Load image resources */
#define IMAGES\
	/* LOADIMG(name, path) */				\
	LOADIMG(Character, "../content/img/character.png")	\
	LOADIMG(Objects, "../content/img/objects.png")		\
	LOADIMG(Overworld, "../content/img/overworld.png")

/* Load TTF fonts */
#define ENABLE_FONTS
#define FONTS										\
	/* LOADFONT(name, path) */							\
	LOADFONT(Merriweather, "../content/font/Merriweather/Merriweather-Regular.ttf")	\
	LOADFONT(Roboto, "../content/font/Roboto/Roboto-Regular.ttf")

#endif /* UNTITLED_PLATFORMER_CONFIG_H */
