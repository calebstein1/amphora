#ifndef IMG_INTERNAL_H
#define IMG_INTERNAL_H

#include "engine/img.h"
#include "engine/internal/render.h"

struct frameset_t {
	Sint32 sx, sy, w, h;
	Uint16 current_frame;
	Uint16 num_frames;
	Uint16 delay;
	Uint64 last_change;
};

struct sprite_slot_t {
	ImageName image;
	Sint32 dx, dy;
	Sint32 order;
	Uint8 scale;
	Sint32 current_frameset;
	Sint32 num_framesets;
	char **frameset_labels;
	struct frameset_t *framesets;
	bool flip : 1;
	bool display : 1;
	bool stationary : 1;
	bool playing_oneshot : 1;
	bool garbage : 1;
	void (*callback)(void);
	struct sprite_slot_t *next;
};

struct amphora_spritesheet_t {
	SDL_Texture *texture;
};

int init_img(void);
void cleanup_sprites(void);
void draw_all_sprites_and_gc(void);

#endif /* IMG_INTERNAL_H */
