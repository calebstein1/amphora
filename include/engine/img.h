#ifndef UNTITLED_PLATFORMER_IMG_H
#define UNTITLED_PLATFORMER_IMG_H

#include "SDL.h"
#include "SDL_image.h"

#include "engine/util.h"
#include "engine/render.h"

#include "resources.h"

typedef enum images_e {
#define LOADIMG(name, path) name##_im,
	IMAGES
#undef LOADIMG
	IMAGES_COUNT
} ImageName;

#ifdef __cplusplus
extern "C" {
#endif
/* Gets the pixel position of the upper left corner of a sprite */
Vector2f Amphora_GetSpritePosition(const AmphoraImage *spr);
/* Gets the pixel position of the center of a sprite */
Vector2f Amphora_GetSpriteCenter(const AmphoraImage *spr);
/* Get whether a sprite is flipped */
bool Amphora_IsSpriteFlipped(const AmphoraImage *spr);
/* Allocate a sprite slot and initialize it with the supplied values */
AmphoraImage *Amphora_CreateSprite(const char *image_name, float x, float y, float scale,
				   bool flip, bool stationary, Sint32 order);
/* Add a frameset to a sprite */
int Amphora_AddFrameset(AmphoraImage *spr, const char *name, const char *override_img, Sint32 sx, Sint32 sy,
			 Sint32 w, Sint32 h, float off_x, float off_y, Uint16 num_frames, Uint16 delay);
/* Set a sprite slot's frameset */
int Amphora_SetFrameset(AmphoraImage *spr, const char *name);
/* Play a one-shot animation, holding on the last frame and executing a callback function when finished */
int Amphora_PlayOneshot(AmphoraImage *spr, const char *name, void (*callback)(void));
/* Set the delay between frames of a frameset animation */
int Amphora_SetFramesetAnimationTime(AmphoraImage *spr, const char *name, Uint16 delay);
/* Change the draw order of a sprite */
AmphoraImage *Amphora_ReorderSprite(AmphoraImage *spr, Sint32 order);
/* Sets a sprite's absolute location */
int Amphora_SetSpriteLocation(AmphoraImage *spr, float x, float y);
/* Move a sprite by the supplied delta values */
int Amphora_MoveSprite(AmphoraImage *spr, float delta_x, float delta_y);
/* Flip a sprite */
int Amphora_FlipSprite(AmphoraImage *spr);
/* Unflip a sprite */
int Amphora_UnflipSprite(AmphoraImage *spr);
/* Show the supplied sprite_slot if hidden */
int Amphora_ShowSprite(AmphoraImage *spr);
/* Hide a sprite without free it */
int Amphora_HideSprite(AmphoraImage *spr);
/* Free a sprite slot */
int Amphora_FreeSprite(AmphoraImage **spr);
#ifdef __cplusplus
}
#endif

#endif /* UNTITLED_PLATFORMER_IMG_H */
