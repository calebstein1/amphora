#ifndef DISABLE_FONTS
#ifndef AMPHORA_TTF_H
#define AMPHORA_TTF_H

#include "SDL.h"
#include "SDL_ttf.h"

#include "engine/render.h"

#include "config.h"
#include "resources.h"

typedef struct amphora_message_t AmphoraString;

enum fonts_e {
#define LOADFONT(name, path) name##_ft,
	FONTS
#undef LOADFONT
	FONTS_COUNT
};

#ifdef __cplusplus
extern "C" {
#endif
/* Create a string */
AmphoraString *create_string(AmphoraString **msg, const char *name, int pt, int x, int y, int order, SDL_Color color, const char *text, bool stationary);
/* Get the number of characters in a message */
size_t get_string_length(const AmphoraString *msg);
/* Update the text in a string */
AmphoraString *update_string_text(AmphoraString **msg, const char *text);
/* Change the number of characters displayed in a string, 0 displays all characters */
AmphoraString *update_string_n(AmphoraString **msg, size_t n);
/* Free a string */
void free_string(AmphoraString *msg);
/* Display an AmphoraString on the screen */
void render_string(const AmphoraString *msg);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_TTF_H */
#endif
