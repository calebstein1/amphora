#ifndef UNTITLED_PLATFORMER_UTIL_H
#define UNTITLED_PLATFORMER_UTIL_H

#define MAX_OF(a,b) (a > b ? a : b)
#define MIN_OF(a,b) (a < b ? a : b)
#define IS_ODD(a) (a & 1)
#define IS_EVEN(a) (!IS_ODD(a))

#define shorter false
#define longer true
#define fullscreen SDL_WINDOW_FULLSCREEN_DESKTOP
#define fixed SDL_WINDOW_SHOWN
#define resizable SDL_WINDOW_RESIZABLE

#define bool SDL_bool
#define true SDL_TRUE
#define false SDL_FALSE

#ifndef BIN_LOADER
void quit_game(void); /* Request to quit the game */
SDL_Renderer *get_renderer(void); /* Return the render context */
#endif

#endif /* UNTITLED_PLATFORMER_UTIL_H */
