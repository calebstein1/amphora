#ifdef WIN32
#include <windows.h>
#endif

#include "engine/ttf.h"

#include "config.h"

#ifdef ENABLE_FONTS
#define MAX_STRING_LENGTH 1024

/* Private structs */
struct amphora_message_t {
	SDL_Texture *texture;
	SDL_Rect rectangle;
	enum fonts_e font;
	int pt;
	size_t len;
	size_t n;
	AmphoraColor color;
	char text[MAX_STRING_LENGTH];
};

struct open_font_t {
	TTF_Font *font;
	int pt;
};

/* Prototypes for private functions */
SDL_Texture *render_string_to_texture(AmphoraMessage *msg);

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
	if (*msg) return *msg;

	if (!((*msg = SDL_malloc(sizeof(struct amphora_message_t))))) {
		return NULL;
	}

	(*msg)->font = font_name;
	(*msg)->pt = pt;
	(*msg)->len = SDL_strlen(text);
	(*msg)->n = 0;
	(*msg)->color = color;
	(*msg)->rectangle.x = x;
	(*msg)->rectangle.y = y;
	SDL_strlcpy((*msg)->text, text, MAX_STRING_LENGTH);

	(*msg)->texture = render_string_to_texture(*msg);

	return *msg;
}

size_t
get_string_length(const AmphoraMessage *msg) {
	return msg->len;
}

AmphoraMessage *
update_string_text(AmphoraMessage **msg, const char *text) {
	(*msg)->len = SDL_strlen(text);
	SDL_strlcpy((*msg)->text, text, MAX_STRING_LENGTH);
	SDL_DestroyTexture((*msg)->texture);
	(*msg)->texture = render_string_to_texture(*msg);

	return *msg;
}

AmphoraMessage *
update_string_n(AmphoraMessage **msg, size_t n) {
	if (n >= (*msg)->len) n = 0;
	(*msg)->n = n;
	SDL_DestroyTexture((*msg)->texture);
	(*msg)->texture = render_string_to_texture(*msg);

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

/*
 * Private functions
 */

SDL_Texture *
render_string_to_texture(AmphoraMessage *msg) {
	enum fonts_e font_name = msg->font;
	int pt = msg->pt;
	size_t n = msg->n;
	const char *text = msg->text;
	const SDL_Color text_color = { .r = msg->color.r, .g = msg->color.g, .b = msg->color.b, .a = 0xff };
	TTF_Font *font = NULL;
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;
	char *n_buff = NULL;

	if (n) {
		if ((n_buff = SDL_malloc(n))) {
			SDL_strlcpy(n_buff, text, n);
		} else {
			SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Failed to allocate buffer for partial string\n");
			n = 0;
		}
	}

	if (open_fonts[font_name].pt != pt) {
		TTF_CloseFont(open_fonts[font_name].font);
		SDL_RWseek(fonts[font_name], 0, RW_SEEK_SET);
		open_fonts[font_name] = (struct open_font_t){ .font = TTF_OpenFontRW(fonts[font_name],0, pt), .pt = pt };
	}

	font = open_fonts[font_name].font;
	surface = TTF_RenderUTF8_Blended(font, n ? n_buff : text, text_color);
	texture = SDL_CreateTextureFromSurface(get_renderer(), surface);
	TTF_SizeUTF8(font, n ? n_buff : text, &msg->rectangle.w, &msg->rectangle.h);
	SDL_FreeSurface(surface);
	if (n_buff) SDL_free(n_buff);

	return texture;
}

#endif
