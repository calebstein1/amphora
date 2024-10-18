#ifndef UNTITLED_PLATFORMER_RENDER_H
#define UNTITLED_PLATFORMER_RENDER_H

#define SPR_NUM_PIXELS 64
#define SPR_NUM_BYTES 32
#define SPR_SIDE 8

#include "SDL.h"

#include "engine/util.h"

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

typedef struct color_t {
	Uint8 r;
	Uint8 g;
	Uint8 b;
} AmphoraColor;

typedef struct vector2_t {
    Sint32 x;
    Sint32 y;
} Vector2;

typedef Vector2 Camera;

/* One-time tasks to initialize the renderer */
int init_render(void);
/* Frees resources allocated for the renderer in anticipation of exit */
void cleanup_render(void);
/* Get the resolution */
Vector2 get_resolution(void);
/* Set the resolution */
void set_resolution(Uint32 res);
/* Get the current upper-left point of the camera */
Vector2 get_camera(void);
/* Set the location of the camera's top left point */
void set_camera(Sint32 x, Sint32 y);
/* Gets the current black color */
struct color_t get_black(void);
/* Gets the current white color */
struct color_t get_white(void);
/* Sets the black color */
void set_black(Uint8 r, Uint8 g, Uint8 b);
/* Sets the white color */
void set_white(Uint8 r, Uint8 g, Uint8 b);
/* Sets the background color to black */
void set_bg_black(void);
/* Sets the background color to white */
void set_bg_white(void);
/* Clear the screen and fill with the background color */
void clear_bg(SDL_Renderer *renderer);
/* Gets the pixel position of the center of a sprite */
Vector2 get_sprite_center(const SpriteSlot *spr);
/* Draw all active sprite slots and clean up freed sprites, called once per frame automatically */
void draw_all_sprites_and_gc(SDL_Renderer *renderer);
/* Allocate a sprite slot and initialize it with the supplied values */
SpriteSlot *init_sprite_slot(SpriteSlot **spr, Uint32 num, Uint16 x_size, Uint16 y_size, Sint32 x, Sint32 y, bool flip, Sint32 order);
/* Show the supplied sprite_slot if hidden */
void show_sprite(SpriteSlot *spr);
/* Hide a sprite without free it */
void hide_sprite(SpriteSlot *spr);
/* Free a sprite slot */
void *release_sprite_slot(SpriteSlot **spr);

#endif /* UNTITLED_PLATFORMER_RENDER_H */
