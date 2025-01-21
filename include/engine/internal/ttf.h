#ifndef TTF_INTERNAL_H
#define TTF_INTERNAL_H

#include "engine/ttf.h"
#include "engine/internal/render.h"

struct amphora_message_t {
	enum amphora_object_type_e type;
	SDL_Texture *texture;
	SDL_FRect rectangle;
	enum fonts_e font;
	int pt;
	size_t len;
	size_t n;
	SDL_Color color;
	char *text;
	struct render_list_node_t *render_list_node;
};

struct open_font_t {
	TTF_Font *font;
	int pt;
};

int Amphora_InitFonts(void);
void Amphora_FreeFonts(void);

#endif /* TTF_INTERNAL_H */
