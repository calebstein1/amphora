#ifndef UNTITLED_PLATFORMER_SPRITES_H
#define UNTITLED_PLATFORMER_SPRITES_H

#define MAX_SPRITES_ON_SCREEN 256
#define SPRITESHEET_SIZE 0x4000
#define SPRITESHEET_PATH "content/img/sprites.chr"
#define SPR_NUM_PIXELS 64
#define SUBPIXEL_STEPS 4 /* Used for bit shifts, not directly */

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "engine/palette.h"
#include "engine/util.h"

struct sprite_slot_t {
    unsigned int num; /* The sprite to draw from the spritesheet */
    int x; /* The sprite's x position */
    int y; /* The sprite's y position */
    u8 x_subp : 4; /* The sprite's x subpixel position */
    u8 y_subp : 4; /* The sprite's y subpixel position */
    u8 pal; /* The sprite's color palette */
    bool flip : 1; /* Whether or not the sprite should be flipped horizontally */
    bool reserved : 1; /* Whether the sprite slot is reserved or free */
    bool display : 1; /* Whether the sprite should be drawn or not */
};

int init_spritesheet(void); /* Run once to load spritesheet and palette data */
void draw_all_sprites(SDL_Renderer *renderer, int p_size); /* Draw all active sprite slots */
struct sprite_slot_t *reserve_sprite_slot(struct sprite_slot_t **spr);
void *release_sprite_slot(struct sprite_slot_t **spr);

#endif /* UNTITLED_PLATFORMER_SPRITES_H */
