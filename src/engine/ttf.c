#ifdef WIN32
#include <windows.h>
#endif

#include "engine/ttf.h"

#include "config.h"

#ifdef ENABLE_FONTS

/* Private structs */
struct amphora_message_t {
	SDL_Texture *texture;
	SDL_Rect rectangle;
};

struct open_font_t {
	TTF_Font *font;
	int pt;
};

/* File-scoped variables */
static SDL_RWops *fonts[FONTS_COUNT];
static struct open_font_t open_fonts[FONTS_COUNT];

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
	(void)i;
	SDL_LogWarn(SDL_LOG_CATEGORY_SYSTEM, "TTF loading unsupported on non-Windows systems\n");
#endif
	return 0;
}

void
free_fonts(void) {
	int i;

	for (i = 0; i < FONTS_COUNT; i++) {
		SDL_RWclose(fonts[i]);
		TTF_CloseFont(open_fonts[i].font);
	}
}

AmphoraMessage *
create_string(AmphoraMessage **msg, const enum fonts_e font_name, const int pt, const int x, const int y, const AmphoraColor color, const char *text) {
	TTF_Font *font;
	const SDL_Color text_color = { .r = color.r, .g = color.g, .b = color.b, .a = 0xff};
	SDL_Surface *surface;

	if (*msg) return *msg;

	if (!((*msg = SDL_malloc(sizeof(struct amphora_message_t))))) {
		return NULL;
	}

	if (open_fonts[font_name].pt != pt) {
		TTF_CloseFont(open_fonts[font_name].font);
		SDL_RWseek(fonts[font_name], 0, RW_SEEK_SET);
		open_fonts[font_name] = (struct open_font_t){ .font = TTF_OpenFontRW(fonts[font_name], 0, pt), .pt = pt };
	}

	(*msg)->rectangle.x = x;
	(*msg)->rectangle.y = y;

	font = open_fonts[font_name].font;
	surface = TTF_RenderUTF8_Blended(font, text, text_color);
	(*msg)->texture = SDL_CreateTextureFromSurface(get_renderer(), surface);
	TTF_SizeUTF8(font, text, &(*msg)->rectangle.w, &(*msg)->rectangle.h);
	SDL_FreeSurface(surface);

	return *msg;
}

void
render_string(const AmphoraMessage *msg) {
	SDL_RenderCopy(get_renderer(), msg->texture, NULL, &msg->rectangle);
}

void
free_string(AmphoraMessage **msg) {
	if (!*msg) return;

	SDL_DestroyTexture((*msg)->texture);
	SDL_free(*msg);
	*msg = NULL;
}

#endif
