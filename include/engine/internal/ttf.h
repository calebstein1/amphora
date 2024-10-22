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
	Uint32 idx;
	bool stationary : 1;
};

struct open_font_t {
	TTF_Font *font;
	int pt;
};

int init_fonts(void);
void free_fonts(void);
void free_all_strings(void);

#endif /* TTF_INTERNAL_H */
