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

typedef enum music_e {
#define LOADMUSIC(name, path) name##_mu,
	MUSIC
#undef LOADMUSIC
	MUSIC_COUNT
} MusicName;

#ifdef __cplusplus
extern "C" {
#endif
/* Play a sound effect */
void play_sfx(const char *name, int channel, int repeat);
void set_music(const char *name);
void play_music(int ms);
void play_music_n(int n, int ms);
void pause_music(void);
void unpause_music(void);
void stop_music(void);
void fade_music(int ms);
#ifdef __cplusplus
};
#endif

#endif /* AMPHORA_MIXER_H */
