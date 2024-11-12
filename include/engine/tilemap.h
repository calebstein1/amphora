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

void set_map(const char *name, Uint16 scale);

#endif /* TILEMAP_H */
#endif
