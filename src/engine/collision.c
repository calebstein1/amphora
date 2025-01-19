#include "engine/internal/collision.h"
#include "engine/internal/img.h"
#include "engine/internal/tilemap.h"

/* Prototypes for private functions */
int sort_collisions(const void *a, const void *b);

bool
check_collision(const AmphoraImage *obj_a, const AmphoraImage *obj_b) {
	return SDL_HasIntersectionF(&obj_a->rectangle, &obj_b->rectangle);
}

AmphoraCollision
check_object_group_collision(const AmphoraImage *obj, const char *name) {
	int c, i;
	float o_sx, o_ex, o_sy, o_ey, r_sx, r_ex, r_sy, r_ey;
	struct overlaps_t overlaps[4];
	SDL_FRect *rects;

	if ((rects = get_rects_by_group(name, &c)) == NULL) return AMPHORA_COLLISION_NONE;

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

			overlaps[0] = (struct overlaps_t){ o_ex - r_sx, AMPHORA_COLLISION_LEFT };
			overlaps[1] = (struct overlaps_t){ o_sx - r_ex, AMPHORA_COLLISION_RIGHT };
			overlaps[2] = (struct overlaps_t){ o_ey - r_sy, AMPHORA_COLLISION_TOP };
			overlaps[3] = (struct overlaps_t){ o_sy - r_ey, AMPHORA_COLLISION_BOTTOM };

			SDL_qsort(overlaps, 4, sizeof(struct overlaps_t), sort_collisions);

			return overlaps[0].dir;
		}
	}

	return AMPHORA_COLLISION_NONE;
}

/*
 * Private functions
 */

int
sort_collisions(const void *a, const void *b) {
	float f1 = ((const struct overlaps_t *)a)->diff;
	float f2 = ((const struct overlaps_t *)b)->diff;

	if (f1 < 0) f1 *= -1;
	if (f2 < 0) f2 *= -1;

	return f1 - f2 > 0 ? 1 : -1;
}
