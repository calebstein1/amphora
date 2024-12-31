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
/* Gets the pixel posiiton of the upper left corner of a sprite */
Vector2f get_sprite_position(const AmphoraImage *spr);
/* Gets the pixel position of the center of a sprite */
Vector2f get_sprite_center(const AmphoraImage *spr);
/* Get whether a sprite is flipped */
bool is_flipped(const AmphoraImage *spr);
/* Allocate a sprite slot and initialize it with the supplied values */
AmphoraImage *create_sprite(AmphoraImage **spr, const char *image_name, float x, float y, float scale,
			    bool flip, bool stationary, Sint32 order);
/* Add a frameset to a sprite */
void add_frameset(AmphoraImage *spr, const char *name, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h, float off_x, float off_y, Uint16 num_frames, Uint16 delay);
/* Set a sprite slot's frameset */
void set_frameset(AmphoraImage *spr, const char *name);
/* Play a one-shot animation, holding on the last frame and executing a callback function when finished */
void play_oneshot(AmphoraImage *spr, const char *name, void (*callback)(void));
/* Set the delay between frames of a frameset animation */
void set_frameset_delay(AmphoraImage *spr, const char *name, Uint16 delay);
/* Change the draw order of a sprite */
AmphoraImage *reorder_sprite(AmphoraImage *spr, Sint32 order);
/* Sets a sprite's absolute location */
void set_sprite_location(AmphoraImage *spr, float x, float y);
/* Move a sprite by the supplied delta values */
void move_sprite(AmphoraImage *spr, float delta_x, float delta_y);
/* Flip a sprite */
void flip_sprite(AmphoraImage *spr);
/* Unflip a sprite */
void unflip_sprite(AmphoraImage *spr);
/* Show the supplied sprite_slot if hidden */
void show_sprite(AmphoraImage *spr);
/* Hide a sprite without free it */
void hide_sprite(AmphoraImage *spr);
/* Free a sprite slot */
void free_sprite(AmphoraImage **spr);
#ifdef __cplusplus
}
#endif

#endif /* UNTITLED_PLATFORMER_IMG_H */
