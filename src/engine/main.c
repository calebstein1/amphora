#include <stdio.h>

#include "engine/events.h"
#include "engine/game_loop.h"
#include "engine/input.h"
#include "engine/render.h"
#include "engine/util.h"

#include "config.h"

int
main(void) {
	Uint64 frame_start, frame_end, frame_time, frame_count = 0;
	int win_size_x, win_size_y;

	SDL_Window *win;
	SDL_Renderer *renderer;
	SDL_Event e;
	union input_state_u key_actions;
	struct save_data_t save_data;
	Vector2 init_window_size;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failed to init SDL: %s\n", SDL_GetError());
		return -1;
	}

	win_size_x = WINDOW_X;
	win_size_y = WINDOW_Y;
	set_pixel_size(RESOLUTION_MODE ?
		MAX_OF(win_size_x, win_size_y) / RESOLUTION :
		MIN_OF(win_size_x, win_size_y) / RESOLUTION);
	win = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			       win_size_x, win_size_y, WINDOW_MODE);
	if (!win) {
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		return -1;
	}
	renderer = SDL_CreateRenderer(win, -1, 0);
	if (!renderer) {
		fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
		return -1;
	}

	SDL_GetRendererOutputSize(renderer, &init_window_size.x, &init_window_size.y);
	set_window_size(init_window_size);

	if (init_render() == -1) {
		fputs("Failed to init render data\n", stderr);
		return -1;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	game_init();

	while (1) {
		frame_start = SDL_GetTicks64();
		frame_count++;

		if (event_loop(&e, &key_actions, &win_size_x, &win_size_y, renderer) == SDL_QUIT) break;
		clear_bg(renderer);
		game_loop(frame_count, &key_actions.state, &save_data);
		draw_all_sprites_and_gc(renderer);

		SDL_RenderPresent(renderer);

		frame_end = SDL_GetTicks64();
		if ((frame_time = frame_end - frame_start) < (1000 / FRAMERATE)) {
			SDL_Delay((1000 / FRAMERATE) - frame_time);
		}
	}

	cleanup_render();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}

