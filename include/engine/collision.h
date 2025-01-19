#ifndef AMPHORA_COLLISION_H
#define AMPHORA_COLLISION_H

#include "SDL.h"

#include "engine/render.h"
#include "engine/util.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Returns true if two objects have collided */
bool check_collision(const AmphoraImage *obj_a, const AmphoraImage *obj_b);
/* Returns true if the object has collided with any rectangle in the object group */
bool check_object_group_collision(const AmphoraImage *obj, const char *name);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_COLLISION_H */
