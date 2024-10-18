#ifdef WIN32
#include <windows.h>
#endif

#include "engine/ttf.h"

#include "config.h"

#ifdef ENABLE_FONTS
#define OPEN_MESSAGES_BATCH_COUNT 8

/* Private structs */
struct amphora_message_t {
	SDL_Texture *texture;
	SDL_Rect rectangle;
	enum fonts_e font;
	int pt;
	size_t len;
	size_t n;
	AmphoraColor color;
	char *text;
	Uint32 idx;
	bool stationary : 1;
};

struct open_font_t {
	TTF_Font *font;
	int pt;
};

/* Prototypes for private functions */
SDL_Texture *render_string_to_texture(AmphoraMessage *msg);
void free_string(AmphoraMessage *msg);

/* File-scoped variables */
static SDL_RWops *fonts[FONTS_COUNT];
static struct open_font_t open_fonts[FONTS_COUNT];
static struct amphora_message_t **open_messages;
static Uint32 open_message_count;
static bool allow_leaks = false;

int
init_fonts(void) {
#ifdef WIN32
	HRSRC ttf_info;
	HGLOBAL ttf_resource;
	SDL_RWops *ttf_rw;
	int i;
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
#define LOADFONT(name, path) extern char name[]; extern int name##_size;
	FONTS
#undef LOADFONT
	SDL_RWops **fonts_ptr = fonts;
#define LOADFONT(name, path) *fonts_ptr = SDL_RWFromConstMem(name, name##_size); fonts_ptr++;
	FONTS
#undef LOADFONT
#endif
	if ((open_messages = SDL_calloc(OPEN_MESSAGES_BATCH_COUNT, sizeof(struct amphora_message_t *)))) {
		open_message_count = OPEN_MESSAGES_BATCH_COUNT;
	} else {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to initialize open messages tracker\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Memory allocation error", "Failed to initialize message tracker... Amphora will attempt to continue with memory leaks, but a crash is likely incoming ¯\\_(ツ)_/¯", 0);
		allow_leaks = true;
	}
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
create_string(AmphoraMessage **msg, const enum fonts_e font_name, const int pt, const int x, const int y, const char *text) {
	Uint32 i = 0;

	if (*msg) return *msg;

	if (!((*msg = SDL_malloc(sizeof(struct amphora_message_t))))) {
		return NULL;
	}
	if (!(((*msg)->text = SDL_malloc(SDL_strlen(text) + 1)))) {
		return NULL;
	}

#define black true
#define white false
	(*msg)->font = font_name;
	(*msg)->pt = pt;
	(*msg)->len = SDL_strlen(text);
	(*msg)->n = 0;
	(*msg)->color = BG_COLOR_MODE ? get_white() : get_black();
	(*msg)->rectangle.x = x;
	(*msg)->rectangle.y = y;
	(*msg)->stationary = false;
	SDL_strlcpy((*msg)->text, text, SDL_strlen(text) + 1);
#undef black
#undef white

	(*msg)->texture = render_string_to_texture(*msg);

	if (allow_leaks) return *msg;

	while (open_messages[i]) {
		if (++i == open_message_count) {
			open_message_count += OPEN_MESSAGES_BATCH_COUNT;
			open_messages = SDL_realloc(open_messages, open_message_count * sizeof(struct amphora_message_t *));
			SDL_memset(&open_messages[open_message_count - OPEN_MESSAGES_BATCH_COUNT], 0, OPEN_MESSAGES_BATCH_COUNT * sizeof(struct amphora_message_t *));
			break;
		}
	}
	open_messages[i] = *msg;
	(*msg)->idx = i;

	return *msg;
}

AmphoraMessage *
create_stationary_string(AmphoraMessage **msg, const enum fonts_e font_name, const int pt, const int x, const int y, const char *text) {
	*msg = create_string(msg, font_name, pt, x, y, text);
	(*msg)->stationary = true;

	return *msg;
}

size_t
get_string_length(const AmphoraMessage *msg) {
	return msg->len;
}

AmphoraMessage *
update_string_text(AmphoraMessage **msg, const char *text) {
	(*msg)->len = SDL_strlen(text);
	SDL_free((*msg)->text);
	if (!(((*msg)->text = SDL_malloc(SDL_strlen(text) + 1)))) {
		return NULL;
	}
	SDL_strlcpy((*msg)->text, text, (*msg)->len + 1);
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
	SDL_Rect pos_adj;
	const Point camera = get_camera();
	const Uint16 pixel_size = get_pixel_size();

	if (msg->stationary) {
		pos_adj = (SDL_Rect){
			.x = msg->rectangle.x * pixel_size - DECODE_POSITION32(camera.x),
			.y = msg->rectangle.y * pixel_size - DECODE_POSITION32(camera.y),
			.w = msg->rectangle.w,
			.h = msg->rectangle.h
		};
	} else {
		pos_adj = (SDL_Rect){
			.x = msg->rectangle.x > 0 ? msg->rectangle.x * pixel_size : get_real_window_size().x - (msg->rectangle.x * -1 * pixel_size),
			.y = msg->rectangle.y > 0 ? msg->rectangle.y * pixel_size : get_real_window_size().y - (msg->rectangle.y * -1 * pixel_size),
			.w = msg->rectangle.w,
			.h = msg->rectangle.h
		};
	}

	SDL_RenderCopy(get_renderer(), msg->texture, NULL, &pos_adj);
}

void
free_all_strings(void) {
	Uint32 i;

	for (i = 0; i < open_message_count; i++) {
		if (!open_messages[i]) continue;

		free_string(open_messages[i]);
	}
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
	texture = SDL_CreateTextureFromSurface(get_renderer(), surface);
	TTF_SizeUTF8(font, n ? n_buff : text, &msg->rectangle.w, &msg->rectangle.h);
	SDL_FreeSurface(surface);
	if (n_buff) SDL_free(n_buff);

	return texture;
}

void
free_string(AmphoraMessage *msg) {
	if (!msg) return;

	open_messages[msg->idx] = NULL;
	SDL_DestroyTexture(msg->texture);
	SDL_free(msg->text);
	SDL_free(msg);
}

#endif
