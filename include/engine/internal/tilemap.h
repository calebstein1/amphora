#ifndef AMPHORA_TILEMAP_INTERNAL_H
#define AMPHORA_TILEMAP_INTERNAL_H

#include "engine/tilemap.h"

enum map_orientation_e {
	orthogonal,
	isometric
};

struct amphora_tilemap_t {
    SDL_Texture *texture;
    int scale;
    enum map_orientation_e orientation;
};

int init_maps(void);
void render_current_map(void);
void destroy_current_map(void);

#endif /* AMPHORA_TILEMAP_INTERNAL_H */
