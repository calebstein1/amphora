#ifndef AMPHORA_COLLISION_H
#define AMPHORA_COLLISION_H

#include "SDL.h"

#include "engine/render.h"
#include "engine/util.h"

typedef struct amphora_collidable_t IAmphoraCollidable;

typedef enum {
	AMPHORA_COLLISION_NONE,
	AMPHORA_COLLISION_LEFT,
	AMPHORA_COLLISION_RIGHT,
	AMPHORA_COLLISION_TOP,
	AMPHORA_COLLISION_BOTTOM
} AmphoraCollision;

#define Amphora_CheckCollision(obj_a, obj_b) Amphora_CheckCollisionInterface((IAmphoraCollidable *)(obj_a), (IAmphoraCollidable *)(obj_b))
#define Amphora_CheckObjectGroupCollision(obj, name) Amphora_CheckObjectGroupCollisionInterface((IAmphoraCollidable *)(obj), (name))

#ifdef __cplusplus
extern "C" {
#endif
/* Returns true if two objects have collided */
bool Amphora_CheckCollisionInterface(const IAmphoraCollidable *obj_a, const IAmphoraCollidable *obj_b);
/* Returns the direction of collision if the object has collided with any rectangle in the object group */
AmphoraCollision Amphora_CheckObjectGroupCollisionInterface(const IAmphoraCollidable *obj, const char *name);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_COLLISION_H */
