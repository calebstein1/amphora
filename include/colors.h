#ifndef AMPHORA_COLORS_H
#define AMPHORA_COLORS_H

#define COLORS					\
	/* COLOR(name, red, green, blue) */	\
	COLOR(black, 0x00, 0x00, 0x00)		\
	COLOR(white, 0xff, 0xff, 0xff)		\
	COLOR(red, 0xff, 0x00, 0x00)		\
	COLOR(green, 0x00, 0xff, 0x00)		\
	COLOR(blue, 0x00, 0x00, 0xff)		\
	COLOR(sky, 0x87, 0xce, 0xeb)

#define COLOR(name, r, g, b) SDL_Color name = { r, g, b, 0xff };
COLORS
#undef COLOR

#endif /* AMPHORA_COLORS_H */
