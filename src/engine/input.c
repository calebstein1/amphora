#include <SDL2/SDL.h>

#include "engine/input.h"

#include "config.h"

/* Private function prototypes */
unsigned int rotate_left(unsigned int n, int c); /* Rotate an the bits of an int to the left by n bits (MSB becomes LSB) */

/* File-scoped variables */
static SDL_Keycode key1[] = {
#define X(action, key1, ...) key1,
    	ACTIONS
#undef X
};

static SDL_Keycode key2[] = {
#define X(action, key1, key2) key2,
	ACTIONS
#undef X
};

void
handle_keydown(input_state *key_actions, const SDL_Event *e) {
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
handle_keyup(input_state *key_actions, const SDL_Event *e) {
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
	unsigned char carry;
	int i;

	for (i = 0; i < c; i++) {
		carry = (n >> (sizeof(int) - 1)) & 1;
		n <<= 1;
		if (carry) n |= 1;
	}

	return n;
}
