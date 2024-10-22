#ifndef RENDER_INTERNAL_H
#define RENDER_INTERNAL_H

#include "engine/render.h"

int init_render(void);
void cleanup_render(void);
void clear_bg(void);
SDL_Window *get_window(void);
SDL_Renderer *get_renderer(void);
void render_texture(SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect);

#endif /* RENDER_INTERNAL_H */
