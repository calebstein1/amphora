#ifndef IMG_INTERNAL_H
#define IMG_INTERNAL_H

#include "engine/img.h"
#include "engine/internal/render.h"

int init_img(void);
void cleanup_sprites(void);
void draw_all_sprites_and_gc(void);

#endif /* IMG_INTERNAL_H */
