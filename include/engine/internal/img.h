#ifndef IMG_INTERNAL_H
#define IMG_INTERNAL_H

#include "engine/img.h"
#include "engine/internal/render.h"

struct frameset_t {
	Sint32 sx, sy, w, h;
	Sint16 current_frame;
	Uint16 num_frames;
	Uint32 delay;
	Uint64 last_change;
	Vector2f position_offset;
	void (*callback)(void);
	bool playing_oneshot : 1;
};

struct sprite_t {
	ImageName image;
	SDL_FRect rectangle;
	float scale;
	bool flip : 1;
	Sint32 current_frameset;
	Sint32 num_framesets;
	char **frameset_labels;
	struct frameset_t *framesets;
	struct render_list_node_t *render_list_node;
};

int init_img(void);
void cleanup_img(void);
SDL_Texture *get_img_texture_by_name(const char *name);
void update_and_draw_sprite(const AmphoraImage *spr);

#endif /* IMG_INTERNAL_H */
