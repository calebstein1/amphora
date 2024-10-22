#ifndef RENDER_INTERNAL_H
#define RENDER_INTERNAL_H

#include "engine/render.h"

int init_render(void);
void cleanup_render(void);
SDL_Window *get_window(void);
SDL_Renderer *get_renderer(void);

#endif /* RENDER_INTERNAL_H */
