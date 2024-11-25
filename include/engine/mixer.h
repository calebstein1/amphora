#ifndef AMPHORA_MIXER_H
#define AMPHORA_MIXER_H

#include "SDL.h"
#include "SDL_mixer.h"

#include "resources.h"

typedef enum sfx_e {
#define LOADSFX(name, path) name##_sf,
	SFX
#undef LOADSFX
	SFX_COUNT
} SfxName;

#ifdef __cplusplus
extern "C" {
#endif
/* Play a sound effect */
void play_sfx(const char *name, int channel, int repeat);
#ifdef __cplusplus
};
#endif

#endif /* AMPHORA_MIXER_H */
