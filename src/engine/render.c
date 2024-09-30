#include <stdio.h>

#include <SDL2/SDL.h>

#include "engine/render.h"

#include "config.h"

/* Prototypes for private functions */
void draw_sprite(const struct sprite_slot_t *spr, SDL_Renderer *renderer);

/* File-scoped variables */
static struct sprite_slot_t sprite_slots[MAX_SPRITES_ON_SCREEN];
static Uint8 *spritesheet;
static size_t spritesheet_size;
static int free_sprite_slots[MAX_SPRITES_ON_SCREEN];
static int *next_free_sprite_slot = free_sprite_slots;
static struct color_t black = BLACK;
static struct color_t white = WHITE;
static Uint8 zones[] = { 0xff, 0xf0, 0xd9, 0xbd, 0xa1, 0x7f, 0x61, 0x43, 0x29, 0x11, 0x00 };
static unsigned short int pixel_size = 1;
static Vector2 camera = { 0, 0 };
static Vector2 render_dimensions = { 0, 0 };

int
init_render(void) {
	int i;
	for (i = 0; i < MAX_SPRITES_ON_SCREEN; i++) {
		free_sprite_slots[i] = i;
	}
	spritesheet = (Uint8 *)SDL_LoadFile(SPRITESHEET_PATH, &spritesheet_size);

	return spritesheet ? 0 : -1;
}

void
cleanup_render(void) {
	SDL_free(spritesheet);
}

unsigned short int
get_pixel_size(void) {
	return pixel_size;
}

void
set_pixel_size(unsigned short int size) {
	pixel_size = size > 0 ? size : 1;
}

Vector2
get_window_size(void) {
	return render_dimensions;
}

void
set_window_size(Vector2 window_size) {
	render_dimensions = window_size;
}

void
set_camera(Vector2 cam_vec) {
	camera = cam_vec;
}

void
set_black(Uint8 r, Uint8 g, Uint8 b) {
	black = (struct color_t){ r, g, b };
}

void
set_white(Uint8 r, Uint8 g, Uint8 b) {
	white = (struct color_t){ r, g, b };
}

void
clear_bg(SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, 0xff);
	SDL_RenderClear(renderer);
}

Vector2
get_sprite_center(const struct sprite_slot_t *spr) {
	Vector2 center;

	center.x = (spr->x * pixel_size) + (spr->x_subp * pixel_size / SUBPIXEL_STEPS) + (4 * spr->x_size);
	center.y = (spr->y * pixel_size) + (spr->y_subp * pixel_size / SUBPIXEL_STEPS) + (4 * spr->y_size);

	return center;
}

void
draw_all_sprites(SDL_Renderer *renderer) {
	int i;
	struct sprite_slot_t *cur_spr;

	for (i = 0; i < MAX_SPRITES_ON_SCREEN; i++) {
		cur_spr = &sprite_slots[i];
		if (!cur_spr->reserved || !cur_spr->display) continue;

		draw_sprite(cur_spr, renderer);
	}
}

struct sprite_slot_t *
reserve_sprite_slot(struct sprite_slot_t **spr) {
	if (*spr) return *spr;
	if (next_free_sprite_slot == free_sprite_slots + (MAX_SPRITES_ON_SCREEN - 1)) return NULL;

	sprite_slots[*next_free_sprite_slot].reserved = true;
	sprite_slots[*next_free_sprite_slot].display = false;
	*spr = &sprite_slots[*next_free_sprite_slot];
	next_free_sprite_slot++;

	return *spr;
}

struct sprite_slot_t *
init_sprite_slot(struct sprite_slot_t **spr, unsigned int num, short int x_size, short int y_size, int x, int y, bool flip) {
	if (!reserve_sprite_slot(spr)) return NULL;

	(*spr)->num = num;
	(*spr)->x_size = x_size;
	(*spr)->y_size = y_size;
	(*spr)->x = x;
	(*spr)->y = y;
	(*spr)->flip = flip;
	(*spr)->display = true;

	return *spr;
}

void
show_sprite(struct sprite_slot_t *spr) {
	spr->display = true;
}

void
hide_sprite(struct sprite_slot_t *spr) {
	spr->display = false;
}

void *
release_sprite_slot(struct sprite_slot_t **spr) {
	if (next_free_sprite_slot == free_sprite_slots) return *spr;
	if (!(*spr)->reserved) return *spr;

	next_free_sprite_slot--;
	*next_free_sprite_slot = (int)(*spr - sprite_slots);
	(*spr)->display = false;
	(*spr)->reserved = false;
	*spr = NULL;

	return NULL;
}

/*
 * Private functions
 */

void
draw_sprite(const struct sprite_slot_t *spr, SDL_Renderer *renderer) {
	int i;
	Uint8 *s_addr = spritesheet + (spr->num * 0x20);
	Uint8 p_0, p_1, p_2, p_3;
	Uint8 zone, last_zone = 0, cur_pxl;
	int num_pixels = (spr->x_size * spr->y_size) * SPR_NUM_PIXELS;
	int spr_x = spr->x * pixel_size;
	int spr_y = spr->y * pixel_size;
	Uint8 x_off = 0, y_off = 0xff;
	SDL_Rect pxl;

	pxl.h = pixel_size;
	pxl.w = pixel_size;

	for (i = 0; i < num_pixels; i++) {
		if (i > 0 && i % (spr->x_size * SPR_NUM_PIXELS) == 0) {
			s_addr += (0x200 - ((spr->x_size - 1) * SPR_NUM_BYTES));
			spr_x -= ((spr->x_size - 1) * (pixel_size * SPR_SIDE));
		} else if (i > 0 && i % SPR_NUM_PIXELS == 0) {
			s_addr += SPR_NUM_BYTES;
			spr_x += (pixel_size * SPR_SIDE);
			y_off -= SPR_SIDE;
		}

		p_0 = ((*(s_addr + ((i % SPR_NUM_PIXELS) >> 3)) >> (i % SPR_SIDE)) & 1);
		p_1 = ((*(s_addr + 8 + ((i % SPR_NUM_PIXELS) >> 3)) >> (i % SPR_SIDE)) & 1);
		p_2 = ((*(s_addr + 16 + ((i % SPR_NUM_PIXELS) >> 3)) >> (i % SPR_SIDE)) & 1);
		p_3 = ((*(s_addr + 24 + ((i % SPR_NUM_PIXELS) >> 3)) >> (i % SPR_SIDE)) & 1);
		cur_pxl = (p_3 << 3) | (p_2 << 2) | (p_1 << 1) | p_0;

		/*
		 * TODO: Fix flip for sprites bigger than 1x1 tile
		 */
		if (spr->flip) {
			if (i % SPR_SIDE == 0) {
				x_off = 0;
				y_off++;
			} else {
				x_off++;
			}
		} else {
			if (i % SPR_SIDE == 0) {
				x_off = 7;
				y_off++;
			} else {
				x_off--;
			}
		}
		if (!cur_pxl) continue;

		zone = zones[cur_pxl];
		if (zone != last_zone) {
			SDL_SetRenderDrawColor(renderer, black.r, black.g, black.b, zone);
			last_zone = zone;
		}

		pxl.x = spr_x + (x_off * pixel_size) + ((spr->x_subp * pixel_size) / SUBPIXEL_STEPS) - camera.x;
		pxl.y = spr_y + (y_off * pixel_size) + ((spr->y_subp * pixel_size) / SUBPIXEL_STEPS) - camera.y;

		SDL_RenderFillRect(renderer, &pxl);
	}
}
