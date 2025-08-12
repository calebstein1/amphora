#ifndef TTF_INTERNAL_H
#define TTF_INTERNAL_H

#include "engine/ttf.h"
#include "engine/internal/render.h"

struct amphora_message_t {
	enum amphora_object_type_e type;
	SDL_FRect rectangle;
	SDL_Texture *texture;
	TTF_Font *font_ptr;
	int pt;
	size_t len;
	size_t n;
	SDL_Color color;
	char *text;
	char *n_buff;
	struct render_list_node_t *render_list_node;
};

int Amphora_InitFonts(void);
void Amphora_FreeAllFonts(void);
void Amphora_CloseFonts(void);

#endif /* TTF_INTERNAL_H */
