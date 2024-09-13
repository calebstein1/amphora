#ifndef UNTITLED_PLATFORMER_SPRITES_H
#define UNTITLED_PLATFORMER_SPRITES_H

#define MAX_SPRITES_ON_SCREEN 256
#define SPRITESHEET_SIZE 0x4000
#define SPRITESHEET_PATH "content/img/sprites.chr"
#define SPR_NUM_PIXELS 64
#define SPR_NUM_BYTES 16
#define SPR_SIDE 8
#define SUBPIXEL_STEPS 16

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "engine/palette.h"
#include "engine/util.h"

struct sprite_slot_t {
	unsigned int num; /* The sprite to draw from the spritesheet */
	short int x_size;
	short int y_size;
	int x; /* The sprite's x position */
	int y; /* The sprite's y position */
	u8 x_subp : 4; /* The sprite's x subpixel position */
	u8 y_subp : 4; /* The sprite's y subpixel position */
	u8 pal; /* The sprite's color palette */
	bool flip : 1; /* Whether or not the sprite should be flipped horizontally */
	bool reserved : 1; /* Whether the sprite slot is reserved or free */
	bool display : 1; /* Whether the sprite should be drawn or not */
};

int init_spritesheet(void); /* Run once to load spritesheet and palette data */
void draw_all_sprites(SDL_Renderer *renderer, int p_size); /* Draw all active sprite slots */
/* Reserve a sprite slot and get a pointer to it */
struct sprite_slot_t *reserve_sprite_slot(struct sprite_slot_t **spr);
/* Reserve a sprite slot and initialize it with default values */
struct sprite_slot_t *init_sprite_slot(struct sprite_slot_t **spr, unsigned int num, short int x_size, short int y_size, int x, int y, u8 pal, bool flip);
/* Display the supplied sprite_slot */
void show_sprite(struct sprite_slot_t *spr);
/* Hide a sprite slot without releasing it */
void hide_sprite(struct sprite_slot_t *spr);
/* Release a sprite slot for reuse */
void *release_sprite_slot(struct sprite_slot_t **spr);

#endif /* UNTITLED_PLATFORMER_SPRITES_H */
