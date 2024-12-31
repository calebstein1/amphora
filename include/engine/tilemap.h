#ifndef DISABLE_TILEMAP
#ifndef TILEMAP_H
#define TILEMAP_H

#include "resources.h"

enum tilemaps_e {
#define LOADMAP(name, path) name##_tm,
	MAPS
#undef LOADMAP
	MAPS_COUNT
};

#ifdef __cplusplus
extern "C" {
#endif
void set_map(const char *name, float scale);
#ifdef __cplusplus
}
#endif

#endif /* TILEMAP_H */
#endif
