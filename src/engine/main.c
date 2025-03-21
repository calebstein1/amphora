#include "engine/util.h"
#include "engine/internal/db.h"
#include "engine/internal/error.h"
#include "engine/internal/events.h"
#include "engine/internal/img.h"
#include "engine/internal/input.h"
#include "engine/internal/mixer.h"
#include "engine/internal/prefs.h"
#include "engine/internal/random.h"
#include "engine/internal/render.h"
#include "engine/internal/save_data.h"
#include "engine/internal/scenes.h"
#include "engine/internal/session_data.h"
#include "engine/internal/tilemap.h"
#include "engine/internal/ttf.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/* Prototypes for private functions */
static int Amphora_MainLoop(SDL_Event *e);
static void Amphora_SaveConfig(void);
static void Amphora_CleanResources(void);

/* File-scored variables */
static Uint32 frame_count = 0;
static Uint32 framerate;
static bool quit_requested = false;

int
main(int argc, char **argv) {
	SDL_Event e;

	/* SDL requires these but we're not actually using them */
	(void)argc;
	(void)argv;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
		Amphora_SetError(AMPHORA_STATUS_CORE_FAIL, "Failed to init SDL: %s", SDL_GetError());
		return AMPHORA_STATUS_CORE_FAIL;
	}

	if (IMG_Init(IMG_INIT_PNG) < 0) {
		Amphora_SetError(AMPHORA_STATUS_CORE_FAIL, "Failed to init SDL_image: %s", SDL_GetError());
		return AMPHORA_STATUS_CORE_FAIL;
	}

#ifndef DISABLE_MIXER
	if (Mix_Init(MIX_INIT_OGG) < 0) {
		Amphora_SetError(AMPHORA_STATUS_CORE_FAIL, "Failed to init SDL_mixer: %s", SDL_GetError());
		return AMPHORA_STATUS_CORE_FAIL;
	}
	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0) {
		Amphora_SetError(AMPHORA_STATUS_CORE_FAIL, "Failed to open audio device: %s", SDL_GetError());
		return AMPHORA_STATUS_CORE_FAIL;
	}
	if (Amphora_InitSFX() == -1) {
		Amphora_SetError(AMPHORA_STATUS_CORE_FAIL, "Failed to load sfx data");
		return AMPHORA_STATUS_CORE_FAIL;
	}
	if (Amphora_InitMusic() == -1) {
		Amphora_SetError(AMPHORA_STATUS_CORE_FAIL, "Failed to load music data");
		return AMPHORA_STATUS_CORE_FAIL;
	}
#endif
#ifndef DISABLE_FONTS
	if (TTF_Init() < 0) {
		Amphora_SetError(AMPHORA_STATUS_CORE_FAIL, "Failed to init SDL_ttf: %s", SDL_GetError());
		return AMPHORA_STATUS_CORE_FAIL;
	}
	if (Amphora_InitFonts() == -1) {
		Amphora_SetError(AMPHORA_STATUS_CORE_FAIL,"Failed to load TTF font data");
		return AMPHORA_STATUS_CORE_FAIL;
	}
#endif
#ifndef DISABLE_TILEMAP
	if (Amphora_InitMaps() == -1) {
		Amphora_SetError(AMPHORA_STATUS_CORE_FAIL,"Failed to load tilemap data");
		return AMPHORA_STATUS_CORE_FAIL;
	}
#endif
	Amphora_InitRand();
	Amphora_InitDB();
	Amphora_InitConfig();
	if (Amphora_InitRender() == -1) {
		Amphora_SetError(AMPHORA_STATUS_CORE_FAIL,"Failed to init renderer");
		return AMPHORA_STATUS_CORE_FAIL;
	}
	Amphora_InitSave();
	Amphora_InitSessionData();
	Amphora_InitInput();
	Amphora_LoadKeymap();
	Amphora_InitSceneManager();

	framerate = (Uint32) Amphora_LoadFPS();

	Amphora_InitScene();

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
#else
	while (Amphora_MainLoop(&e) == 0) {}
#endif
	Amphora_SaveConfig();
	Amphora_CleanResources();
	IMG_Quit();
#ifndef DISABLE_MIXER
	Mix_CloseAudio();
	Mix_Quit();
#endif
#ifndef DISABLE_FONTS
	TTF_Quit();
#endif
	SDL_Quit();

	return AMPHORA_STATUS_OK;
}

void
Amphora_QuitGame(void) {
	quit_requested = true;
}

Uint32
Amphora_GetFPS(void) {
	return framerate;
}

/*
 * Private functions
 */

static int
Amphora_MainLoop(SDL_Event *e) {
	static Uint32 frame_start, frame_end;
	static Uint32 frame_time;

	frame_start = SDL_GetTicks();
	frame_count++;

	if (Amphora_ProcessEventLoop(e) == SDL_QUIT) quit_requested = true;
	if (quit_requested) {
#ifdef __EMSCRIPTEN__
		Amphora_SaveConfig();
		Amphora_CleanResources();
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
	Amphora_ClearBG();
	Amphora_UpdateScene(frame_count);
	Amphora_ProcessDeferredTransition();
	Amphora_ProcessRenderList();
	Amphora_UpdateCamera();

	SDL_RenderPresent(Amphora_GetRenderer());

	frame_end = SDL_GetTicks64();
	if ((frame_time = (frame_end - frame_start)) < (1000 / framerate)) {
		SDL_Delay((1000 / framerate) - frame_time);
#ifdef DEBUG
	} else if (frame_time > (1000 / framerate)) {
		SDL_Log("Lag on frame %u (frame took %u ticks, %d ticks per frame)\n", frame_count, frame_end - frame_start, 1000 /
			Amphora_GetFPS());
	}
#else
	}
#endif

	return 0;
}

static void
Amphora_SaveConfig(void) {
	Vector2 win_size = Amphora_GetResolution();
	Uint32 win_flags = SDL_GetWindowFlags(Amphora_GetWindow());

	if (!Ampohra_IsWindowFullscreen()) {
		Amphora_SaveWinX(win_size.x);
		Amphora_SaveWinY(win_size.y);
	}
	Amphora_SaveWinFlags(win_flags);
	Amphora_SaveFPS(framerate);
}

static void
Amphora_CleanResources(void) {
	Amphora_DestroyScene();
	Amphora_DeInitSceneManager();
	Amphora_CloseIMG();
#ifndef DISABLE_FONTS
	Amphora_CloseFonts();
#endif
#ifndef DISABLE_MIXER
	Amphora_CloseSFX();
	Amphora_CloseMusic();
#endif
#ifndef DISABLE_TILEMAP
	Amphora_FreeAllObjectGroups();
	Amphora_CloseMapHashTables();
#endif
	Amphora_DeInitSessionData();
	Amphora_CloseRender();
	Amphora_ReleaseControllers();
	Amphora_CloseDB();
}
