#ifndef UNTITLED_PLATFORMER_RENDER_H
#define UNTITLED_PLATFORMER_RENDER_H

#define SPR_NUM_PIXELS 64
#define SPR_NUM_BYTES 32
#define SPR_SIDE 8
#define SUBPIXEL_STEPS 16
#define SUBPIXEL_SHIFT 4

#define DECODE_POSITION32(p) (((p >> SUBPIXEL_SHIFT) * pixel_size) + (((p & (SUBPIXEL_STEPS - 1)) * pixel_size) / SUBPIXEL_STEPS))

#include "SDL.h"

#include "engine/util.h"

typedef Sint32 Position32; /* A position in the game, each unit is equal to 1/16 of a pixel */

struct sprite_slot_t {
	Uint32 num; /* The sprite to draw from the spritesheet */
	Sint16 x_size; /* The number of horizontal tiles in the sprite */
	Sint16 y_size; /* The number of vertical tiles in the sprite */
	Position32 x; /* The sprite's x position in 1/16 of a pixel */
	Position32 y; /* The sprite's y position in 1/16 of a pixel */
	Sint32 order; /* The draw order of sprites, higher numbers on top */
	bool flip : 1; /* Whether or not the sprite should be flipped horizontally */
	bool display : 1; /* Whether the sprite should be drawn or not */
	bool garbage : 1; /* Whether the garbage collector should free the allocated memory */
	struct sprite_slot_t *next;
};

struct color_t {
	Uint8 r;
	Uint8 g;
	Uint8 b;
};

typedef struct vector2_t {
	Sint32 x;
	Sint32 y;
} Vector2;

typedef struct point_t {
	Position32 x;
	Position32 y;
} Point;

typedef Point Camera;

int init_render(void); /* Run once to load spritesheet and palette data */
void cleanup_render(void); /* Free allocated memory for spritesheet, palettes, and colors */
unsigned short int get_pixel_size(void); /* Get the current pixel size */
void set_pixel_size(unsigned short int size); /* Set the current pixel size */
Point get_game_window_size(void); /* Get the current window dimensions in subpixel steps */
Vector2 get_real_window_size(void); /* Get the current window dimensions in real pixels */
void set_window_size(Vector2 window_size); /* Set the current window dimensions */
void set_camera(Position32 x, Position32 y); /* Set the location of the camera's top left point */
void set_black(Uint8 r, Uint8 g, Uint8 b); /* Sets the black color */
void set_white(Uint8 r, Uint8 g, Uint8 b); /* Sets the white color */
void clear_bg(SDL_Renderer *renderer); /* Clear the screen and fill with the background color */
Point get_sprite_center(const struct sprite_slot_t *spr);
void draw_all_sprites_and_gc(SDL_Renderer *renderer); /* Draw all active sprite slots */
/* Reserve a sprite slot and initialize it with default values */
struct sprite_slot_t *init_sprite_slot(struct sprite_slot_t **spr, unsigned int num, short int x_size, short int y_size, int x, int y, bool flip, int order);
/* Display the supplied sprite_slot */
void show_sprite(struct sprite_slot_t *spr);
/* Hide a sprite slot without releasing it */
void hide_sprite(struct sprite_slot_t *spr);
/* Release a sprite slot for reuse */
void *release_sprite_slot(struct sprite_slot_t **spr);

#endif /* UNTITLED_PLATFORMER_RENDER_H */
