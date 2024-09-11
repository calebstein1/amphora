#ifndef UNTITLED_PLATFORMER_SPRITES_H
#define UNTITLED_PLATFORMER_SPRITES_H

#define MAX_SPRITES_ON_SCREEN 256
#define SPRITESHEET_SIZE 0x4000
#define SPRITESHEET_PATH "content/img/sprites.chr"
#define SPR_NUM_PIXELS 64
#define SUBPIXEL_STEPS 4 /* Used for bit shifts, not directly */

#include <SDL2/SDL.h>

#include "globals.h"
#include "palette.h"

struct sprite_slot_t {
    unsigned int spr_num;
    int spr_x;
    int spr_y;
    u8 spr_subp_x : 4;
    u8 spr_subp_y : 4;
    u8 spr_pal;
    u8 spr_prop;
    u8 reserved;
};

int init_spritesheet(void); /* Run once to load spritesheet and palette data */
void draw_all_sprites(SDL_Renderer *renderer, int p_size); /* Draw all active sprite slots */
struct sprite_slot_t *get_spriteslot(struct sprite_slot_t **spr);
void free_spriteslot(struct sprite_slot_t *spr);

#endif /* UNTITLED_PLATFORMER_SPRITES_H */
