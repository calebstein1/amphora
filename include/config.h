#ifndef UNTITLED_PLATFORMER_CONFIG_H
#define UNTITLED_PLATFORMER_CONFIG_H

#define GAME_TITLE "Untitled Platformer"

#define FRAMERATE 60
#define WINDOW_X 800
#define WINDOW_Y 600
#define START_FULLSCREEN false

#define ACTIONS				\
	/* (action, key1, key2) */	\
	X(left, SDLK_a, SDLK_LEFT)	\
	X(right, SDLK_d, SDLK_RIGHT)	\
	X(up, SDLK_w, SDLK_UP)		\
	X(down, SDLK_s, SDLK_DOWN)

#endif /* UNTITLED_PLATFORMER_CONFIG_H */