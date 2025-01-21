#ifndef AMPHORA_COLLISION_H
#define AMPHORA_COLLISION_H

#include "SDL.h"

#include "engine/render.h"
#include "engine/util.h"

typedef enum {
	AMPHORA_COLLISION_NONE,
	AMPHORA_COLLISION_LEFT,
	AMPHORA_COLLISION_RIGHT,
	AMPHORA_COLLISION_TOP,
	AMPHORA_COLLISION_BOTTOM
} AmphoraCollision;

#ifdef __cplusplus
extern "C" {
#endif
/* Returns true if two objects have collided */
bool Amphora_CheckCollision(const AmphoraImage *obj_a, const AmphoraImage *obj_b);
/* Returns the direction of collision if the object has collided with any rectangle in the object group */
AmphoraCollision Amphora_CheckObjectGroupCollision(const AmphoraImage *obj, const char *name);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_COLLISION_H */
