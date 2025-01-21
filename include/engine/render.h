#ifndef UNTITLED_PLATFORMER_RENDER_H
#define UNTITLED_PLATFORMER_RENDER_H

#include "SDL.h"

#include "engine/util.h"

typedef SDL_FPoint Vector2f;
typedef SDL_FPoint Camera;
typedef SDL_Point Vector2;
typedef struct sprite_t AmphoraImage;
typedef struct amphora_message_t AmphoraString;

#ifdef __cplusplus
extern "C" {
#endif
/* Get the resolution */
Vector2 Amphora_GetResolution(void);
/* Get the renderer logical size */
Vector2 Amphora_GetRenderLogicalSize(void);
/* Get the current upper-left point of the camera */
Vector2f Ampohra_GetCamera(void);
/* Set the location of the camera's top left point */
void Amphora_SetCamera(float x, float y);
/* Move the camera */
void Amphora_MoveCamera(float delta_x, float delta_y);
/* Set the sprite for the camera to follow, NULL to disable tracking */
void Amphora_SetCameraTarget(AmphoraImage *target);
/* Set the zoom factor for the camera */
void Amphora_SetCameraZoom(Uint16 factor, Uint16 delay);
/* Reset the camera zoom */
void Amphora_ResetCameraZoom(Uint16 delay);
/* Gets the current background color */
SDL_Color Amphora_GetBGColor(void);
/* Sets the background color */
void Amphora_SetBGColor(SDL_Color color);
/* Sets the game window to fullscreen */
void Amphora_SetWindowFullscreen(void);
/* Sets the game window to windowed */
void Amphora_SetWindowWindowed(void);
/* Returns true if the game window is fullscreen, false if windowed */
bool Ampohra_IsWindowFullscreen(void);
#ifdef __cplusplus
}
#endif

#endif /* UNTITLED_PLATFORMER_RENDER_H */
