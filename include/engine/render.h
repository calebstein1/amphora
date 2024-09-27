#ifndef UNTITLED_PLATFORMER_RENDER_H
#define UNTITLED_PLATFORMER_RENDER_H

#define MAX_SPRITES_ON_SCREEN 256
#define SPR_NUM_PIXELS 64
#define SPR_NUM_BYTES 16
#define SPR_SIDE 8
#define SUBPIXEL_STEPS 16
#define PALETTE_SIZE 4

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "engine/util.h"

#define A_OPAQUE 0xff
#define A_TRANSPARENT 0

struct sprite_slot_t {
	unsigned int num; /* The sprite to draw from the spritesheet */
	short int x_size; /* The number of horizontal tiles in the sprite */
	short int y_size; /* The number of vertical tiles in the sprite */
	int x; /* The sprite's x position */
	int y; /* The sprite's y position */
	Uint8 x_subp : 4; /* The sprite's x subpixel position */
	Uint8 y_subp : 4; /* The sprite's y subpixel position */
	Uint8 pal; /* The sprite's color palette */
	Uint8 alpha;
	bool flip : 1; /* Whether or not the sprite should be flipped horizontally */
	bool reserved : 1; /* Whether the sprite slot is reserved or free */
	bool display : 1; /* Whether the sprite should be drawn or not */
};

struct color_t {
	Uint8 r;
	Uint8 g;
	Uint8 b;
};

int init_render(void); /* Run once to load spritesheet and palette data */
void cleanup_render(void); /* Free allocated memory for spritesheet, palettes, and colors */
void set_bg(Uint8 r, Uint8 g, Uint8 b); /* Set the screen background color */
struct color_t get_bg(void); /* Get the current screen backgroundn color */
void clear_bg(SDL_Renderer *renderer); /* Clear the screen and fill with the background color */
void draw_all_sprites(SDL_Renderer *renderer, int p_size); /* Draw all active sprite slots */
/* Reserve a sprite slot and get a pointer to it */
struct sprite_slot_t *reserve_sprite_slot(struct sprite_slot_t **spr);
/* Reserve a sprite slot and initialize it with default values */
struct sprite_slot_t *init_sprite_slot(struct sprite_slot_t **spr, unsigned int num, short int x_size, short int y_size, int x, int y, Uint8 pal, Uint8 alpha, bool flip);
/* Display the supplied sprite_slot */
void show_sprite(struct sprite_slot_t *spr);
/* Hide a sprite slot without releasing it */
void hide_sprite(struct sprite_slot_t *spr);
/* Release a sprite slot for reuse */
void *release_sprite_slot(struct sprite_slot_t **spr);

#endif /* UNTITLED_PLATFORMER_RENDER_H */
