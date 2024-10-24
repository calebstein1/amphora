#include "engine/game_loop.h"
#include "engine/util.h"
#include "engine/internal/events.h"
#include "engine/internal/img.h"
#include "engine/internal/input.h"
#include "engine/internal/render.h"
#include "engine/internal/timer.h"
#include "engine/internal/ttf.h"

#include "config.h"

/* Globals */
Uint64 frame_count = 0;

/* File-scored variables */
static bool quit_requested = false;

int
main(int argc, char **argv) {
	Uint64 frame_start, frame_end;
	Uint32 frame_time;

	SDL_Event e;
	static union input_state_u key_actions;
	static SaveData save_data;

	/* SDL requires these but we're not actually using them */
	(void)argc;
	(void)argv;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init SDL: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to init SDL", SDL_GetError(), 0);
		return -1;
	}
	find_controllers();

	if (!IMG_Init(IMG_INIT_PNG)) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init SDL_image: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to init SDL_image", SDL_GetError(), 0);
		return -1;
	}

#ifdef ENABLE_FONTS
	if (TTF_Init() < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init SDL_ttf: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to init SDL_ttf", SDL_GetError(), 0);
		return -1;
	}
	if (init_fonts() == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER,"Failed to load TTF font data\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to load TTF font data", "Failed to load TTF font data", 0);
		return -1;
	}
#endif

	if (init_render() == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER,"Failed to init renderer\n");
		return -1;
	}

	game_init();

	while (!quit_requested) {
		frame_start = SDL_GetTicks64();
		frame_count++;

		if (event_loop(&e, &key_actions) == SDL_QUIT) quit_requested = true;
		clear_bg();
		game_loop(frame_count, &key_actions.state, &save_data);
		draw_all_sprites_and_gc();

		SDL_RenderPresent(get_renderer());

		frame_end = SDL_GetTicks64();
		if ((frame_time = (Uint32)(frame_end - frame_start)) < (1000 / FRAMERATE)) {
			SDL_Delay((1000 / FRAMERATE) - frame_time);
		}
	}

	game_shutdown();
	cleanup_sprites();
#ifdef ENABLE_FONTS
	free_all_strings();
	free_fonts();
#endif
	cleanup_render();
	cleanup_controllers();
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

	return 0;
}

void
quit_game(void) {
	quit_requested = true;
}
