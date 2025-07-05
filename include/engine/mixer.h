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
void Amphora_PlaySFX(const char *name, int channel, int repeat);
/* Sets the volume for a sound effect */
void Amphora_SetSFXVolume(const char *name, int volume);
/* Sets the current music track */
void Amphora_SetMusic(const char *name);
/* Play the current set music track with a fade-in, looping infinitely */
void Amphora_PlayMusic(int ms);
/* Play the current set music track with a fade-in, looping n times */
void Amphora_PlayMusicN(int n, int ms);
/* Pause the currently playing music track */
void Amphora_PauseMusic(void);
/* Unpause the currently playing music track if paused */
void Amphora_UnpauseMusic(void);
/* Stop the currently playing music track and free its resources immediately */
void Amphora_StopMusic(void);
/* Stop the currently playing music track and free its resources after a fade-out */
void Amphora_FadeOutMusic(int ms);
#ifdef __cplusplus
};
#endif

#endif /* AMPHORA_MIXER_H */
