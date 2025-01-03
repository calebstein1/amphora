#ifndef UNTITLED_PLATFORMER_UTIL_H
#define UNTITLED_PLATFORMER_UTIL_H

#include "SDL.h"

#define MAX_OF(a,b) (a > b ? a : b)
#define MIN_OF(a,b) (a < b ? a : b)
#define IS_ODD(a) (a & 1)
#define IS_EVEN(a) (!IS_ODD(a))

#define fullscreen SDL_WINDOW_FULLSCREEN_DESKTOP
#define fixed SDL_WINDOW_SHOWN
#define resizable SDL_WINDOW_RESIZABLE

#ifndef __cplusplus
#ifndef bool
#define bool SDL_bool
#endif
#ifndef true
#define true SDL_TRUE
#endif
#ifndef false
#define false SDL_FALSE
#endif
#endif

#ifndef BIN_LOADER
#ifdef __cplusplus
extern "C" {
#endif
void quit_game(void); /* Request to quit the game */
Uint32 get_framerate(void); /* Get the current running framerate */
#ifdef __cplusplus
}
#endif
#endif

#endif /* UNTITLED_PLATFORMER_UTIL_H */
