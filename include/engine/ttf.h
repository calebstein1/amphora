#ifndef DISABLE_FONTS
#ifndef AMPHORA_TTF_H
#define AMPHORA_TTF_H

#include "SDL.h"
#include "SDL_ttf.h"

#include "engine/render.h"

#include "config.h"
#include "resources.h"

typedef struct amphora_message_t AmphoraMessage;

enum fonts_e {
#define LOADFONT(name, path) name,
	FONTS
#undef LOADFONT
	FONTS_COUNT
};

#ifdef __cplusplus
extern "C" {
#endif
/* Create a string */
AmphoraMessage *create_string(AmphoraMessage **msg, enum fonts_e font_name, int pt, int x, int y, SDL_Color color, const char *text, bool stationary);
/* Get the number of characters in a message */
size_t get_string_length(const AmphoraMessage *msg);
/* Update the text in a string */
AmphoraMessage *update_string_text(AmphoraMessage **msg, const char *text);
/* Change the number of characters displayed in a string, 0 displays all characters */
AmphoraMessage *update_string_n(AmphoraMessage **msg, size_t n);
/* Display an AmphoraMessage on the screen */
void render_string(const AmphoraMessage *msg);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_TTF_H */
#endif
