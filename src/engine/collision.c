#include "engine/internal/collision.h"
#include "engine/internal/img.h"
#include "engine/internal/tilemap.h"

bool
Amphora_CheckCollision(const AmphoraImage *obj_a, const AmphoraImage *obj_b) {
	return SDL_HasIntersectionF(&obj_a->rectangle, &obj_b->rectangle);
}

AmphoraCollision
Amphora_CheckObjectGroupCollision(const AmphoraImage *obj, const char *name) {
	int c, i, j;
	float o_sx, o_ex, o_sy, o_ey, r_sx, r_ex, r_sy, r_ey;
	float overlaps[4];
	AmphoraCollision dirs[] = {
		AMPHORA_COLLISION_LEFT,
		AMPHORA_COLLISION_RIGHT,
		AMPHORA_COLLISION_TOP,
		AMPHORA_COLLISION_BOTTOM
	};
	int min_overlap = 0;
	SDL_FRect *rects;

	if ((rects = Amphora_GetRectsByGroup(name, &c)) == NULL) return AMPHORA_COLLISION_NONE;

	for (i = 0; i < c; i++) {
		if (SDL_HasIntersectionF(&obj->rectangle, &rects[i])) {
			o_sx = obj->rectangle.x;
			o_ex = o_sx + obj->rectangle.w;
			o_sy = obj->rectangle.y;
			o_ey = o_sy + obj->rectangle.h;
			r_sx = rects[i].x;
			r_ex = r_sx + rects[i].w;
			r_sy = rects[i].y;
			r_ey = r_sy + rects[i].h;

			overlaps[0] = SDL_fabsf(o_ex - r_sx);
			overlaps[1] = SDL_fabsf(o_sx - r_ex);
			overlaps[2] = SDL_fabsf(o_ey - r_sy);
			overlaps[3] = SDL_fabsf(o_sy - r_ey);

			for (j = 1; j < 4; j++) {
				if (overlaps[j] < overlaps[min_overlap]) {
					min_overlap = j;
				}
			}

			return dirs[min_overlap];
		}
	}

	return AMPHORA_COLLISION_NONE;
}
