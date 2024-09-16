#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "engine/render.h"

#include "config.h"

/* Prototypes for private functions */
void draw_sprite(const struct sprite_slot_t *spr, SDL_Renderer *renderer, int p_size);

/* File-scoped variables */
static struct sprite_slot_t sprite_slots[MAX_SPRITES_ON_SCREEN];
static struct color_t *colors;
static Uint8 *spritesheet;
static Uint8 *user_palettes;
static int free_sprite_slots[MAX_SPRITES_ON_SCREEN];
static int *next_free_sprite_slot = free_sprite_slots;
static struct color_t bg_color = { 0, 0, 0 };

int
init_render(void) {
	int spr_fd, pal_fd, p_tbl_fd;
	struct stat spr_stat, pal_stat, p_tbl_stat;
	int i;

	for (i = 0; i < MAX_SPRITES_ON_SCREEN; i++) {
		free_sprite_slots[i] = i;
	}

	if ((spr_fd = open(SPRITESHEET_PATH, O_RDONLY)) == -1) {
		perror("open");
		return -1;
	}
	if ((pal_fd = open(USER_PALETTES_PATH, O_RDONLY)) == -1) {
		perror("open");
		return -1;
	}
	if ((p_tbl_fd = open(COLOR_TABLE_PATH, O_RDONLY)) == -1) {
		perror("open");
		return -1;
	}
	fstat(spr_fd, &spr_stat);
	if (!(spritesheet = malloc(spr_stat.st_size))) {
		perror("malloc");
		goto leave;
	}
	fstat(pal_fd, &pal_stat);
	if (!(user_palettes = malloc(pal_stat.st_size))) {
		perror("malloc");
		goto free_spr;
	}
	fstat(p_tbl_fd, &p_tbl_stat);
	if (!(colors = malloc(p_tbl_stat.st_size))) {
		perror("malloc");
		goto free_pal;
	}

	lseek(spr_fd, 0, SEEK_SET);
	read(spr_fd, spritesheet, spr_stat.st_size);
	lseek(pal_fd, 0, SEEK_SET);
	read(pal_fd, user_palettes, pal_stat.st_size);
	lseek(p_tbl_fd, 0, SEEK_SET);
	read(p_tbl_fd, colors, p_tbl_stat.st_size * sizeof(struct color_t));
	close(spr_fd);
	close(pal_fd);
	close(p_tbl_fd);

	return 0;

	/* Error cleanup */
	free_pal:
	free(user_palettes);
	free_spr:
	free(spritesheet);
	leave:
	return -1;
}

void
cleanup_render(void) {
	free(spritesheet);
	free(user_palettes);
	free(colors);
}

void
set_bg(Uint8 r, Uint8 g, Uint8 b) {
	bg_color.r = r;
	bg_color.g = g;
	bg_color.b = b;
}

struct color_t
get_bg(void) {
	return bg_color;
}

void
clear_bg(SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, 0xff);
	SDL_RenderClear(renderer);
}

void
draw_all_sprites(SDL_Renderer *renderer, int p_size) {
	int i;
	struct sprite_slot_t *cur_spr;

	for (i = 0; i < MAX_SPRITES_ON_SCREEN; i++) {
		cur_spr = &sprite_slots[i];
		if (!cur_spr->reserved || !cur_spr->display) continue;

		draw_sprite(cur_spr, renderer, p_size);
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
init_sprite_slot(struct sprite_slot_t **spr, unsigned int num, short int x_size, short int y_size, int x, int y, Uint8 pal, bool flip) {
	if (!reserve_sprite_slot(spr)) return NULL;

	(*spr)->num = num;
	(*spr)->x_size = x_size;
	(*spr)->y_size = y_size;
	(*spr)->x = x;
	(*spr)->y = y;
	(*spr)->pal = pal;
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
draw_sprite(const struct sprite_slot_t *spr, SDL_Renderer *renderer, int p_size) {
	int i;
	Uint8 *s_addr = spritesheet + (spr->num << 4);
	Uint8 p_low, p_hi;
	Uint8 color, last_color = 0, cur_pxl;
	int num_pixels = (spr->x_size * spr->y_size) * SPR_NUM_PIXELS;
	int spr_x = spr->x * p_size;
	int spr_y = spr->y * p_size;
	Uint8 x_off = 0, y_off = 0xff;
	SDL_Rect pxl;

	pxl.h = p_size;
	pxl.w = p_size;

	for (i = 0; i < num_pixels; i++) {
		if (i > 0 && i % (spr->x_size * SPR_NUM_PIXELS) == 0) {
			s_addr += (0x100 - ((spr->x_size - 1) * SPR_NUM_BYTES));
			spr_x -= ((spr->x_size - 1) * (p_size * SPR_SIDE));
		} else if (i > 0 && i % SPR_NUM_PIXELS == 0) {
			s_addr += SPR_NUM_BYTES;
			spr_x += (p_size * SPR_SIDE);
			y_off -= SPR_SIDE;
		}

		p_low = ((*(s_addr + ((i % SPR_NUM_PIXELS) >> 3)) >> (i % SPR_SIDE)) & 1);
		p_hi = ((*(s_addr + (SPR_NUM_BYTES / 2) + ((i % SPR_NUM_PIXELS) >> 3)) >> (i % SPR_SIDE)) & 1);
		cur_pxl = (p_hi << 1) | p_low;

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

		color = *(user_palettes + (spr->pal * PALETTE_SIZE) + cur_pxl);
		if (color != last_color) {
			SDL_SetRenderDrawColor(renderer, colors[color].r, colors[color].g, colors[color].b, 0xff);
			last_color = color;
		}

		pxl.x = spr_x + (x_off * p_size) + ((spr->x_subp * p_size) / SUBPIXEL_STEPS);
		pxl.y = spr_y + (y_off * p_size) + ((spr->y_subp * p_size) / SUBPIXEL_STEPS);

		SDL_RenderFillRect(renderer, &pxl);
	}
}
