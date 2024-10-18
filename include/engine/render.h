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

typedef struct sprite_slot_t {
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

typedef struct point_t {
	Position32 x;
	Position32 y;
} Point;

typedef Point Camera;

/* One-time tasks to initialize the renderer */
int init_render(void);
/* Frees resources allocated for the renderer in anticipation of exit */
void cleanup_render(void);
/* Get the current pixel size */
Uint16 get_pixel_size(void);
/* Set the current pixel size */
void set_pixel_size(Uint16 size);
/* Get the current window dimensions in scaled pixels */
Point get_game_window_size(void);
/* Get the current window dimensions in subpixel steps */
Point get_game_subpixel_window_size(void);
/* Get the current window dimensions in real pixels */
Vector2 get_real_window_size(void);
/* Set the current window dimensions */
void set_window_size(Vector2 window_size);
/* Get the current upper-left point of the camera */
Point get_camera(void);
/* Set the location of the camera's top left point */
void set_camera(Position32 x, Position32 y);
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
/* Gets the scaled pixel position of the center of a sprite */
Point get_sprite_center(const SpriteSlot *spr);
/* Draw all active sprite slots and clean up freed sprites, called once per frame automatically */
void draw_all_sprites_and_gc(SDL_Renderer *renderer);
/* Allocate a sprite slot and initialize it with the supplied values */
SpriteSlot *init_sprite_slot(SpriteSlot **spr, unsigned int num, short int x_size, short int y_size, int x, int y, bool flip, int order);
/* Show the supplied sprite_slot if hidden */
void show_sprite(SpriteSlot *spr);
/* Hide a sprite without free it */
void hide_sprite(SpriteSlot *spr);
/* Free a sprite slot */
void *release_sprite_slot(SpriteSlot **spr);

#endif /* UNTITLED_PLATFORMER_RENDER_H */
