#include "engine/internal/collision.h"
#include "engine/internal/img.h"
#include "engine/internal/tilemap.h"

bool
check_collision(const AmphoraImage *obj_a, const AmphoraImage *obj_b) {
	return SDL_HasIntersectionF(&obj_a->rectangle, &obj_b->rectangle);
}

bool
check_object_group_collision(const AmphoraImage *obj, const char *name) {
	int c, i;
	SDL_FRect *rects;

	if ((rects = get_rects_by_group(name, &c)) == NULL) return false;

	for (i = 0; i < c; i++) {
		if (SDL_HasIntersectionF(&obj->rectangle, &rects[i])) return true;
	}

	return false;
}
