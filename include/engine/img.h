#ifndef UNTITLED_PLATFORMER_IMG_H
#define UNTITLED_PLATFORMER_IMG_H

#include "SDL.h"
#include "SDL_image.h"

#include "engine/util.h"
#include "engine/render.h"

#include "config.h"

enum images_e {
#define LOADIMG(name, path) name,
	IMAGES
#undef LOADIMG
	IMAGES_COUNT
};

typedef struct sprite_slot_t {
	Uint32 num; /* The sprite to draw from the spritesheet */
	Uint16 x_size; /* The number of horizontal tiles in the sprite */
	Uint16 y_size; /* The number of vertical tiles in the sprite */
	Sint32 x; /* The sprite's x position in 1/16 of a pixel */
	Sint32 y; /* The sprite's y position in 1/16 of a pixel */
	Sint32 order; /* The draw order of sprites, higher numbers on top */
	bool flip : 1; /* Whether or not the sprite should be flipped horizontally */
	bool display : 1; /* Whether the sprite should be drawn or not */
	bool garbage : 1; /* Whether the garbage collector should free the allocated memory */
	struct sprite_slot_t *next;
} SpriteSlot;

/* Gets the pixel position of the center of a sprite */
Vector2 get_sprite_center(const SpriteSlot *spr);
/* Allocate a sprite slot and initialize it with the supplied values */
SpriteSlot *init_sprite_slot(SpriteSlot **spr, Uint32 num, Uint16 x_size, Uint16 y_size, Sint32 x, Sint32 y, bool flip, Sint32 order);
/* Show the supplied sprite_slot if hidden */
void show_sprite(SpriteSlot *spr);
/* Hide a sprite without free it */
void hide_sprite(SpriteSlot *spr);
/* Free a sprite slot */
void *release_sprite_slot(SpriteSlot **spr);

#endif /* UNTITLED_PLATFORMER_IMG_H */
