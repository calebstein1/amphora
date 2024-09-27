#include <SDL2/SDL.h>

#include "engine/input.h"

#include "config.h"

/* Private function prototypes */
unsigned int rotate_left(unsigned int n, int c); /* Rotate the bits of n to the left by c bits (MSB becomes LSB) */

/* File-scoped variables */
static SDL_Keycode key1[] = {
#define KMAP(action, key1, ...) key1,
    	ACTIONS
#undef KMAP
};

static SDL_Keycode key2[] = {
#define KMAP(action, key1, key2) key2,
	ACTIONS
#undef KMAP
};

void
handle_keydown(union input_state_u *key_actions, const SDL_Event *e) {
	int i;

	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->key.keysym.sym == key1[i]) {
			key_actions->bits |= (1 << i);
			return;
		}
	}
	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->key.keysym.sym == key2[i]) {
			key_actions->bits |= (1 << i);
			return;
		}
	}
}

void
handle_keyup(union input_state_u *key_actions, const SDL_Event *e) {
	int i;
	unsigned int mask = 0xfffffffe;

	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->key.keysym.sym == key1[i]) {
			key_actions->bits &= (rotate_left(mask, i));
			return;
		}
	}
	for (i = 0; i < ACTION_COUNT; i++) {
		if (e->key.keysym.sym == key2[i]) {
			key_actions->bits &= (rotate_left(mask, i));
			return;
		}
	}
}

/*
 * Private functions
 */

unsigned int
rotate_left(unsigned int n, int c) {
	int int_size = sizeof(unsigned int) * 8;

	if (c == 0) return n;
	c %= int_size;

	return (n << c) | (n >> (int_size - c));
}
