#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "engine/palette.h"
#include "engine/sprites.h"
#include "engine/util.h"

/* Prototypes for private functions */
void draw_sprite(const struct sprite_slot_t *spr, SDL_Renderer *renderer, int p_size);
u8 get_cur_pixel(const struct sprite_slot_t *spr, u8 s_pxl_low, u8 s_pxl_hi, u8 i);

/* File-scoped variables */
static struct sprite_slot_t sprite_slots[MAX_SPRITES_ON_SCREEN];
static struct color_t pal[MAX_COLORS];
static u8 spritesheet[SPRITESHEET_SIZE];
static u8 user_palettes[PALETTE_SIZE * MAX_USER_PALETTES];
static int free_sprite_slots[MAX_SPRITES_ON_SCREEN];
static int *next_free_sprite_slot = free_sprite_slots;

int
init_spritesheet(void) {
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
	if ((p_tbl_fd = open(PALETTE_TABLE_PATH, O_RDONLY)) == -1) {
		perror("open");
		return -1;
	}
	fstat(spr_fd, &spr_stat);
	if (spr_stat.st_size > SPRITESHEET_SIZE) {
		fputs("Spritesheet too large\n", stderr);
		return -1;
	}
	fstat(pal_fd, &pal_stat);
	if (pal_stat.st_size > PALETTE_SIZE * MAX_USER_PALETTES) {
		fputs("User palettes too large\n", stderr);
		return -1;
	}
	fstat(p_tbl_fd, &p_tbl_stat);
	if (p_tbl_stat.st_size > MAX_COLORS * (long)sizeof(struct color_t)) {
		fputs("Palette table too large\n", stderr);
		return -1;
	}

	lseek(spr_fd, 0, SEEK_SET);
	read(spr_fd, spritesheet, SPRITESHEET_SIZE);
	lseek(pal_fd, 0, SEEK_SET);
	read(pal_fd, user_palettes, PALETTE_SIZE * MAX_USER_PALETTES);
	lseek(p_tbl_fd, 0, SEEK_SET);
	read(p_tbl_fd, pal, PALETTE_SIZE * MAX_USER_PALETTES);
	close(spr_fd);
	close(pal_fd);
	close(p_tbl_fd);

	return 0;
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
	printf("Allocating slot %d\n", *next_free_sprite_slot);
	if (*spr) return *spr;
	if (next_free_sprite_slot == free_sprite_slots + (MAX_SPRITES_ON_SCREEN - 1)) return NULL;

	sprite_slots[*next_free_sprite_slot].reserved = true;
	*spr = &sprite_slots[*next_free_sprite_slot];
	next_free_sprite_slot++;

	return *spr;
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
	u8 i;
	u8 *s_addr = spritesheet + (spr->num << 4);
	u8 c, cur_pxl;
	int spr_x = spr->x * p_size;
	int spr_y = spr->y * p_size;
	/* initialize y to 255 since we increment y each 8 pixels, and 0 is a multiple of 8 */
	u8 x_off = 0, y_off = 255;
	SDL_Rect pxl;

	pxl.h = p_size;
	pxl.w = p_size;

	for (i = 0; i < SPR_NUM_PIXELS; i++) {
		/* i & 7 == i % 8 */
		if (spr->flip) {
			if ((i & 7) == 0) {
				x_off = 0;
				y_off++;
			} else {
				x_off++;
			}
		} else {
			if ((i & 7) == 0) {
				x_off = 7;
				y_off++;
			} else {
				x_off--;
			}
		}

		cur_pxl = get_cur_pixel(spr, *(s_addr + (i >> 3)), *(s_addr + 8 + (i >> 3)), i & 7);
		if (!cur_pxl) continue;

		c = *(user_palettes + spr->pal + cur_pxl);
		SDL_SetRenderDrawColor(renderer, pal[c].r, pal[c].g, pal[c].b, 0xff);

		pxl.x = spr_x + (x_off * p_size) + ((spr->x_subp * p_size) >> SUBPIXEL_STEPS);
		pxl.y = spr_y + (y_off * p_size) + ((spr->y_subp * p_size) >> SUBPIXEL_STEPS);

		SDL_RenderFillRect(renderer, &pxl);
	}
}

u8
get_cur_pixel(const struct sprite_slot_t *spr, u8 s_pxl_low, u8 s_pxl_hi, u8 i) {
	u8 low = (s_pxl_low >> i) & 1;
	u8 hi = (s_pxl_hi >> i) & 1;
	switch ((hi << 8) + low) {
		case 0x0:
			return 0;
		case 0x01:
			return 1;
		case 0x100:
			return 2;
		case 0x101:
			return 3;
		default:
			fprintf(stderr, "Processing sprite %u resulted in an impossible pixel color on pixel %u, got %x\n\n",
				spr->num, i, ((hi << 8) + low));
	}

	return 0;
}
