#ifndef RENDER_INTERNAL_H
#define RENDER_INTERNAL_H

#include "engine/util.h"
#include "engine/render.h"

enum camera_mode_e {
	manual,
	tracking
};

enum render_list_node_type_e {
	SPRITE,
	STRING,
	MAP,
	NONE
};

struct render_list_node_t {
	enum render_list_node_type_e type;
	void *data;
	int order;
	bool garbage : 1;
	bool display : 1;
	bool stationary : 1;
	struct render_list_node_t *next;
};

int init_render(void);
void cleanup_render(void);
void set_render_logical_size(Vector2 size);
void clear_bg(void);
SDL_Window *get_window(void);
SDL_Renderer *get_renderer(void);
struct render_list_node_t *add_render_list_node(int order);
void draw_render_list_and_gc(void);
void free_render_list(void);
void update_camera(void);
void render_texture(SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect, double angle, SDL_RendererFlip flip);

#endif /* RENDER_INTERNAL_H */
