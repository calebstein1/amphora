#ifndef UNTITLED_PLATFORMER_IMG_H
#define UNTITLED_PLATFORMER_IMG_H

#include "SDL.h"
#include "SDL_image.h"

#include "engine/util.h"
#include "engine/render.h"

#include "config.h"

typedef enum images_e {
#define LOADIMG(name, path) name,
	IMAGES
#undef LOADIMG
	IMAGES_COUNT
} ImageName;

typedef struct sprite_slot_t AmphoraImage;

/* Gets the pixel position of the center of a sprite */
Vector2 get_sprite_center(const AmphoraImage *spr);
/* Allocate a sprite slot and initialize it with the supplied values */
AmphoraImage *init_sprite_slot(AmphoraImage **spr, ImageName name, Sint32 x, Sint32 y, bool flip, Sint32 order);
/* Add a frameset to a sprite */
void add_frameset(AmphoraImage *spr, const char *name, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h, Uint16 num_frames, Uint16 delay);
/* Set a sprite slot's frameset */
void set_frameset(AmphoraImage *spr, const char *name);
/* Move a sprite by the supplied delta values */
void move_sprite(AmphoraImage *spr, Sint32 delta_x, Sint32 delta_y);
/* Show the supplied sprite_slot if hidden */
void show_sprite(AmphoraImage *spr);
/* Hide a sprite without free it */
void hide_sprite(AmphoraImage *spr);
/* Free a sprite slot */
void *release_sprite_slot(AmphoraImage **spr);

#endif /* UNTITLED_PLATFORMER_IMG_H */
