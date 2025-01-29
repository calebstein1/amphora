#ifndef AMPHORA_SCENES_INTERNAL_H
#define AMPHORA_SCENES_INTERNAL_H

#include "scenes_list.h"

typedef struct amphora_scene_t {
	void (*init_func)(void);
	void (*update_func)(Uint64, InputState *);
	void (*destroy_func)(void);
} AmphoraScene;

enum scene_list_e {
#define SCENE(name) name,
	SCENES
#undef SCENE
	SCENES_COUNT
};

void Amphora_InitScene(void);
void Amphora_UpdateScene(void);
void Amphora_DestroyScene(void);

#endif /* AMPHORA_SCENES_INTERNAL_H */
