#ifndef AMPHORA_TILEMAP_INTERNAL_H
#define AMPHORA_TILEMAP_INTERNAL_H

#include "engine/tilemap.h"

#define OBJ_GRP_BATCH_SIZE 4

enum map_orientation_e {
	MAP_ORTHOGONAL,
	MAP_ISOMETRIC
};

struct amphora_tilemap_layer_t {
	SDL_Texture *texture;
	struct render_list_node_t *node;
	float a, a_stp;
	bool hiding;
};

struct amphora_tilemap_t {
	struct amphora_tilemap_layer_t *layers;
	char **layer_names;
	int num_layers;
	float scale;
	enum map_orientation_e orientation;
};

struct amphora_object_groups_t {
	char **labels;
	SDL_FRect **rects;
	int *c_rects;
	int c;
	int max_c;
};

int Amphora_InitMaps(void);
SDL_FRect *Amphora_GetMapRectangle(void);
void Amphora_DestroyCurrentMap(void);
void Amphora_FreeObjectGroup(void);
void Amphora_FreeAllObjectGroups(void);
SDL_FRect *Amphora_GetRectsByGroup(const char *name, int *c);
void Amphora_ProcessDeferredTransition(void);

#endif /* AMPHORA_TILEMAP_INTERNAL_H */
