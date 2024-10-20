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

/* One-time tasks to load fonts and initialize the font system */
int init_fonts(void);
/* Close all fonts in preperation for exit */
void free_fonts(void);
/* Create a string that will move with the camera, negative x and y values start from the right and bottom edges of the screen */
AmphoraMessage *create_string(AmphoraMessage **msg, enum fonts_e font_name, int pt, int x, int y, SDL_Color color, const char *text);
/* Create a string that is fixed to an absolute coordinate */
AmphoraMessage *create_stationary_string(AmphoraMessage **msg, enum fonts_e font_name, int pt, int x, int y, SDL_Color color, const char *text);
/* Get the number of characters in a message */
size_t get_string_length(const AmphoraMessage *msg);
/* Update the text in a string */
AmphoraMessage *update_string_text(AmphoraMessage **msg, const char *text);
/* Change the number of characters displayed in a string, 0 displays all characters */
AmphoraMessage *update_string_n(AmphoraMessage **msg, size_t n);
/* Display an AmphoraMessage on the screen */
void render_string(const AmphoraMessage *msg);
/* Free all created AmphoraMessage instances, this is called automatically at exit */
void free_all_strings(void);

#endif
#endif /* AMPHORA_TTF_H */
