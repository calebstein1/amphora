#ifndef AMPHORA_TTF_H
#define AMPHORA_TTF_H

#include "SDL.h"
#include "SDL_ttf.h"

#include "engine/render.h"

#include "config.h"

#ifdef ENABLE_FONTS

typedef struct amphora_message_t AmphoraMessage;

enum fonts_e {
#define LOADFONT(name, path) name,
	FONTS
#undef LOADFONT
	FONTS_COUNT
};

int init_fonts(void);
void free_fonts(void);
AmphoraMessage *create_string(AmphoraMessage **msg, enum fonts_e font_name, int pt, int x, int y, AmphoraColor color, const char *text);
size_t get_string_length(const AmphoraMessage *msg);
AmphoraMessage *update_string_text(AmphoraMessage **msg, const char *text);
AmphoraMessage *update_string_n(AmphoraMessage **msg, size_t n);
void render_string(const AmphoraMessage *msg);
void free_all_strings(void);

#endif
#endif /* AMPHORA_TTF_H */
