#include "engine/events.h"
#include "engine/game_loop.h"
#include "engine/input.h"
#include "engine/render.h"
#include "engine/util.h"

#include "config.h"

/* File-scored variables */
static SDL_bool quit_requested = false;

int
main(int argc, char **argv) {
	Uint64 frame_start, frame_end, frame_time, frame_count = 0;
	int win_size_x, win_size_y;

	SDL_Window *win;
	SDL_Renderer *renderer;
	SDL_Event e;
	static union input_state_u key_actions;
	static struct save_data_t save_data;
	Vector2 init_window_size;

	/* SDL requires these but we're not actually using them */
	(void)argc;
	(void)argv;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init SDL: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to init SDL", SDL_GetError(), 0);
		return -1;
	}

	win_size_x = WINDOW_X;
	win_size_y = WINDOW_Y;
	set_pixel_size(RESOLUTION_MODE ?
		MAX_OF(win_size_x, win_size_y) / RESOLUTION :
		MIN_OF(win_size_x, win_size_y) / RESOLUTION);
	if (!(win = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				     win_size_x, win_size_y, WINDOW_MODE))) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to create window: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to create window", SDL_GetError(), 0);
		return -1;
	}
	if (!(renderer = SDL_CreateRenderer(win, 1, 0))) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to create renderer: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to create renderer", SDL_GetError(), 0);
		return -1;
	}

	SDL_GetRendererOutputSize(renderer, &init_window_size.x, &init_window_size.y);
	set_window_size(init_window_size);

	if (init_render() == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER,"Failed to init render data\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to init render data", "Failed to initialize sprite slots", 0);
		return -1;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	game_init();

	while (!quit_requested) {
		frame_start = SDL_GetTicks64();
		frame_count++;

		if (event_loop(&e, &key_actions, &win_size_x, &win_size_y, renderer) == SDL_QUIT) quit_requested = true;
		clear_bg(renderer);
		game_loop(frame_count, &key_actions.state, &save_data);
		draw_all_sprites_and_gc(renderer);

		SDL_RenderPresent(renderer);

		frame_end = SDL_GetTicks64();
		if ((frame_time = frame_end - frame_start) < (1000 / FRAMERATE)) {
			SDL_Delay((1000 / FRAMERATE) - frame_time);
		}
	}

	game_shutdown();
	cleanup_render();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}

void
quit_game(void) {
	quit_requested = true;
}
