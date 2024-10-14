#ifndef AMPHORA_TTF_H
#define AMPHORA_TTF_H

#include "SDL.h"
#include "SDL_ttf.h"

#include "config.h"

enum fonts_e {
#define LOADFONT(name, path) name,
	FONTS
#undef LOADFONT
	FONTS_COUNT
};

int load_fonts(void);

#endif /* AMPHORA_TTF_H */
