#ifdef WIN32
#include <windows.h>
#endif

#include "engine/internal/img.h"
#include "engine/internal/render.h"

#include "config.h"

/* File-scoped variables */
static SpriteSlot *sprite_slot;
static SpriteSlot *sprite_slots_head;
Uint32 sprite_slots_count = 1;

int
init_sprites(void) {
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
cleanup_sprites(void) {
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
}

Vector2
get_sprite_center(const SpriteSlot *spr) {
	return (Vector2){ spr->x + (spr->x_size * 4), spr->y + (spr->y_size * 4) };
}

void
draw_all_sprites_and_gc(void) {
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
		//if (sprite_slot->display) draw_sprite(sprite_slot);

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
		}
		if (sprite_slot->next->order >= order) {
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
