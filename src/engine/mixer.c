#ifndef DISABLE_MIXER
#ifdef WIN32
#include <windows.h>
#endif

#include "engine/internal/mixer.h"

/* File-scoped variables */
static SDL_RWops *sfx[SFX_COUNT];
static Mix_Chunk *open_sfx[SFX_COUNT];
static const char *sfx_names[] = {
#define LOADSFX(name, path) #name,
	SFX
#undef LOADSFX
};

/*
 * Internal functions
 */

int
init_sfx(void) {
	int i;
#ifdef WIN32
	HRSRC sfx_info;
	HGLOBAL sfx_resource;
	SDL_RWops *sfx_rw;

	for (i = 0; i < SFX_COUNT; i++) {
		if (!((sfx_info = FindResourceA(NULL, sfx_names[i], "SFX")))) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to locate sfx resource... Amphora will crash now\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to locate sfx resource... Amphora will crash now", 0);
			return -1;
		}
		if (!((sfx_resource = LoadResource(NULL, sfx_info)))) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to load sfx resource... Amphora will crash now\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to load sfx resource... Amphora will crash now", 0);
			return -1;
		}
		sfx_rw = SDL_RWFromConstMem(sfx_resource, SizeofResource(NULL, sfx_info));
		sfx[i] = sfx_rw;
	}
#else
#define LOADSFX(name, path) extern char name##_sf[]; extern int name##_sf_size;
	SFX
#undef LOADSFX
	SDL_RWops **sfx_ptr = sfx;
#define LOADSFX(name, path) *sfx_ptr = SDL_RWFromConstMem(name##_sf, name##_sf_size); sfx_ptr++;
	SFX
#undef LOADSFX
#endif
#ifdef DEBUG
	for (i = 0; i < SFX_COUNT; i++) {
		SDL_Log("Found sfx %s\n", sfx_names[i]);
	}
#endif

	return 0;
}

#endif
