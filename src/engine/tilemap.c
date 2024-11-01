#ifndef DISABLE_TILEMAP
#ifdef WIN32
#include <windows.h>
#endif

#include "engine/internal/render.h"
#include "engine/internal/tilemap.h"

/* File-scoped vairables */
static char *map_names[] = {
#define LOADMAP(name, path) #name,
	MAPS
#undef LOADMAP
};
static Uint32 map_sizes[MAPS_COUNT];
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
	Uint32 *map_sizes_ptr = map_sizes;
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

#endif

/*
 * Orivate functions
 */

void
parse_map(enum tilemaps_e map) {
}
