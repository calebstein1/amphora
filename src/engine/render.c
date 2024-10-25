#include "engine/internal/img.h"
#include "engine/internal/render.h"

#include "config.h"

/* File-scoped variables */
static SDL_Renderer *renderer;
static SDL_Window *window;
static Camera camera = { 0, 0 };
static SDL_Color bg = { 0, 0, 0, 0xff };
static Vector2 render_logical_size = { 0, 0 };

Vector2
get_resolution(void) {
	Sint32 rx, ry;
	SDL_GetWindowSize(window, &rx, &ry);
	return (Vector2){ rx, ry };
}

Vector2
get_render_logical_size(void) {
	return render_logical_size;
}

Vector2
get_camera(void) {
	return camera;
}

void
set_camera(Sint32 x, Sint32 y) {
	camera.x = x;
	camera.y = y;
}

void
set_camera_zoom(Uint16 factor, Uint16 delay) {
	static Uint16 current_factor = 100;
	static Vector2 *scale_steps = NULL;
	static Uint16 scale_steps_index = 0;
	static Uint16 scale_steps_count = 0;
	Vector2 current_resolution = get_resolution();
	Vector2 current_logical_size = get_render_logical_size();
	int i;
	Vector2 step_size = {
		.x = (current_logical_size.x - ((current_resolution.x * 100) / factor)) / (delay ? delay : 1),
		.y = (current_logical_size.y - ((current_resolution.y * 100) / factor)) / (delay ? delay : 1)
	};

	if (!scale_steps && delay > 0) {
		scale_steps_count = delay;
		if (factor == current_factor) return;

		current_factor = factor;
		if (!((scale_steps = SDL_malloc(delay * sizeof(Vector2))))) {
			SDL_LogWarn(SDL_LOG_CATEGORY_ERROR, "Failed to allocate scale steps\n");
			return;
		}
		for (i = 0; i < scale_steps_count; i++) {
			scale_steps[i] = (Vector2){
				.x = render_logical_size.x - (step_size.x * (i + 1)),
				.y = render_logical_size.y - (step_size.y * (i + 1))
			};
		}
	}
	if (scale_steps_index == scale_steps_count || factor != current_factor) {
		SDL_free(scale_steps);
		scale_steps = NULL;
		scale_steps_index = 0;
		scale_steps_count = 0;
#ifdef DEBUG
		SDL_Log("Finished scaling to %d, %d\n", current_logical_size.x, current_logical_size.y);
#endif
		return;
	}
	set_render_logical_size(scale_steps[scale_steps_index++]);
}

void
reset_camera_zoom(Uint16 delay) {
	set_camera_zoom(100, delay);
}

SDL_Color
get_bg (void) {
	return bg;
}

void
set_bg(SDL_Color color) {
	bg = color;
}

/*
 * Internal functions
 */

int
init_render(void) {
	if (!((window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
					 WINDOW_X, WINDOW_Y, WINDOW_MODE | SDL_WINDOW_ALLOW_HIGHDPI)))) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to create window: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to create window", SDL_GetError(), 0);
		return -1;
	}
	if (!((renderer = SDL_CreateRenderer(window, -1, 0)))) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to create renderer: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to create renderer", SDL_GetError(), 0);
		return -1;
	}
	if (init_img() == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER,"Failed to init image system\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to init image system", "Failed to initialize image system", 0);
		return -1;
	}
	set_render_logical_size(get_resolution());

	return 0;
}

void
cleanup_render(void) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void
set_render_logical_size(const Vector2 size) {
	render_logical_size = size;
	SDL_RenderSetLogicalSize(renderer, render_logical_size.x, render_logical_size.y);
}

void
clear_bg(void) {
	SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(renderer);
}

SDL_Window *
get_window(void) {
	return window;
}

SDL_Renderer *
get_renderer(void) {
	return renderer;
}

void
render_texture(SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect) {
	SDL_RenderCopy(renderer, texture, srcrect, dstrect);
}
