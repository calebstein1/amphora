#ifndef TTF_INTERNAL_H
#define TTF_INTERNAL_H

#include "engine/ttf.h"

struct amphora_message_t {
	SDL_Texture *texture;
	SDL_Rect rectangle;
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

int init_fonts(void);
void free_fonts(void);

#endif /* TTF_INTERNAL_H */
