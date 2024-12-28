#ifndef DISABLE_TILEMAP
#ifdef WIN32
#include <windows.h>
#endif

#include "engine/internal/img.h"
#include "engine/internal/render.h"
#include "engine/internal/tilemap.h"

#define CUTE_TILED_IMPLEMENTATION
#include "vendor/cute_tiled.h"

/* Prototypes for private functions */
int get_map_by_name(const char *name);
int parse_map_to_texture(enum tilemaps_e map_idx);

/* File-scoped variables */
static char *map_names[] = {
#define LOADMAP(name, path) #name,
	MAPS
#undef LOADMAP
};
static Sint32 map_sizes[MAPS_COUNT];
static char *map_data[MAPS_COUNT];
static struct amphora_tilemap_t current_map;
static SDL_Rect map_rect;
static struct amphora_object_groups_t obj_groups;

void
set_map(const char *name, const Uint16 scale) {
	int idx, i;
	struct render_list_node_t *render_list_node;

	destroy_current_map();
	if (!name) return;

	if ((idx = get_map_by_name(name)) == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to locate map %s\n", name);
		return;
	}
	current_map.scale = scale ? scale : 1;
	if (parse_map_to_texture(idx) == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from map: %s\n", name);
		return;
	}
	for (i = 0; i < current_map.num_layers; i++) {
		render_list_node = add_render_list_node(100 * i);
		render_list_node->type = MAP;
		render_list_node->data = current_map.layers[i].texture;
		current_map.layers[i].node = render_list_node;
	}
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

SDL_Rect *
get_map_rectangle(void) {
	return &map_rect;
}

void
destroy_current_map(void) {
	int i;

	for (i = 0; i < current_map.num_layers; i++) {
		SDL_DestroyTexture(current_map.layers[i].texture);
		current_map.layers[i].node->garbage = true;
	}
	if (current_map.num_layers > 0) SDL_free(current_map.layers);
	current_map.num_layers = 0;
}

void
free_object_groups(void) {
	int i;

	for (i = 0; i < obj_groups.c; i++) {
		SDL_free(obj_groups.labels[i]);
		SDL_free(obj_groups.rects[i]);
	}
	SDL_free(obj_groups.labels);
	SDL_free(obj_groups.c_rects);
}

SDL_Rect *
get_rects_by_group(const char *name, int *c) {
	int i;

	for (i = 0; i < obj_groups.c; i++) {
		if (SDL_strcmp(name, obj_groups.labels[i]) == 0) break;
	}
	if (i == obj_groups.c) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not find object group: %s\n", name);
		return NULL;
	}
	*c = obj_groups.c_rects[i];

	return obj_groups.rects[i];
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

int
parse_map_to_texture(const enum tilemaps_e map_idx) {
	SDL_Renderer *renderer = get_renderer();
	cute_tiled_map_t *map = cute_tiled_load_map_from_memory(map_data[map_idx], map_sizes[map_idx], 0);
	cute_tiled_layer_t *layer = map->layers;
	cute_tiled_object_t *object;
	cute_tiled_tileset_t *tileset = map->tilesets;
	int tileset_img_w, tileset_img_h;
	SDL_Texture *tileset_img = get_img_texture_by_name(tileset->name.ptr);
	SDL_Rect tile_s = { .w = map->tilewidth, .h = map->tileheight };
	SDL_Rect tile_d = { .w = map->tilewidth, .h = map->tileheight };
	int pixel_width = map->width * map->tilewidth;
	int pixel_height = map->height * map->tileheight;
	int tile_idx, i, j, row;

	while (layer) {
		if (SDL_strcmp(layer->type.ptr, "tilelayer") == 0) current_map.num_layers++;
		layer = layer->next;
	}
	if (!((current_map.layers = SDL_malloc(current_map.num_layers * sizeof(struct amphora_tilemap_layer_t))))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not allocate map layers\n");
		return -1;
	}
	layer = map->layers;
	if (SDL_strcmp(map->orientation.ptr, "orthogonal") == 0) {
		current_map.orientation = orthogonal;
		for (i = 0; i < current_map.num_layers; i++) {
			current_map.layers[i].texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
								  SDL_TEXTUREACCESS_TARGET, pixel_width,
								  pixel_height);
		}
	} else if (SDL_strcmp(map->orientation.ptr, "isometric") == 0) {
		current_map.orientation = isometric;
		for (i = 0; i < current_map.num_layers; i++) {
			current_map.layers[i].texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
								SDL_TEXTUREACCESS_TARGET,
								pixel_width + (map->tilewidth / 2), pixel_height);
		}
	} else {
		return -1;
	}
	SDL_QueryTexture(tileset_img, NULL, NULL, &tileset_img_w, &tileset_img_h);
	i = 0;
	while (layer) {
		if (SDL_strcmp(layer->type.ptr, "tilelayer") == 0) {
			SDL_SetTextureBlendMode(current_map.layers[i].texture, SDL_BLENDMODE_BLEND);
			SDL_SetRenderTarget(renderer, current_map.layers[i].texture);
			for (j = 0; j < layer->data_count; j++) {
				tile_idx = layer->data[j] - 1;
				row = ((j * map->tilewidth) / (map->width * map->tilewidth));
				tile_s.x = (tile_idx * map->tilesets->tilewidth) % tileset_img_w;
				tile_s.y = ((tile_idx * map->tilesets->tilewidth) / tileset_img_w) * map->tileheight;
				switch (current_map.orientation) {
					case orthogonal:
						tile_d.x = (j * map->tilewidth) % (map->width * map->tilewidth);
						tile_d.y = row * map->tileheight;
						break;
					case isometric:
						tile_d.x = (j * map->tilewidth) % (map->width * map->tilewidth) +
							   (((map->width * map->tilewidth) / 2) - ((j % map->width) *
												   (map->tilewidth /
												    2)) -
							    (row * (map->tilewidth / 2)));
						tile_d.y = row * map->tileheight +
							   ((j % map->width) * (map->tileheight / 2)) -
							   (row * (map->tileheight / 2));
						break;
				}
				SDL_RenderCopy(renderer, tileset_img, &tile_s, &tile_d);
			}
			i++;
		} else if (SDL_strcmp(layer->type.ptr, "objectgroup") == 0) {
			if (!((obj_groups.labels = SDL_realloc(obj_groups.labels, ++obj_groups.c * sizeof(char *))))) {
				SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate object group labels\n");
				return -1;
			}
			if (!((obj_groups.c_rects = SDL_realloc(obj_groups.c_rects, obj_groups.c * sizeof(int))))) {
				SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate object group rectangle count\n");
				return -1;
			}
			if (!((obj_groups.rects = SDL_realloc(obj_groups.rects, obj_groups.c * sizeof(SDL_Rect *))))) {
				SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate object group rectangle list\n");
				return -1;
			}
			object = layer->objects;
			j = 0;
			while (object) {
				j++;
				object = object->next;
			}
			object = layer->objects;
			if (!((obj_groups.labels[obj_groups.c - 1] = SDL_malloc(SDL_strlen(layer->name.ptr) + 1)))) {
				SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate space for label\n");
				return -1;
			}
			if (!((obj_groups.rects[obj_groups.c - 1] = SDL_malloc(j * sizeof(SDL_Rect))))) {
				SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate object group rectangles\n");
				return -1;
			}
			SDL_strlcpy(obj_groups.labels[obj_groups.c - 1], layer->name.ptr, SDL_strlen(layer->name.ptr) + 1);
			obj_groups.c_rects[obj_groups.c - 1] = j;
			j = 0;
			while (object) {
				obj_groups.rects[obj_groups.c - 1][j].x = (int)object->x * current_map.scale;
				obj_groups.rects[obj_groups.c - 1][j].y = (int)object->y * current_map.scale;
				obj_groups.rects[obj_groups.c - 1][j].w = (int)object->width * current_map.scale;
				obj_groups.rects[obj_groups.c - 1][j].h = (int)object->height * current_map.scale;
				j++;
				object = object->next;
			}
		}
		layer = layer->next;
	}
	SDL_SetRenderTarget(renderer, NULL);
	cute_tiled_free_map(map);
	SDL_QueryTexture(current_map.layers[0].texture, NULL, NULL, &map_rect.w, &map_rect.h);
	map_rect.w *= current_map.scale;
	map_rect.h *= current_map.scale;

	return 0;
}

#endif
