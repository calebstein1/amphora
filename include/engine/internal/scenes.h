#ifndef AMPHORA_SCENES_INTERNAL_H
#define AMPHORA_SCENES_INTERNAL_H

#include "engine/internal/input.h"

typedef struct amphora_scene_t {
	void (*init_func)(void);
	void (*update_func)(Uint32, const InputState *);
	void (*destroy_func)(void);
} AmphoraScene;

void Amphora_InitSceneManager(void);
void Amphora_DeInitSceneManager(void);
void Amphora_InitScene(void);
void Amphora_UpdateScene(Uint32 frame_count);
void Amphora_DestroyScene(void);

#endif /* AMPHORA_SCENES_INTERNAL_H */
