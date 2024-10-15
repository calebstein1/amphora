#ifndef AMPHORA_TTF_H
#define AMPHORA_TTF_H

#include "SDL.h"
#include "SDL_ttf.h"

#include "engine/render.h"

#include "config.h"

typedef struct amphora_message_t AmphoraMessage;

enum fonts_e {
#define LOADFONT(name, path) name,
	FONTS
#undef LOADFONT
	FONTS_COUNT
};

int load_fonts(void);
void free_fonts(void);
AmphoraMessage *create_string(AmphoraMessage **amsg, enum fonts_e font_name, int pt, int x, int y, AmphoraColor color, const char *text);
void render_string(const AmphoraMessage *msg);
void free_string(AmphoraMessage **amsg);

#endif /* AMPHORA_TTF_H */
