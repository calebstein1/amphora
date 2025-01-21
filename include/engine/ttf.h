#ifndef DISABLE_FONTS
#ifndef AMPHORA_TTF_H
#define AMPHORA_TTF_H

#include "SDL.h"
#include "SDL_ttf.h"

#include "engine/render.h"

#include "config.h"
#include "resources.h"

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
AmphoraString *Amphora_CreateString(AmphoraString **msg, const char *name, int pt, float x, float y, int order, SDL_Color color, const char *text, bool stationary);
/* Get the number of characters in a message */
size_t Amphora_GetStringLength(const AmphoraString *msg);
/* Get the text of an AmphoraString */
const char *Ampohra_GetStringText(AmphoraString *msg);
/* Update the text in a string */
AmphoraString *Amphora_UpdateStringText(AmphoraString **msg, const char *text);
/* Change the number of characters displayed in a string, 0 displays all characters */
AmphoraString *Amphora_UpdateStringCharsDisplayed(AmphoraString **msg, size_t n);
/* Free a string */
void Amphora_FreeString(AmphoraString **msg);
/* Display an AmphoraString on the screen */
void Amphora_RenderString(const AmphoraString *msg);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_TTF_H */
#endif
