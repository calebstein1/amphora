#ifdef WIN32
#include <windows.h>
#endif

#include "engine/ttf.h"

#include "config.h"

struct amphora_message_t {
	SDL_Texture *texture;
	SDL_Rect rectangle;
};

/* File-scoped variables */
static SDL_RWops *fonts[FONTS_COUNT];

int
load_fonts(void) {
	int i;
#ifdef WIN32
	HRSRC ttf_info;
	HGLOBAL ttf_resource;
	SDL_RWops *ttf_rw;
	const char *font_names[] = {
#define LOADFONT(name, path) #name,
		FONTS
#undef LOADFONT
	};

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
		ttf_rw = SDL_RWFromConstMem(ttf_resource, SizeofResource(NULL, ttf_info));
		fonts[i] = ttf_rw;
	}
#else
#warning TTF Fonts are not yet supported on non-Windows systems
#endif
	return 0;
}

void
free_fonts(void) {
	int i;

	for (i = 0; i < FONTS_COUNT; i++) {
		SDL_RWclose(fonts[i]);
	}
}

AmphoraMessage *
create_string(AmphoraMessage **amsg, const enum fonts_e font_name, const int pt, const int x, const int y, const AmphoraColor color, const char *text) {
	TTF_Font *font;
	const SDL_Color text_color = { .r = color.r, .g = color.g, .b = color.b, .a = 0xff};
	SDL_Surface *surface;

	if (*amsg) return *amsg;

	if (!((*amsg = malloc(sizeof(struct amphora_message_t))))) {
		return NULL;
	}

	(*amsg)->rectangle.x = x;
	(*amsg)->rectangle.y = y;

	font = TTF_OpenFontRW(fonts[font_name], 0, pt);
	surface = TTF_RenderUTF8_Blended(font, text, text_color);
	(*amsg)->texture = SDL_CreateTextureFromSurface(get_renderer(), surface);
	TTF_SizeUTF8(font, text, &(*amsg)->rectangle.w, &(*amsg)->rectangle.h);
	TTF_CloseFont(font);
	SDL_FreeSurface(surface);

	return *amsg;
}

void
render_string(const AmphoraMessage *msg) {
	SDL_RenderCopy(get_renderer(), msg->texture, NULL, &msg->rectangle);
}

void
free_string(AmphoraMessage **amsg) {
	free(*amsg);
	*amsg = NULL;
}
