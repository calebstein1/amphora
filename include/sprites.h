#ifndef UNTITLED_PLATFORMER_SPRITES_H
#define UNTITLED_PLATFORMER_SPRITES_H

#define MAX_SPRITES 32
#define SPRITESHEET_SIZE 0x4000
#define SPRITESHEET_PATH "content/img/sprites.chr"
#define SPR_NUM_PIXELS 64
#define SUBPIXEL_STEPS 4 /* Used for bit shifts, not directly */

#include <SDL2/SDL.h>

#include "palette.h"

struct sprite_slot_t {
    unsigned int spr_num;
    int spr_x;
    int spr_y;
    unsigned short spr_subp_x;
    unsigned short spr_subp_y;
    unsigned char spr_pal;
    unsigned char spr_prop;
};

int init_spritesheet(void); /* Run once to load spritesheet and palette data */
void draw_all_sprites(SDL_Renderer *renderer, int p_size); /* Draw all active sprite slots */

#endif /* UNTITLED_PLATFORMER_SPRITES_H */
