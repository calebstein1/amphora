#ifndef AMPHORA_TILEMAP_INTERNAL_H
#define AMPHORA_TILEMAP_INTERNAL_H

#include "engine/tilemap.h"

enum map_orientation_e {
	orthogonal,
	isometric
};

struct amphora_tilemap_layer_t {
	SDL_Texture *texture;
	struct render_list_node_t *node;
};

struct amphora_tilemap_t {
	struct amphora_tilemap_layer_t *layers;
	int num_layers;
	int scale;
	enum map_orientation_e orientation;
};

struct amphora_object_groups_t {
	char **labels;
	SDL_Rect **rects;
	int *c_rects;
	int c;
};

int init_maps(void);
SDL_Rect *get_map_rectangle(void);
void destroy_current_map(void);
void free_object_groups(void);
SDL_Rect *get_rects_by_group(const char *name, int *c);

#endif /* AMPHORA_TILEMAP_INTERNAL_H */
