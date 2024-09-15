#ifndef UNTITLED_PLATFORMER_PALETTE_H
#define UNTITLED_PLATFORMER_PALETTE_H

#include "engine/util.h"

#define MAX_USER_PALETTES 64
#define MAX_COLORS 256
#define PALETTE_SIZE 4
#define USER_PALETTES_PATH "content/palettes/user_palettes.dat"
#define PALETTE_TABLE_PATH "content/palettes/table.pal"

struct color_t {
	Uint8 r;
	Uint8 g;
	Uint8 b;
};

#endif /* UNTITLED_PLATFORMER_PALETTE_H */
