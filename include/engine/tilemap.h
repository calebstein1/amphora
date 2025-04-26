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
void Amphora_SetMap(const char *name, float scale);
const SDL_FRect *Amphora_GetMapRectangle(void);
void Amphora_HideMapLayer(const char *name, int t);
void Amphora_ShowMapLayer(const char *name, int t);
#ifdef __cplusplus
}
#endif

#endif /* TILEMAP_H */
#endif
