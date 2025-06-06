#include "engine/internal/error.h"
#include "engine/internal/events.h"
#include "engine/internal/ht_hash.h"
#include "engine/internal/img.h"
#include "engine/internal/lib.h"
#include "engine/internal/render.h"
#include "engine/internal/tilemap.h"
#include "config.h"

#ifndef DISABLE_TILEMAP

#define CUTE_TILED_IMPLEMENTATION
#include "vendor/cute_tiled.h"

/* Prototypes for private functions */
static int Amphora_ParseMapToTexture(const char *name);
static int Amphora_ParseTileLayer(const cute_tiled_map_t *map, const cute_tiled_layer_t *layer, int tileset_img_w, SDL_Texture *tileset_img, int n);
static int Amphora_ParseObjectGroup(const cute_tiled_layer_t *layer);
static int Amphora_GetMapLayerByName(const char *name);
static void Amphora_ProcessDeferredTransition(void);

/* File-scoped variables */
static char *map_names[] = {
#define LOADMAP(name, path) #name,
	MAPS
#undef LOADMAP
};
static char *map_paths[] = {
#define LOADMAP(name, path) #path,
	MAPS
#undef LOADMAP
};
static HT_HashTable map_data;
static struct amphora_tilemap_t current_map;
static AmphoraFader transition_fader;
static bool transitioning = false;
static bool persist_shown = false;
static struct amphora_tilemap_layer_t *fade_layer;
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

const SDL_FRect *
Amphora_GetMapRectangle(void) {
	return &map_rect;
}

int
Amphora_HideMapLayer(const char *name, int t) {
	int i, n = Amphora_GetMapLayerByName(name);

	if (n == -1 || !current_map.layers[n].node->display || transitioning) return AMPHORA_STATUS_OK;

	fade_layer = &current_map.layers[n];
	if (t < 1) {
		current_map.layers[n].node->display = false;
		return AMPHORA_STATUS_OK;
	}
	transition_fader.timer = t;
	transition_fader.frames = transition_fader.timer / (1000 / Amphora_GetFPS());
	transition_fader.idx = 0;
	if (!((transition_fader.steps = SDL_malloc(transition_fader.frames * sizeof(Uint8))))) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to allocate memory for fade steps\n");
		return AMPHORA_STATUS_ALLOC_FAIL;
	}
	for (i = 0; i < (transition_fader.frames); i++) {
		transition_fader.steps[i] = 255 - i * 255 / (transition_fader.frames - 1);
	}
	transitioning = true;
	persist_shown = false;
	Amphora_RegisterEvent("amph_internal_map_layer_fade", Amphora_ProcessDeferredTransition);

	return AMPHORA_STATUS_OK;
}

int
Amphora_ShowMapLayer(const char *name, int t) {
	int i, n = Amphora_GetMapLayerByName(name);

	if (n == -1 || current_map.layers[n].node->display || transitioning) return AMPHORA_STATUS_OK;

	current_map.layers[n].node->display = true;
	fade_layer = &current_map.layers[n];
	if (t < 1) {
		SDL_SetTextureAlphaMod(fade_layer->texture, 0xff);
		return AMPHORA_STATUS_OK;
	}
	transition_fader.timer = t;
	transition_fader.frames = transition_fader.timer / (1000 / Amphora_GetFPS());
	transition_fader.idx = 0;
	if (!((transition_fader.steps = SDL_malloc(transition_fader.frames * sizeof(Uint8))))) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to allocate memory for fade steps\n");
		return AMPHORA_STATUS_ALLOC_FAIL;
	}
	for (i = 0; i < (transition_fader.frames); i++) {
		transition_fader.steps[i] = i * 255 / (transition_fader.frames - 1);
	}
	transitioning = true;
	persist_shown = true;
	Amphora_RegisterEvent("amph_internal_map_layer_fade", Amphora_ProcessDeferredTransition);

	return AMPHORA_STATUS_OK;
}

/*
 * Internal functions
 */

int
Amphora_InitMaps(void) {
	int i;

	map_data = HT_NewTable();
	for (i = 0; i < MAPS_COUNT; i++) {
		HT_StoreRef(map_names[i], map_paths[i], map_data);
#ifdef DEBUG
		SDL_Log("Found map %s\n", map_names[i]);
#endif
	}

	obj_groups.i = HT_NewTable();
	if (!((obj_groups.rects = SDL_malloc(HT_GetSize(obj_groups.i) * sizeof(SDL_FRect *))))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate object group rectangle list\n");
		return -1;
	}

	return 0;
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
	Uint32 i;

	for (i = 0; i < HT_GetCount(obj_groups.i); i++) {
        SDL_free(obj_groups.rects[i]);
        obj_groups.rects[i] = NULL;
	}

	HT_FreeTable(obj_groups.i);
	obj_groups.i = HT_NewTable();
	obj_groups.rects = SDL_realloc(obj_groups.rects, HT_GetSize(obj_groups.i) * sizeof(SDL_FRect *));
}

void
Amphora_FreeAllObjectGroups(void) {
	Amphora_FreeObjectGroup();
	SDL_free(obj_groups.rects);
	HT_FreeTable(obj_groups.i);
}

void
Amphora_CloseMapHashTables(void) {
	HT_FreeTable(map_data);
}

SDL_FRect *
Amphora_GetRectsByGroup(const char *name, Uint32 *c) {
	int i = (int)HT_GetValue(name, obj_groups.i);

	*c = HT_GetStatus(name, obj_groups.i);

	return obj_groups.rects[i];
}

/*
 * Private functions
 */

static int
Amphora_ParseMapToTexture(const char *name) {
	SDL_Renderer *renderer = Amphora_GetRenderer();
	cute_tiled_map_t *map = cute_tiled_load_map_from_file(HT_GetRef(name, char, map_data), NULL);
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

	return AMPHORA_STATUS_OK;
}

static int
Amphora_ParseObjectGroup(const cute_tiled_layer_t *layer) {
	int i;
	unsigned c = HT_GetCount(obj_groups.i), s = HT_GetSize(obj_groups.i);
	cute_tiled_object_t *object;

	HT_SetValue(layer->name.ptr, c, obj_groups.i);
	if (s != HT_GetSize(obj_groups.i)) {
		if (!((obj_groups.rects = SDL_realloc(obj_groups.rects,
						      HT_GetSize(obj_groups.i) * sizeof(SDL_FRect *))))) {
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
	if (!((obj_groups.rects[c] = SDL_malloc(i * sizeof(SDL_FRect))))) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate object group rectangles\n");
		return -1;
	}
	HT_SetStatus(layer->name.ptr, i, obj_groups.i);
	i = 0;
	while (object) {
		obj_groups.rects[c][i].x = object->x * current_map.scale;
		obj_groups.rects[c][i].y = object->y * current_map.scale;
		obj_groups.rects[c][i].w = object->width * current_map.scale;
		obj_groups.rects[c][i].h = object->height * current_map.scale;
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

static void
Amphora_ProcessDeferredTransition(void) {
	SDL_SetTextureAlphaMod(fade_layer->texture, transition_fader.steps[transition_fader.idx++]);
	if (transition_fader.idx == transition_fader.frames) {
		fade_layer->node->display = persist_shown;
		transitioning = false;
		SDL_free(transition_fader.steps);
		Amphora_UnregisterEvent("amph_internal_map_layer_fade");
	}
}

#endif
