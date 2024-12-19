#include "engine/game_loop.h"
#include "engine/util.h"
#include "engine/internal/db.h"
#include "engine/internal/events.h"
#include "engine/internal/img.h"
#include "engine/internal/input.h"
#include "engine/internal/mixer.h"
#include "engine/internal/prefs.h"
#include "engine/internal/render.h"
#include "engine/internal/save_data.h"
#include "engine/internal/tilemap.h"
#include "engine/internal/timer.h"
#include "engine/internal/ttf.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/* Prototypes for private functions */
int main_loop(SDL_Event *e);
void save_config(void);
void clean_resources(void);

/* Globals */
Uint64 frame_count = 0;

/* File-scored variables */
static Uint32 framerate;
static bool quit_requested = false;

int
main(int argc, char **argv) {
	SDL_Event e;

	/* SDL requires these but we're not actually using them */
	(void)argc;
	(void)argv;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init SDL: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to init SDL", SDL_GetError(), 0);
		return -1;
	}

	if (IMG_Init(IMG_INIT_PNG) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init SDL_image: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to init SDL_image", SDL_GetError(), 0);
		return -1;
	}

#ifndef DISABLE_MIXER
	if (Mix_Init(MIX_INIT_OGG) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init SDL_mixer: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to init SDL_mixer", SDL_GetError(), 0);
		return -1;
	}
	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open audio device: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to open audio device", SDL_GetError(), 0);
	}
	if (init_sfx() == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Failed to load sfx data\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to load sfx data", "Failed to load sfx data", 0);
		return -1;
	}
	if (init_music() == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Failed to load music data\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to load music data", "Failed to load sfx data", 0);
		return -1;
	}
#endif
#ifndef DISABLE_FONTS
	if (TTF_Init() < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init SDL_ttf: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to init SDL_ttf", SDL_GetError(), 0);
		return -1;
	}
	if (init_fonts() == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Failed to load TTF font data\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to load TTF font data", "Failed to load TTF font data", 0);
		return -1;
	}
#endif
#ifndef DISABLE_TILEMAP
	if (init_maps() == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Failed to load tilemap data\n");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to load tilemap data", "Failed to load tilemap data", 0);
		return -1;
	}
#endif
	init_db();
	init_config();
	if (init_render() == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Failed to init renderer\n");
		return -1;
	}
	init_save();
	init_input();
	load_keymap();

	framerate = (Uint32)load_framerate();

	game_init();

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
#else
	while (main_loop(&e) == 0) {}
#endif
	save_config();
	clean_resources();
	IMG_Quit();
#ifndef DISABLE_MIXER
	Mix_CloseAudio();
	Mix_Quit();
#endif
#ifndef DISABLE_FONTS
	TTF_Quit();
#endif
	SDL_Quit();

	return 0;
}

void
quit_game(void) {
	quit_requested = true;
}

/*
 * Private functions
 */

int
main_loop(SDL_Event *e) {
	static Uint64 frame_start, frame_end;
	static Uint32 frame_time;

	frame_start = SDL_GetTicks64();
	frame_count++;

	if (event_loop(e) == SDL_QUIT) quit_requested = true;
	if (quit_requested) {
#ifdef __EMSCRIPTEN__
		save_config();
		clean_resources();
		IMG_Quit();
#ifndef DISABLE_MIXER
		Mix_CloseAudio();
		Mix_Quit();
#endif
#ifndef DISABLE_FONTS
		TTF_Quit();
#endif
		SDL_Quit();
		emscripten_cancel_main_loop();
#else
		return 1;
#endif
	}
	clear_bg();
	game_loop(frame_count, get_key_actions_state());
	draw_render_list_and_gc();
	update_camera();

	SDL_RenderPresent(get_renderer());

	frame_end = SDL_GetTicks64();
	if ((frame_time = (Uint32)(frame_end - frame_start)) < (1000 / framerate)) {
		SDL_Delay((1000 / framerate) - frame_time);
#ifdef DEBUG
	} else if (frame_time > (1000 / framerate)) {
		SDL_Log("Lag on frame %llu (frame took %llu ticks, %d ticks per frame)\n", frame_count, frame_end - frame_start, 1000 / FRAMERATE);
	}
#else
	}
#endif

	return 0;
}

void
save_config(void) {
	Vector2 win_size = get_resolution();
	Uint32 win_flags = SDL_GetWindowFlags(get_window());

	save_window_x(win_size.x);
	save_window_y(win_size.y);
	save_win_flags(win_flags);
	save_framerate(framerate);
}

void
clean_resources(void) {
	game_shutdown();
	cleanup_img();
#ifndef DISABLE_FONTS
	free_fonts();
#endif
#ifndef DISABLE_TILEMAP
	destroy_current_map();
#endif
#ifndef DISABLE_MIXER
	cleanup_sfx();
	cleanup_music();
#endif
	free_render_list();
	cleanup_render();
	cleanup_controllers();
	cleanup_db();
}
