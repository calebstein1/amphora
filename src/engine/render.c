#ifdef WIN32
#include <windows.h>
#endif

#include "engine/render.h"

#include "config.h"

/* Prototypes for private functions */
void draw_sprite(const SpriteSlot *spr, SDL_Renderer *renderer);

/* File-scoped variables */
static SpriteSlot *sprite_slot;
static SpriteSlot *sprite_slots_head;
Uint32 sprite_slots_count = 1;
static SDL_Color black = { 0, 0, 0, 255 };
static SDL_Color white = { 255, 255, 255, 255 };
static SDL_Color bg = { 255, 255, 255, 255 };
static Uint8 zones[] = { 0xff, 0xf0, 0xd9, 0xbd, 0xa1, 0x7f, 0x61, 0x43, 0x29, 0x11, 0x00 };
static Uint16 pixel_size = 1;
static Camera camera = { 0, 0 };
static SDL_DisplayMode *display_modes = NULL;
static Sint32 display_index, num_display_modes;
#ifdef WIN32
static Uint8 *spritesheet;
static DWORD spritesheet_size;
#else
extern Uint8 spritesheet[];
extern int spritesheet_size;
#endif

int
init_render(void) {
	Sint32 i;
#ifdef WIN32
	HRSRC spritesheet_info = FindResourceA(NULL, "SpriteSheet", "AMPHORA_SPRITESHEET");
	if (!spritesheet_info) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to locate spritesheet resource... Amphora will crash now\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to locate spritesheet resource... Amphora will crash now", 0);
		return -1;
	}
	HGLOBAL spritesheet_resource = LoadResource(NULL, spritesheet_info);
	if (!spritesheet_resource) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to load spritesheet resource... Amphora will crash now\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to load spritesheet resource... Amphora will crash now", 0);
		return -1;
	}
	spritesheet_size = SizeofResource(NULL, spritesheet_info);
	spritesheet = (Uint8 *)spritesheet_resource;
#endif
	display_index = SDL_GetWindowDisplayIndex(get_window());
	num_display_modes = SDL_GetNumDisplayModes(0);
	if (((display_modes = SDL_malloc(num_display_modes * sizeof(SDL_DisplayMode))))) {
		for (i = 0; i < num_display_modes; i++) {
			SDL_GetDisplayMode(display_index, i, &display_modes[i]);
		}
	} else {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to allocate space for display modes\n");
	}

	if ((sprite_slot = SDL_malloc(sizeof(SpriteSlot))) == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to initialize sprite slots\n");

		return -1;
	}
	sprite_slot->order = SDL_MIN_SINT32;
	sprite_slot->display = false;
	sprite_slot->garbage = false;
	sprite_slot->next = NULL;
	sprite_slots_head = sprite_slot;

	return 0;
}

void
cleanup_render(void) {
	SpriteSlot **allocated_addrs = SDL_malloc(sprite_slots_count * sizeof(SpriteSlot *));
	Uint32 i = 0;

	while (sprite_slot) {
		allocated_addrs[i++] = sprite_slot;
		sprite_slot = sprite_slot->next;
	}
	for (i = 0; i < sprite_slots_count; i++) {
		SDL_free(allocated_addrs[i]);
	}
	SDL_free(allocated_addrs);
	if (display_modes) SDL_free(display_modes);
}

Vector2
get_resolution(void) {
	Sint32 rx, ry;
	SDL_GL_GetDrawableSize(get_window(), &rx, &ry);
	return (Vector2){ rx, ry };
}

void
set_resolution(Sint32 mode) {
#if WINDOW_MODE != fullscreen
	return;
#endif
	int wx, wy;

#ifndef DISABLE_UNDOCUMENTED_WARNING
	SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "set_resolution is an experimental, undocumented, and unsuported function");
#endif

	SDL_GL_GetDrawableSize(get_window(), &wx, &wy);
	SDL_SetWindowDisplayMode(get_window(), &display_modes[mode]);
	SDL_RenderSetLogicalSize(get_renderer(), wx, wy);
}

Vector2
get_camera(void) {
	return camera;
}

void
set_camera(Sint32 x, Sint32 y) {
	camera.x = x;
	camera.y = y;
}

SDL_Color
get_bg (void) {
	return bg;
}

void
set_bg(SDL_Color color) {
	bg = color;
}

void
clear_bg(SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(renderer);
}

Vector2
get_sprite_center(const SpriteSlot *spr) {
	Vector2 center;

	center.x = spr->x + (spr->x_size * 4);
	center.y = spr->y + (spr->y_size * 4);

	return center;
}

void
draw_all_sprites_and_gc(SDL_Renderer *renderer) {
	SpriteSlot *garbage;

	if (!sprite_slot->next) return;

	while(1) {
		if (!sprite_slot) break;

		if (sprite_slot->next && sprite_slot->next->garbage) {
			garbage = sprite_slot->next;
			sprite_slot->next = sprite_slot->next->next;
			SDL_free(garbage);
			garbage = NULL;
			sprite_slots_count--;
		}
		if (sprite_slot->display) draw_sprite(sprite_slot, renderer);

		sprite_slot = sprite_slot->next;
	}

	sprite_slot = sprite_slots_head;
}

SpriteSlot *
init_sprite_slot(SpriteSlot **spr, Uint32 num, Uint16 x_size, Uint16 y_size, Sint32 x, Sint32 y, bool flip, Sint32 order) {
	SpriteSlot *sprite_slot_temp = NULL;

	if (*spr) return *spr;

	if ((sprite_slot_temp = SDL_malloc(sizeof(SpriteSlot))) == NULL) {
		SDL_LogError(SDL_LOG_PRIORITY_WARN, "Failed to initialize sprite\n");
		*spr = NULL;

		return NULL;
	}
	while (1) {
		if (sprite_slot->next == NULL) {
			sprite_slot_temp->next = NULL;
			sprite_slot->next = sprite_slot_temp;
			break;
		} else if (sprite_slot->next->order >= order) {
			sprite_slot_temp->next = sprite_slot->next;
			sprite_slot->next = sprite_slot_temp;
			break;
		}
		sprite_slot = sprite_slot->next;
	}
	*spr = sprite_slot_temp;
	sprite_slots_count++;

	(*spr)->num = num;
	(*spr)->x_size = x_size;
	(*spr)->y_size = y_size;
	(*spr)->x = x;
	(*spr)->y = y;
	(*spr)->flip = flip;
	(*spr)->display = true;
	(*spr)->garbage = false;
	(*spr)->order = order;

	return *spr;
}

void
show_sprite(SpriteSlot *spr) {
	spr->display = true;
}

void
hide_sprite(SpriteSlot *spr) {
	spr->display = false;
}

void *
release_sprite_slot(SpriteSlot **spr) {
	if (spr) {
		(*spr)->garbage = true;
		*spr = NULL;
	}

	return NULL;
}

/*
 * Private functions
 */

void
draw_sprite(const SpriteSlot *spr, SDL_Renderer *renderer) {
	Uint8 *spr_addr = spritesheet + (spr->num * 0x20);
	Uint8 p_0, p_1, p_2, p_3;
	Uint8 zone, cur_pxl;
	Uint32 num_pixels = (spr->x_size * spr->y_size) * SPR_NUM_PIXELS;
	Uint32 i;
	Sint32 spr_x = spr->x;
	Sint32 spr_y = spr->y;
	Uint8 x_off = 0, y_off = 0xff;
	SDL_Rect pxl;

	(void)spritesheet_size;

	pxl.h = pixel_size;
	pxl.w = pixel_size;

	for (i = 0; i < num_pixels; i++) {
		if (spr->flip) {
			if (i == 0) spr_x += ((spr->x_size - 1) * SPR_SIDE);
			if (i > 0 && i % (spr->x_size * SPR_NUM_PIXELS) == 0) {
				spr_addr += (0x200 - ((spr->x_size - 1) * SPR_NUM_BYTES));
				spr_x += ((spr->x_size - 1) * SPR_SIDE);
			} else if (i > 0 && i % SPR_NUM_PIXELS == 0) {
				spr_addr += SPR_NUM_BYTES;
				spr_x -= SPR_SIDE;
				y_off -= SPR_SIDE;
			}
		} else {
			if (i > 0 && i % (spr->x_size * SPR_NUM_PIXELS) == 0) {
				spr_addr += (0x200 - ((spr->x_size - 1) * SPR_NUM_BYTES));
				spr_x -= ((spr->x_size - 1) * SPR_SIDE);
			} else if (i > 0 && i % SPR_NUM_PIXELS == 0) {
				spr_addr += SPR_NUM_BYTES;
				spr_x += SPR_SIDE;
				y_off -= SPR_SIDE;
			}
		}

		p_0 = ((*(spr_addr + ((i % SPR_NUM_PIXELS) >> 3)) >> (i % SPR_SIDE)) & 1);
		p_1 = ((*(spr_addr + 8 + ((i % SPR_NUM_PIXELS) >> 3)) >> (i % SPR_SIDE)) & 1);
		p_2 = ((*(spr_addr + 16 + ((i % SPR_NUM_PIXELS) >> 3)) >> (i % SPR_SIDE)) & 1);
		p_3 = ((*(spr_addr + 24 + ((i % SPR_NUM_PIXELS) >> 3)) >> (i % SPR_SIDE)) & 1);
		cur_pxl = (p_3 << 3) | (p_2 << 2) | (p_1 << 1) | p_0;

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

		pxl.x = (x_off * pixel_size) + spr_x - camera.x;
		pxl.y = (y_off * pixel_size) + spr_y - camera.y;

		SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, 0xff);
		SDL_RenderFillRect(renderer, &pxl);

		zone = zones[cur_pxl];
		SDL_SetRenderDrawColor(renderer, black.r, black.g, black.b, zone);
		SDL_RenderFillRect(renderer, &pxl);
	}
}
