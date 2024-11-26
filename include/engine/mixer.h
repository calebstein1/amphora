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
/* Sets the current music track */
void set_music(const char *name);
/* Play the current set music track with a fade-in, looping infinitely */
void play_music(int ms);
/* Play the current set music track with a fade-in, looping n times */
void play_music_n(int n, int ms);
/* Pause the currently playing music track */
void pause_music(void);
/* Unpause the currently playing music track if paused */
void unpause_music(void);
/* Stop the currently playing music track and free its resources immediately */
void stop_music(void);
/* Stop the currently playing music track and free its resources after a fade-out */
void fade_music(int ms);
#ifdef __cplusplus
};
#endif

#endif /* AMPHORA_MIXER_H */
