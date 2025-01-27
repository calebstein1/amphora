#ifdef WIN32
#include <windows.h>
#endif

#include <engine/internal/render.h>
#include "engine/internal/ttf.h"

#include "config.h"

#ifndef DISABLE_FONTS

/* Prototypes for private functions */
static int Amphora_GetFontByName(const char *name);
static SDL_Texture *Amphora_RenderStringToTexture(AmphoraString *msg);

/* File-scoped variables */
static SDL_RWops *fonts[FONTS_COUNT];
static struct open_font_t open_fonts[FONTS_COUNT];
static const char *font_names[] = {
#define LOADFONT(name, path) #name,
	FONTS
#undef LOADFONT
};

AmphoraString *
Amphora_CreateString(AmphoraString **msg, const char *name, const int pt, const float x, const float y, const int order, const SDL_Color color, const char *text, const bool stationary) {
	struct render_list_node_t *render_list_node = Amphora_AddRenderListNode(order);
	int idx;

	if (*msg) return *msg;

	if ((idx = Amphora_GetFontByName(name)) == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to locate font %s\n", name);
		return NULL;
	}

	if (!((*msg = SDL_malloc(sizeof(struct amphora_message_t))))) {
		return NULL;
	}
	if (!(((*msg)->text = SDL_malloc(SDL_strlen(text) + 1)))) {
		return NULL;
	}

	(*msg)->type = AMPH_OBJ_TXT;
	(*msg)->font = idx;
	(*msg)->pt = pt;
	(*msg)->len = SDL_strlen(text);
	(*msg)->n = 0;
	(*msg)->color = color;
	(*msg)->rectangle.x = x;
	(*msg)->rectangle.y = y;
	(*msg)->render_list_node = render_list_node;
	render_list_node->type = AMPH_OBJ_TXT;
	render_list_node->data = *msg;
	render_list_node->stationary = stationary;
	SDL_strlcpy((*msg)->text, text, SDL_strlen(text) + 1);

	(*msg)->texture = Amphora_RenderStringToTexture(*msg);

	return *msg;
}

size_t
Amphora_GetStringLength(const AmphoraString *msg) {
	return msg->len;
}

const char *
Ampohra_GetStringText(AmphoraString *msg) {
	return msg->text;
}

AmphoraString *
Amphora_UpdateStringText(AmphoraString **msg, const char *fmt, ...) {
	va_list args;
	char text[4096];

	va_start(args, fmt);
	vsnprintf(text, 4096, fmt, args);

	(*msg)->len = SDL_strlen(text);
	SDL_free((*msg)->text);
	if (!(((*msg)->text = SDL_malloc(SDL_strlen(text) + 1)))) {
		return NULL;
	}
	SDL_strlcpy((*msg)->text, text, (*msg)->len + 1);
	SDL_DestroyTexture((*msg)->texture);
	(*msg)->texture = Amphora_RenderStringToTexture(*msg);

	return *msg;
}

AmphoraString *
Amphora_UpdateStringCharsDisplayed(AmphoraString **msg, size_t n) {
	if (n >= (*msg)->len) n = 0;
	(*msg)->n = n;
	SDL_DestroyTexture((*msg)->texture);
	(*msg)->texture = Amphora_RenderStringToTexture(*msg);

	return *msg;
}

void
Amphora_FreeString(AmphoraString **msg) {
	if (!*msg) return;

	SDL_DestroyTexture((*msg)->texture);
	SDL_free((*msg)->text);
	(*msg)->render_list_node->garbage = true;
	SDL_free(*msg);
	*msg = NULL;
}

void
Amphora_RenderString(const AmphoraString *msg) {
	SDL_FRect pos_adj;
	const Vector2f camera = Ampohra_GetCamera();
	Vector2 logical_size = Amphora_GetRenderLogicalSize();

	if (msg->render_list_node->stationary) {
		pos_adj = (SDL_FRect){
			.x = msg->rectangle.x > 0 ? msg->rectangle.x : (float) Amphora_GetResolution().x + msg->rectangle.x - msg->rectangle.w,
			.y = msg->rectangle.y > 0 ? msg->rectangle.y : (float) Amphora_GetResolution().y + msg->rectangle.y - msg->rectangle.h,
			.w = msg->rectangle.w,
			.h = msg->rectangle.h
		};
	} else {
		pos_adj = (SDL_FRect){
			.x = msg->rectangle.x - camera.x,
			.y = msg->rectangle.y - camera.y,
			.w = msg->rectangle.w,
			.h = msg->rectangle.h
		};
	}

	if (msg->render_list_node->stationary) Amphora_SetRenderLogicalSize(Amphora_GetResolution());
	Amphora_RenderTexture(msg->texture, NULL, &pos_adj, 0, 0);
	if (msg->render_list_node->stationary) Amphora_SetRenderLogicalSize(logical_size);
}

/*
 * Internal functions
 */

int
Amphora_InitFonts(void) {
	int i;
#ifdef WIN32
	HRSRC ttf_info;
	HGLOBAL ttf_resource;
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
		ttf_rw = SDL_RWFromConstMem(ttf_resource, SizeofResource(NULL, ttf_info));
		fonts[i] = ttf_rw;
	}
#else
#define LOADFONT(name, path) extern char name##_ft[]; extern int name##_ft_size;
	FONTS
#undef LOADFONT
	SDL_RWops **fonts_ptr = fonts;
#define LOADFONT(name, path) *fonts_ptr = SDL_RWFromConstMem(name##_ft, name##_ft_size); fonts_ptr++;
	FONTS
#undef LOADFONT
#endif
#ifdef DEBUG
	for (i = 0; i < FONTS_COUNT; i++) {
		SDL_Log("Found font %s\n", font_names[i]);
	}
#endif

	return 0;
}

void
Amphora_FreeFonts(void) {
	int i;

	for (i = 0; i < FONTS_COUNT; i++) {
		SDL_RWclose(fonts[i]);
		TTF_CloseFont(open_fonts[i].font);
	}
}

/*
 * Private functions
 */

static int
Amphora_GetFontByName(const char *name) {
	int i;

	for (i = 0; i < FONTS_COUNT; i++) {
		if (SDL_strcmp(name, font_names[i]) == 0) return i;
	}
	return -1;
}

static SDL_Texture *
Amphora_RenderStringToTexture(AmphoraString *msg) {
	enum fonts_e font_name = msg->font;
	int pt = msg->pt;
	int msg_rect_w, msg_rect_h;
	size_t n = msg->n;
	const char *text = msg->text;
	const SDL_Color text_color = msg->color;
	TTF_Font *font = NULL;
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;
	char *n_buff = NULL;

	if (n) {
		if ((n_buff = SDL_malloc(n + 1))) {
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
	texture = SDL_CreateTextureFromSurface(Amphora_GetRenderer(), surface);
	TTF_SizeUTF8(font, n ? n_buff : text, &msg_rect_w, &msg_rect_h);
	msg->rectangle.w = (float)msg_rect_w;
	msg->rectangle.h = (float)msg_rect_h;
	SDL_FreeSurface(surface);
	if (n_buff) SDL_free(n_buff);

	return texture;
}
#endif
