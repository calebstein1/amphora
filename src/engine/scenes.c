#include "engine/internal/error.h"
#include "engine/internal/ht_hash.h"
#include "engine/internal/img.h"
#include "engine/internal/input.h"
#include "engine/internal/mixer.h"
#include "engine/internal/render.h"
#include "engine/internal/scenes.h"

#include <engine/internal/ttf.h>

#include "engine/internal/tilemap.h"

#include "scene_list.h"
#include "engine/events.h"

#ifdef __cplusplus
extern "C"
#endif
#define SCENE(name) extern void name##_Init(void); extern void name##_Update(Uint32, const InputState *); extern void name##_Destroy(void);
	SCENES
#undef SCENE
#ifdef __cplusplus
}
#endif

/* Prototypes for private functions */
void Amphora_SceneTransitionEvent(void);

/* File-scoped variables */
static HT_HashTable scenes;
static AmphoraScene scene_structs[] = {
#define SCENE(name) { .init_func = name##_Init, .update_func = name##_Update, .destroy_func = name##_Destroy },
	SCENES
#undef SCENE
};
static char *scene_names[] = {
#define SCENE(name) #name,
	SCENES
#undef SCENE
};
static long current_scene_idx = 0;
static int current_scene_name = 0;
static Uint16 fade_timer = 0;
static Uint16 fade_frames = 0;
static int fade_idx = 0;
static int fade_idx_mod = 1;
static Uint8 *fade_steps;
static SDL_Color fade_color = { 255, 255, 255, 255 };
static SDL_Rect fade_rect;
static bool scene_loaded = false;

int
Amphora_DoLoadScene(const char *name) {
	Vector2 screen_size = Amphora_GetResolution();
	int i;

	for (i = 0; i < SCENES_COUNT; i++) {
		if (SDL_strcmp(name, scene_names[i]) == 0) {
			current_scene_name = i;
			break;
		}
	}
	if (i == SCENES_COUNT) {
		Amphora_SetError(AMPHORA_STATUS_FAIL_UNDEFINED, "No scene %s", name);
		return AMPHORA_STATUS_FAIL_UNDEFINED;
	}
	fade_rect.w = screen_size.x;
	fade_rect.h = screen_size.y;
	fade_frames = fade_timer / Amphora_GetFPS();
	if (!((fade_steps = SDL_malloc((fade_frames >> 1) * sizeof(Uint8))))) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to allocate memory for fade steps\n");
		return AMPHORA_STATUS_ALLOC_FAIL;
	}
	for (i = 0; i < (fade_frames >> 1); i++) {
		fade_steps[i] = i * 255 / ((fade_frames >> 1) - 1);
	}
	fade_idx = 0;
	fade_idx_mod = 1;
	Amphora_RegisterEvent("amph_internal_transition", Amphora_SceneTransitionEvent);

	return AMPHORA_STATUS_OK;
}

int
Amphora_SetSceneFadeParameters(Uint16 ms, SDL_Color color) {
	fade_timer = ms;
	fade_color = color;

	return AMPHORA_STATUS_OK;
}

/*
 * Internal functions
 */

void
Amphora_InitSceneManager(void) {
	int i;

	scenes = HT_NewTable();
	for (i = 0; i < SCENES_COUNT; i++) {
		HT_SetValue(scene_names[i], i, scenes);
	}
}

void
Amphora_DeInitSceneManager(void) {
	HT_FreeTable(scenes);
}

bool
Amphora_IsSceneLoaded(void) {
	return scene_loaded;
}

void
Amphora_InitScene(void) {
	scene_structs[current_scene_idx].init_func();
	scene_loaded = true;
}

void
Amphora_UpdateScene(Uint32 frame_count) {
	scene_structs[current_scene_idx].update_func(frame_count, Amphora_GetKeyActionState());
}

void
Amphora_DestroyScene(void) {
	scene_structs[current_scene_idx].destroy_func();
#ifndef DISABLE_TILEMAP
	Amphora_DestroyCurrentMap();
	Amphora_FreeObjectGroup();
#endif
#ifndef DISABLE_MIXER
	Amphora_FreeAllSFX();
#endif
	Amphora_ResetRenderList();
	Amphora_UnboundCamera();
	Amphora_FreeAllIMG();
#ifndef DISABLE_FONTS
	Amphora_FreeAllFonts();
#endif
	scene_loaded = false;
}

/*
 * Private functions
 */

void
Amphora_SceneTransitionEvent(void) {
	SDL_Renderer *renderer = Amphora_GetRenderer();

	if (!fade_timer) {
		Amphora_DestroyScene();
		current_scene_idx = HT_GetValue(scene_names[current_scene_name], scenes);
		Amphora_InitScene();
		Amphora_UnregisterEvent("amph_internal_transition");
		return;
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, fade_color.r, fade_color.g, fade_color.b, fade_steps[fade_idx]);
	SDL_RenderFillRect(renderer, &fade_rect);
	fade_idx += fade_idx_mod;
	if (fade_idx == (fade_frames >> 1) - 1) {
		fade_idx_mod = -1;
		Amphora_DestroyScene();
		current_scene_idx = HT_GetValue(scene_names[current_scene_name], scenes);
		Amphora_InitScene();
	}
	if (fade_idx == 0 && fade_idx_mod == -1) {
		SDL_free(fade_steps);
		Amphora_UnregisterEvent("amph_internal_transition");
	}
}
