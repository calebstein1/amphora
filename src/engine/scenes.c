#include "engine/internal/error.h"
#include "engine/internal/input.h"
#include "engine/internal/mixer.h"
#include "engine/internal/render.h"
#include "engine/internal/scenes.h"
#include "engine/internal/tilemap.h"
#include "engine/internal/timer.h"

#include "config.h"

#include "scene_list.h"

#ifdef __cplusplus
extern "C"
#endif
#define SCENE(name) extern void name##_Init(void); extern void name##_Update(Uint64, InputState *); extern void name##_Destroy(void);
	SCENES
#undef SCENE
#ifdef __cplusplus
}
#endif

/* Prototypes for private functions */
int Amphora_FindScene(const char *name);

/* File-scoped variables */
static AmphoraScene scenes[] = {
#define SCENE(name) { .init_func = name##_Init, .update_func = name##_Update, .destroy_func = name##_Destroy },
	SCENES
#undef SCENE
};
static char *scene_names[] = {
#define SCENE(name) #name,
	SCENES
#undef SCENE
};
int current_scene = 0;

int
Amphora_DoLoadScene(const char *name) {
	int idx = Amphora_FindScene(name);

	if (idx == -1) {
		Amphora_SetError(AMPHORA_STATUS_FAIL_UNDEFINED, "Failed to find scene %s", name);

		return AMPHORA_STATUS_FAIL_UNDEFINED;
	}
	Amphora_DestroyScene();
	current_scene = idx;
	Amphora_InitScene();

	return AMPHORA_STATUS_OK;
}

/*
 * Internal functions
 */

void
Amphora_InitScene(void) {
	scenes[current_scene].init_func();
}

void
Amphora_UpdateScene(void) {
	scenes[current_scene].update_func(frame_count, Amphora_GetKeyActionState());
}

void
Amphora_DestroyScene(void) {
#ifndef DISABLE_MIXER
	Amphora_StopMusic();
#endif
	scenes[current_scene].destroy_func();
#ifndef DISABLE_TILEMAP
	Amphora_DestroyCurrentMap();
	Amphora_FreeObjectGroup();
#endif
	Amphora_FreeRenderList();
}

/*
 * Private functions
 */

int
Amphora_FindScene(const char *name) {
	int i;

	for (i = 0; i < SCENES_COUNT; i++) {
		if (SDL_strcmp(name, scene_names[i]) == 0) return i;
	}

	return -1;
}
