#ifndef UNTITLED_PLATFORMER_RENDER_H
#define UNTITLED_PLATFORMER_RENDER_H

#include "SDL.h"

typedef struct vector2_t {
    Sint32 x;
    Sint32 y;
} Vector2;

typedef Vector2 Camera;

/* Get the resolution */
Vector2 get_resolution(void);
/* Get the current upper-left point of the camera */
Vector2 get_camera(void);
/* Set the location of the camera's top left point */
void set_camera(Sint32 x, Sint32 y);
/* Gets the current background color */
SDL_Color get_bg(void);
/* Sets the background color */
void set_bg(SDL_Color color);

#endif /* UNTITLED_PLATFORMER_RENDER_H */
