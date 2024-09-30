#ifndef UNTITLED_PLATFORMER_RENDER_H
#define UNTITLED_PLATFORMER_RENDER_H

#define MAX_SPRITES_ON_SCREEN 256
#define SPR_NUM_PIXELS 64
#define SPR_NUM_BYTES 32
#define SPR_SIDE 8
#define SUBPIXEL_STEPS 16

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "engine/util.h"

struct sprite_slot_t {
	unsigned int num; /* The sprite to draw from the spritesheet */
	short int x_size; /* The number of horizontal tiles in the sprite */
	short int y_size; /* The number of vertical tiles in the sprite */
	int x; /* The sprite's x position */
	int y; /* The sprite's y position */
	Uint8 x_subp : 4; /* The sprite's x subpixel position */
	Uint8 y_subp : 4; /* The sprite's y subpixel position */
	bool flip : 1; /* Whether or not the sprite should be flipped horizontally */
	bool reserved : 1; /* Whether the sprite slot is reserved or free */
	bool display : 1; /* Whether the sprite should be drawn or not */
};

struct color_t {
	Uint8 r;
	Uint8 g;
	Uint8 b;
};

typedef struct vector2_t {
	int x;
	int y;
} Vector2;

int init_render(void); /* Run once to load spritesheet and palette data */
void cleanup_render(void); /* Free allocated memory for spritesheet, palettes, and colors */
unsigned short int get_pixel_size(void); /* Get the current pixel size */
void set_pixel_size(unsigned short int size); /* Set the current pixel size */
Vector2 get_window_size(void); /* Get the current window dimensions */
void set_window_size(Vector2 window_size); /* Set the current window dimensions */
void set_camera(Vector2 cam_vec); /* Set the location of the camera's top left point */
void set_black(Uint8 r, Uint8 g, Uint8 b); /* Sets the black color */
void set_white(Uint8 r, Uint8 g, Uint8 b); /* Sets the white color */
void clear_bg(SDL_Renderer *renderer); /* Clear the screen and fill with the background color */
Vector2 get_sprite_center(const struct sprite_slot_t *spr);
void draw_all_sprites(SDL_Renderer *renderer); /* Draw all active sprite slots */
/* Reserve a sprite slot and get a pointer to it */
struct sprite_slot_t *reserve_sprite_slot(struct sprite_slot_t **spr);
/* Reserve a sprite slot and initialize it with default values */
struct sprite_slot_t *init_sprite_slot(struct sprite_slot_t **spr, unsigned int num, short int x_size, short int y_size, int x, int y, bool flip);
/* Display the supplied sprite_slot */
void show_sprite(struct sprite_slot_t *spr);
/* Hide a sprite slot without releasing it */
void hide_sprite(struct sprite_slot_t *spr);
/* Release a sprite slot for reuse */
void *release_sprite_slot(struct sprite_slot_t **spr);

#endif /* UNTITLED_PLATFORMER_RENDER_H */
