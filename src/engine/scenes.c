#include "engine/internal/error.h"
#include "engine/internal/ht_hash.h"
#include "engine/internal/input.h"
#include "engine/internal/mixer.h"
#include "engine/internal/render.h"
#include "engine/internal/scenes.h"
#include "engine/internal/tilemap.h"

#include "scene_list.h"

#ifdef __cplusplus
extern "C"
#endif
#define SCENE(name) extern void name##_Init(void); extern void name##_Update(Uint32, const InputState *); extern void name##_Destroy(void);
	SCENES
#undef SCENE
#ifdef __cplusplus
}
#endif

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

int
Amphora_DoLoadScene(const char *name) {
	Amphora_DestroyScene();
	current_scene_idx = HT_GetValue(name, scenes);
	Amphora_InitScene();

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

void
Amphora_InitScene(void) {
	scene_structs[current_scene_idx].init_func();
}

void
Amphora_UpdateScene(Uint32 frame_count) {
	scene_structs[current_scene_idx].update_func(frame_count, Amphora_GetKeyActionState());
}

void
Amphora_DestroyScene(void) {
#ifndef DISABLE_MIXER
	Amphora_StopMusic();
#endif
	scene_structs[current_scene_idx].destroy_func();
#ifndef DISABLE_TILEMAP
	Amphora_DestroyCurrentMap();
	Amphora_FreeObjectGroup();
#endif
	Amphora_FreeRenderList();
}
