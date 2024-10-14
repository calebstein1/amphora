#ifdef WIN32
#include <windows.h>
#endif

#include "engine/ttf.h"

#include "config.h"

/* File-scoped variables */
static char *font_names[] = {
#define LOADFONT(name, path) #name,
	FONTS
#undef LOADFONT
};
static SDL_RWops *fonts[FONTS_COUNT];

int
load_fonts(void) {
	int i;
#ifdef WIN32
	HRSRC ttf_info;
	HGLOBAL ttf_resource;
	DWORD ttf_size;
	SDL_RWops *ttf_rw;

	for (i = 0; i < FONTS_COUNT; i++) {
		if (!((ttf_info = FindResourceA(NULL, font_names[i], "TTF_FONT")))) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to locate font resource... Amphora will crash now\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to locate font resource... Amphora will crash now", 0);
			return -1;
		}
		if (!((ttf_resource = LoadResource(NULL, ttf_info)))) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to load font resource... Amphora will crash now\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to load font resource... Amphora will crash now", 0);
			return -1;
		}
		ttf_size = SizeofResource(NULL, ttf_resource);
		ttf_rw = SDL_RWFromConstMem(ttf_resource, ttf_size);
		fonts[i] = ttf_rw;
	}
#else
#warning TTF Fonts are not yet supported on non-Windows systems
#endif
	return 0;
}