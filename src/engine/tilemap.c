#ifndef DISABLE_TILEMAP
#ifdef WIN32
#include <windows.h>
#endif

#include <engine/internal/img.h>
#include "engine/internal/render.h"
#include "engine/internal/tilemap.h"

#define CUTE_TILED_IMPLEMENTATION
#include "vendor/cute_tiled.h"

/* Prototypes for private functions */
MapTexture *parse_map_to_texture(enum tilemaps_e map_idx);

/* File-scoped variables */
static char *map_names[] = {
#define LOADMAP(name, path) #name,
	MAPS
#undef LOADMAP
};
static Sint32 map_sizes[MAPS_COUNT];
static char *map_data[MAPS_COUNT];

/*
 * Internal functions
 */

int
init_maps(void) {
	int i;
#ifdef WIN32
	HRSRC map_info;
	HGLOBAL map_resource;

	for (i = 0; i < MAPS_COUNT; i++) {
		if (!((map_info = FindResourceA(NULL, map_names[i], "TILEMAP")))) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to locate map resource... Amphora will crash now\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to locate map resource... Amphora will crash now", 0);
			return -1;
		}
		if (!((map_resource = LoadResource(NULL, map_info)))) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to load map resource... Amphora will crash now\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to load map resource... Amphora will crash now", 0);
			return -1;
		}
		map_data[i] = (char *)map_resource;
		map_sizes[i] = SizeofResource(NULL, map_info);
	}
#else
#define LOADMAP(name, path) extern char name##_tm[]; extern int name##_tm_size;
	MAPS
#undef LOADMAP
	Sint32 *map_sizes_ptr = map_sizes;
	char **map_data_ptr = map_data;
#define LOADMAP(name, path) *map_sizes_ptr = name##_tm_size; *map_data_ptr = name##_tm; map_sizes_ptr++; map_data_ptr++;
	MAPS
#undef LOADMAP
#endif
#ifdef DEBUG
	for (i = 0; i < MAPS_COUNT; i++) {
		SDL_Log("Found map %s\n", map_names[i]);
	}
#endif

	parse_map_to_texture(0);
	return 0;
}

/*
 * Private functions
 */

MapTexture *
parse_map_to_texture(const enum tilemaps_e map_idx) {
	cute_tiled_map_t *map = cute_tiled_load_map_from_memory(map_data[map_idx], map_sizes[map_idx], 0);
	cute_tiled_layer_t *layer = map->layers;
	cute_tiled_tileset_t *tileset = map->tilesets;
	SDL_Texture *tileset_img = get_img_texture_by_name(tileset->name.ptr);
	SDL_Rect tile_s = { .w = map->tilewidth, .h = map->tileheight };
	SDL_Rect tile_d = { .w = map->tilewidth, .h = map->tileheight };
	int tileset_img_w, tileset_img_h;
	int draw_col, draw_row, tile_idx, tile_x, tile_y, i;
	MapTexture *texture = SDL_CreateTexture(get_renderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, map->width * map->tilewidth, map->height * map->tileheight);

	SDL_QueryTexture(tileset_img, NULL, NULL, &tileset_img_w, &tileset_img_h);

	while (layer) {
		draw_col = 0;
		draw_row = 0;

		for (i = 0; i < layer->data_count; i++) {
			tile_idx = layer->data[i];
			tile_x = (tile_idx * map->tilewidth) % tileset_img_w;
			tile_y = (tile_idx * map->tileheight) / tileset_img_w;
		}
		layer = layer->next;
	}

	cute_tiled_free_map(map);

	return texture;
}

#endif
