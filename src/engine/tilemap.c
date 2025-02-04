#ifndef DISABLE_TILEMAP
#ifdef WIN32
#include <windows.h>
#endif

#include "engine/internal/hash_table.h"
#include "engine/internal/img.h"
#include "engine/internal/render.h"
#include "engine/internal/tilemap.h"

#define CUTE_TILED_IMPLEMENTATION
#include "vendor/cute_tiled.h"

/* Prototypes for private functions */
static int Amphora_ParseMapToTexture(const char *name);
static int Amphora_ParseTileLayer(const cute_tiled_map_t *map, const cute_tiled_layer_t *layer, int tileset_img_w, SDL_Texture *tileset_img, int n);
static int Amphora_ParseObjectGroup(const cute_tiled_layer_t *layer);
static int Amphora_GetMapLayerByName(const char *name);

/* File-scoped variables */
static char *map_names[] = {
#define LOADMAP(name, path) #name,
	MAPS
#undef LOADMAP
};
static HT_HashTable map_sizes[MAPS_COUNT * 4 / 2];
static HT_HashTable map_data[MAPS_COUNT * 4 / 2];
static struct amphora_tilemap_t current_map;
static struct amphora_tilemap_layer_t *deferred_transition;
static SDL_FRect map_rect;
static struct amphora_object_groups_t obj_groups;

void
Amphora_SetMap(const char *name, const float scale) {
	int i;
	struct render_list_node_t *render_list_node;

	Amphora_DestroyCurrentMap();
	if (!name) return;

	current_map.scale = scale ? scale : 1;
	if (Amphora_ParseMapToTexture(name) == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from map: %s\n", name);
		return;
	}
	for (i = 0; i < current_map.num_layers; i++) {
		render_list_node = Amphora_AddRenderListNode(100 * i);
		render_list_node->type = AMPH_OBJ_MAP;
		render_list_node->data = current_map.layers[i].texture;
		current_map.layers[i].node = render_list_node;
	}
}

void
Amphora_HideMapLayer(const char *name, int t) {
	int n = Amphora_GetMapLayerByName(name);

	if (n == -1) return;

	if (t < 1) {
		current_map.layers[n].node->display = false;
		return;
	}
	deferred_transition = &current_map.layers[n];
	deferred_transition->a_stp = 0xff / (((float)t / 1000) * (float) Amphora_GetFPS());
	deferred_transition->hiding = true;
}

void
Amphora_ShowMapLayer(const char *name, int t) {
	int n = Amphora_GetMapLayerByName(name);

	if (n == -1) return;

	current_map.layers[n].node->display = true;
	if (t < 1) {
		current_map.layers[n].a = 0xff;
		return;
	}
	deferred_transition = &current_map.layers[n];
	deferred_transition->a_stp = 0xff / (((float)t / 1000) * (float) Amphora_GetFPS());
	deferred_transition->hiding = false;
}

/*
 * Internal functions
 */

int
Amphora_InitMaps(void) {
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
#define LOADMAP(name, path) Amphora_HTSetValue(#name, char *, name##_tm, map_data); \
			Amphora_HTSetValue(#name, int, name##_tm_size, map_sizes);
	MAPS
#undef LOADMAP
#endif
#ifdef DEBUG
	for (i = 0; i < MAPS_COUNT; i++) {
		SDL_Log("Found map %s\n", map_names[i]);
	}
#endif
	obj_groups.max_c = OBJ_GRP_BATCH_SIZE;
	if (!((obj_groups.labels = SDL_malloc(OBJ_GRP_BATCH_SIZE * sizeof(char *))))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate object group labels\n");
		return -1;
	}
	if (!((obj_groups.c_rects = SDL_malloc(OBJ_GRP_BATCH_SIZE * sizeof(int))))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate object group rectangle count\n");
		return -1;
	}
	if (!((obj_groups.rects = SDL_malloc(OBJ_GRP_BATCH_SIZE * sizeof(SDL_FRect *))))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate object group rectangle list\n");
		return -1;
	}

	return 0;
}

SDL_FRect *
Amphora_GetMapRectangle(void) {
	return &map_rect;
}

void
Amphora_DestroyCurrentMap(void) {
	int i;

	for (i = 0; i < current_map.num_layers; i++) {
		SDL_free(current_map.layer_names[i]);
		SDL_DestroyTexture(current_map.layers[i].texture);
		current_map.layers[i].node->garbage = true;
	}
	if (current_map.num_layers > 0) {
		SDL_free(current_map.layers);
		SDL_free(current_map.layer_names);
		current_map.layers = NULL;
		current_map.layer_names = NULL;
	}
	current_map.num_layers = 0;
}

void
Amphora_FreeObjectGroup(void) {
	int i;

	for (i = 0; i < obj_groups.c; i++) {
		SDL_free(obj_groups.labels[i]);
		SDL_free(obj_groups.rects[i]);
		obj_groups.labels[i] = NULL;
		obj_groups.rects[i] = NULL;

	}

	obj_groups.c = 0;
}

void
Amphora_FreeAllObjectGroups (void) {
	Amphora_FreeObjectGroup();
	SDL_free(obj_groups.labels);
	SDL_free(obj_groups.c_rects);
	SDL_free(obj_groups.rects);
}

SDL_FRect *
Amphora_GetRectsByGroup(const char *name, int *c) {
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

void
Amphora_ProcessDeferredTransition(void) {
	if (!deferred_transition) return;

	if (deferred_transition->hiding) {
		deferred_transition->a -= deferred_transition->a_stp;
		if (deferred_transition->a <= 1) {
			deferred_transition->node->display = false;
			deferred_transition->a = 0;
			SDL_SetTextureAlphaMod(deferred_transition->texture, (Uint8)deferred_transition->a);
			deferred_transition = NULL;
			return;
		}
	} else {
		deferred_transition->a += deferred_transition->a_stp;
		if (deferred_transition->a >= 0xff) {
			deferred_transition->a = 0xff;
			SDL_SetTextureAlphaMod(deferred_transition->texture, (Uint8)deferred_transition->a);
			deferred_transition = NULL;
			return;
		}
	}
	SDL_SetTextureAlphaMod(deferred_transition->texture, (Uint8)deferred_transition->a);
}

/*
 * Private functions
 */

static int
Amphora_ParseMapToTexture(const char *name) {
	SDL_Renderer *renderer = Amphora_GetRenderer();
	cute_tiled_map_t *map = cute_tiled_load_map_from_memory(Amphora_HTGetValue(name, char *, map_data),
								Amphora_HTGetValue(name, int, map_sizes), 0);
	cute_tiled_layer_t *layer = map->layers;
	cute_tiled_tileset_t *tileset = map->tilesets;
	int tileset_img_w, tileset_img_h;
	SDL_Texture *tileset_img = Amphora_GetIMGTextureByName(tileset->name.ptr);
	int pixel_width = map->width * map->tilewidth;
	int pixel_height = map->height * map->tileheight;
	int map_base_w, map_base_h;
	int i;

	while (layer) {
		if (SDL_strcmp(layer->type.ptr, "tilelayer") == 0) current_map.num_layers++;
		layer = layer->next;
	}
	if (!((current_map.layers = SDL_malloc(current_map.num_layers * sizeof(struct amphora_tilemap_layer_t))))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not allocate map layers\n");
		return -1;
	}
	if (!((current_map.layer_names = SDL_malloc(current_map.num_layers * sizeof(char *))))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not allocate map layers labels\n");
		return -1;
	}
	layer = map->layers;
	if (SDL_strcmp(map->orientation.ptr, "orthogonal") == 0) {
		current_map.orientation = MAP_ORTHOGONAL;
		for (i = 0; i < current_map.num_layers; i++) {
			current_map.layers[i].texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
								  SDL_TEXTUREACCESS_TARGET, pixel_width,
								  pixel_height);
		}
	} else if (SDL_strcmp(map->orientation.ptr, "isometric") == 0) {
		current_map.orientation = MAP_ISOMETRIC;
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
			Amphora_ParseTileLayer(map, layer, tileset_img_w, tileset_img, i++);
		} else if (SDL_strcmp(layer->type.ptr, "objectgroup") == 0) {
			Amphora_ParseObjectGroup(layer);
		}
		layer = layer->next;
	}
	SDL_SetRenderTarget(renderer, NULL);
	cute_tiled_free_map(map);
	SDL_QueryTexture(current_map.layers[0].texture, NULL, NULL, &map_base_w, &map_base_h);
	map_rect.w = (float)map_base_w * current_map.scale;
	map_rect.h = (float)map_base_h * current_map.scale;

	return 0;
}

static int
Amphora_ParseTileLayer(const cute_tiled_map_t *map, const cute_tiled_layer_t *layer, int tileset_img_w, SDL_Texture *tileset_img, int n) {
	SDL_Renderer *renderer = Amphora_GetRenderer();
	SDL_Rect tile_s = { .w = map->tilewidth, .h = map->tileheight };
	SDL_Rect tile_d = { .w = map->tilewidth, .h = map->tileheight };
	int i, tile_idx, row;
	size_t name_len;

	name_len = SDL_strlen(layer->name.ptr);
	if ((current_map.layer_names[n] = SDL_malloc(name_len + 1))) {
		SDL_strlcpy(current_map.layer_names[n], layer->name.ptr, name_len + 1);
	} else {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not allocate space for layer name!");
	}
	current_map.layers[n].a = 0xff;
	SDL_SetTextureBlendMode(current_map.layers[n].texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, current_map.layers[n].texture);
	for (i = 0; i < layer->data_count; i++) {
		tile_idx = layer->data[i] - 1;
		row = ((i * map->tilewidth) / (map->width * map->tilewidth));
		tile_s.x = (tile_idx * map->tilesets->tilewidth) % tileset_img_w;
		tile_s.y = ((tile_idx * map->tilesets->tilewidth) / tileset_img_w) * map->tileheight;
		switch (current_map.orientation) {
			case MAP_ORTHOGONAL:
				tile_d.x = (i * map->tilewidth) % (map->width * map->tilewidth);
				tile_d.y = row * map->tileheight;
				break;
			case MAP_ISOMETRIC:
				tile_d.x = (i * map->tilewidth) % (map->width * map->tilewidth) +
					   (((map->width * map->tilewidth) / 2) - ((i % map->width) *
										   (map->tilewidth /
										    2)) -
					    (row * (map->tilewidth / 2)));
				tile_d.y = row * map->tileheight +
					   ((i % map->width) * (map->tileheight / 2)) -
					   (row * (map->tileheight / 2));
				break;
		}
		SDL_RenderCopy(renderer, tileset_img, &tile_s, &tile_d);
	}

	return 0;
}

static int
Amphora_ParseObjectGroup(const cute_tiled_layer_t *layer) {
	int i;
	cute_tiled_object_t *object;

	if (++obj_groups.c > obj_groups.max_c) {
		obj_groups.max_c += OBJ_GRP_BATCH_SIZE;
		if (!((obj_groups.labels = SDL_realloc(obj_groups.labels,
						       obj_groups.max_c * sizeof(char *))))) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR,
				     "Failed to reallocate object group labels\n");
			return -1;
		}
		if (!((obj_groups.c_rects = SDL_realloc(obj_groups.c_rects,
							obj_groups.max_c * sizeof(int))))) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR,
				     "Failed to reallocate object group rectangle count\n");
			return -1;
		}
		if (!((obj_groups.rects = SDL_realloc(obj_groups.rects,
						      obj_groups.max_c * sizeof(SDL_FRect *))))) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR,
				     "Failed to reallocate object group rectangle list\n");
			return -1;
		}
	}
	object = layer->objects;
	i = 0;
	while (object) {
		i++;
		object = object->next;
	}
	object = layer->objects;
	if (!((obj_groups.labels[obj_groups.c - 1] = SDL_malloc(SDL_strlen(layer->name.ptr) + 1)))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate space for label\n");
		return -1;
	}
	if (!((obj_groups.rects[obj_groups.c - 1] = SDL_malloc(i * sizeof(SDL_FRect))))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate object group rectangles\n");
		return -1;
	}
	SDL_strlcpy(obj_groups.labels[obj_groups.c - 1], layer->name.ptr, SDL_strlen(layer->name.ptr) + 1);
	obj_groups.c_rects[obj_groups.c - 1] = i;
	i = 0;
	while (object) {
		obj_groups.rects[obj_groups.c - 1][i].x = object->x * current_map.scale;
		obj_groups.rects[obj_groups.c - 1][i].y = object->y * current_map.scale;
		obj_groups.rects[obj_groups.c - 1][i].w = object->width * current_map.scale;
		obj_groups.rects[obj_groups.c - 1][i].h = object->height * current_map.scale;
		i++;
		object = object->next;
	}

	return 0;
}

static int
Amphora_GetMapLayerByName(const char *name) {
	int i;

	for (i = 0; i < current_map.num_layers; i++) {
		if (SDL_strcmp(name, current_map.layer_names[i]) == 0) return i;
	}
	return -1;
}

#endif
