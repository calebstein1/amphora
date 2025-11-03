/*
 * DO NOT TOUCH THIS FILE!
 *
 * The engine requires this file in this format to dynamically read game scenes
 */

#include "engine/internal/scenes.h"
#include "engine/input.h"
#include "scene_list.h"

enum scene_list_e {
#define SCENE(name) name,
	SCENES
#undef SCENE
	SCENES_COUNT
};

#define SCENE(name) extern void name##_Init(void); extern void name##_Update(Uint32, const InputState *); extern void name##_Destroy(void);
	SCENES
#undef SCENE

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
static int scenes_count = SCENES_COUNT;
static int loaded = 0;

void
Amphora_GetSceneData(AmphoraScene **scenes, char ***names, int *count)
{
	if (scenes == NULL || names == NULL || count == NULL) return;
	if (loaded == 1) return;

	*scenes = scene_structs;
	*names = scene_names;
	*count = scenes_count;
	loaded = 1;
}
