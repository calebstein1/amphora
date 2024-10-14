#include "engine/input.h"

#include "config.h"

/* Private function prototypes */
Uint64 rotate_left(Uint64 n, Uint32 c); /* Rotate the bits of n to the left by c bits (MSB becomes LSB) */

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
	Uint32 i;

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
	Uint32 i;
	Uint64 mask = 0xfffffffffffffffe;

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

Uint64
rotate_left(Uint64 n, Uint32 c) {
	if (c == 0) return n;

	return (n << c) | (n >> (64 - c));
}
