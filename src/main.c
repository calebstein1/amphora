#include <stdio.h>
#include <SDL2/SDL.h>

#include "gameplay.h"
#include "globals.h"
#include "screen.h"
#include "sprites.h"

#define GAME_TITLE "Untitled Platformer"

int
main(void) {
	unsigned long int frame_start, frame_end, frame_time, frame_count = 0;
	int win_size_x, win_size_y;
	unsigned short pixel_size;

	SDL_Window *win;
	SDL_Renderer *renderer;
	SDL_Event e;
	unsigned char running = 1;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failed to init SDL: %s\n", SDL_GetError());
		return -1;
	}

	win_size_x = 800;
	win_size_y = 600;
	win = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			       win_size_x, win_size_y, SDL_WINDOW_RESIZABLE);
	if (!win) {
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		return -1;
	}
	renderer = SDL_CreateRenderer(win, -1, 0);
	if (!renderer) {
		fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
		return -1;
	}

	if (init_spritesheet() == -1) {
		fputs("Failed to init spritesheet\n", stderr);
		return -1;
	}

	game_init();

	while (running) {
		frame_start = SDL_GetTicks();
		frame_count++;

		SDL_GetWindowSize(win, &win_size_x, &win_size_y);
		pixel_size = MIN_OF(win_size_x, win_size_y) >> 7;

		(void)frame_count;

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = 0;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
		SDL_RenderClear(renderer);

		game_loop(frame_count);

		draw_all_sprites(renderer, pixel_size);

		SDL_RenderPresent(renderer);

		frame_end = SDL_GetTicks();
		if ((frame_time = frame_end - frame_start) < SCREEN_TICKS_PER_FRAME) {
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_time);
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();

    return 0;
}

