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
int get_map_by_name(const char *name);
MapTexture *parse_map_to_texture(enum tilemaps_e map_idx);

/* File-scoped variables */
static char *map_names[] = {
#define LOADMAP(name, path) #name,
	MAPS
#undef LOADMAP
};
static Sint32 map_sizes[MAPS_COUNT];
static char *map_data[MAPS_COUNT];
static MapTexture *current_map;
static int map_scale = 1;

void
set_map(const char *name, const Uint16 scale) {
	int idx;

	if (!name) {
		if (current_map) SDL_DestroyTexture(current_map);
		current_map = NULL;
		return;
	}
	if ((idx = get_map_by_name(name)) == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to locate map %s\n", name);
		return;
	}
	if (current_map) SDL_DestroyTexture(current_map);
	current_map = parse_map_to_texture(idx);
	if (scale) map_scale = scale;
}

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

	return 0;
}

void
render_current_map(void) {
	const Camera camera = get_camera();
	SDL_Rect map_rect = {
		.x = -camera.x,
		.y = -camera.y
	};

	SDL_QueryTexture(current_map, NULL, NULL, &map_rect.w, &map_rect.h);
	map_rect.w *= map_scale;
	map_rect.h *= map_scale;
	render_texture(current_map, NULL, &map_rect, 0, SDL_FLIP_NONE);
}

void
destroy_current_map(void) {
	if (current_map) SDL_DestroyTexture(current_map);
}

/*
 * Private functions
 */

int
get_map_by_name(const char *name) {
	int i;

	for (i = 0; i < MAPS_COUNT; i++) {
		if (SDL_strcmp(name, map_names[i]) == 0) return i;
	}
	return -1;
}

MapTexture *
parse_map_to_texture(const enum tilemaps_e map_idx) {
	SDL_Renderer *renderer = get_renderer();
	cute_tiled_map_t *map = cute_tiled_load_map_from_memory(map_data[map_idx], map_sizes[map_idx], 0);
	cute_tiled_layer_t *layer = map->layers;
	cute_tiled_tileset_t *tileset = map->tilesets;
	int tileset_img_w, tileset_img_h;
	SDL_Texture *tileset_img = get_img_texture_by_name(tileset->name.ptr);
	SDL_Rect tile_s = { .w = map->tilewidth, .h = map->tileheight };
	SDL_Rect tile_d = { .w = map->tilewidth, .h = map->tileheight };
	int tile_idx, i;
	MapTexture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, map->width * map->tilewidth, map->height * map->tileheight);

	SDL_QueryTexture(tileset_img, NULL, NULL, &tileset_img_w, &tileset_img_h);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, texture);
	while (layer) {
		for (i = 0; i < layer->data_count; i++) {
			tile_idx = layer->data[i] - 1;
			tile_s.x = (tile_idx * map->tilewidth) % tileset_img_w;
			tile_s.y = ((tile_idx * map->tileheight) / tileset_img_w) * map->tileheight;
			tile_d.x = (i * map->tilewidth) % (map->width * map->tilewidth);
			tile_d.y = ((i * map->tileheight) / (map->width * map->tilewidth)) * map->tileheight;
			SDL_RenderCopy(renderer, tileset_img, &tile_s, &tile_d);
		}
		layer = layer->next;
	}
	SDL_SetRenderTarget(renderer, NULL);
	cute_tiled_free_map(map);

	return texture;
}

#endif
