#ifndef UNTITLED_PLATFORMER_RENDER_H
#define UNTITLED_PLATFORMER_RENDER_H

#include "SDL.h"

typedef SDL_Point Vector2;
typedef Vector2 Camera;
typedef struct sprite_t AmphoraImage;
typedef struct amphora_message_t AmphoraString;

#ifdef __cplusplus
extern "C" {
#endif
/* Get the resolution */
Vector2 get_resolution(void);
/* Get the renderer logical size */
Vector2 get_render_logical_size(void);
/* Get the current upper-left point of the camera */
Vector2 get_camera(void);
/* Set the location of the camera's top left point */
void set_camera(Sint32 x, Sint32 y);
/* Move the camera */
void move_camera(Sint32 x, Sint32 y);
/* Set the sprite for the camera to follow, NULL to disable tracking */
void set_camera_target(AmphoraImage *target);
/* Set the zoom factor for the camera */
void set_camera_zoom(Uint16 factor, Uint16 delay);
/* Reset the camera zoom */
void reset_camera_zoom(Uint16 delay);
/* Gets the current background color */
SDL_Color get_bg(void);
/* Sets the background color */
void set_bg(SDL_Color color);
/* Sets the game window to fullscreen */
void set_window_fullscreen(void);
/* Sets the game window to windowed */
void set_window_windowed(void);
/* Returns true if the game window is fullscreen, false if windowed */
bool is_window_fullscreen(void);
#ifdef __cplusplus
}
#endif

#endif /* UNTITLED_PLATFORMER_RENDER_H */
