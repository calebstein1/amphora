#ifndef DISABLE_MIXER
#ifdef WIN32
#include <windows.h>
#endif

#include "engine/internal/mixer.h"

/* Prototypes for private functions */
int get_sfx_by_name(const char *name);
int get_music_by_name(const char *name);
void free_music(void);

/* File-scoped variables */
static SDL_RWops *sfx[SFX_COUNT];
static SDL_RWops *music[MUSIC_COUNT];
static Mix_Chunk *open_sfx[SFX_COUNT];
static Mix_Music *current_music;
static const char *sfx_names[] = {
#define LOADSFX(name, path) #name,
	SFX
#undef LOADSFX
};
static const char *music_names[] = {
#define LOADMUSIC(name, path) #name,
	MUSIC
#undef LOADMUSIC
};

void
play_sfx(const char *name, const int channel, const int repeat) {
	const int idx = get_sfx_by_name(name);

	if (idx == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not find sfx: %s\n", name);
		return;
	}
	if (!open_sfx[idx]) {
		open_sfx[idx] = Mix_LoadWAV_RW(sfx[idx], 0);
	}
	if (channel > -1 && Mix_Playing(channel)) return;

	Mix_PlayChannel(channel, open_sfx[idx], repeat);
}

void
set_music(const char *name) {
	const int idx = get_music_by_name(name);

	if (idx == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not find music: %s\n", name);
		return;
	}
	if (Mix_PlayingMusic()) {
		Mix_HaltMusic();
		Mix_FreeMusic(current_music);
		current_music = NULL;
	}

	current_music = Mix_LoadMUS_RW(music[idx], 0);
}

void
play_music(int ms) {
	if (Mix_PlayingMusic()) return;

	Mix_FadeInMusic(current_music, -1, ms);
}

void
play_music_n(int n, int ms) {
	if (Mix_PlayingMusic()) return;

	Mix_FadeInMusic(current_music, n, ms);
}

void
pause_music(void) {
	if (!Mix_PlayingMusic()) return;

	Mix_PauseMusic();
}

void
unpause_music(void) {
	if (!Mix_PausedMusic()) return;

	Mix_ResumeMusic();
}

void
stop_music(void) {
	if (!Mix_PlayingMusic()) return;

	Mix_HaltMusic();
}

void
fade_music(int ms) {
	if (!Mix_PlayingMusic()) return;

	Mix_FadeOutMusic(ms);
}

/*
 * Internal functions
 */

int
init_sfx(void) {
	int i;
#ifdef WIN32
	HRSRC sfx_info;
	HGLOBAL sfx_resource;
	SDL_RWops *sfx_rw;

	for (i = 0; i < SFX_COUNT; i++) {
		if (!((sfx_info = FindResourceA(NULL, sfx_names[i], "SFX")))) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to locate sfx resource... Amphora will crash now\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to locate sfx resource... Amphora will crash now", 0);
			return -1;
		}
		if (!((sfx_resource = LoadResource(NULL, sfx_info)))) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to load sfx resource... Amphora will crash now\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to load sfx resource... Amphora will crash now", 0);
			return -1;
		}
		sfx_rw = SDL_RWFromConstMem(sfx_resource, SizeofResource(NULL, sfx_info));
		sfx[i] = sfx_rw;
	}
#else
#define LOADSFX(name, path) extern char name##_sf[]; extern int name##_sf_size;
	SFX
#undef LOADSFX
	SDL_RWops **sfx_ptr = sfx;
#define LOADSFX(name, path) *sfx_ptr = SDL_RWFromConstMem(name##_sf, name##_sf_size); sfx_ptr++;
	SFX
#undef LOADSFX
#endif
#ifdef DEBUG
	for (i = 0; i < SFX_COUNT; i++) {
		SDL_Log("Found sfx %s\n", sfx_names[i]);
	}
#endif

	return 0;
}

int
init_music(void) {
	int i;
#ifdef WIN32
	HRSRC music_info;
	HGLOBAL music_resource;
	SDL_RWops *music_rw;

	for (i = 0; i < MUSIC_COUNT; i++) {
		if (!((music_info = FindResourceA(NULL, music_names[i], "MUSIC")))) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to locate music resource... Amphora will crash now\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to locate music resource... Amphora will crash now", 0);
			return -1;
		}
		if (!((music_resource = LoadResource(NULL, music_info)))) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to load music resource... Amphora will crash now\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to load music resource... Amphora will crash now", 0);
			return -1;
		}
		music_rw = SDL_RWFromConstMem(music_resource, SizeofResource(NULL, music_info));
		music[i] = music_rw;
	}
#else
#define LOADMUSIC(name, path) extern char name##_mu[]; extern int name##_mu_size;
	MUSIC
#undef LOADMUSIC
	SDL_RWops **music_ptr = music;
#define LOADMUSIC(name, path) *music_ptr = SDL_RWFromConstMem(name##_mu, name##_mu_size); music_ptr++;
	MUSIC
#undef LOADMUSIC
#endif
#ifdef DEBUG
	for (i = 0; i < MUSIC_COUNT; i++) {
		SDL_Log("Found music %s\n", music_names[i]);
	}
#endif
	Mix_HookMusicFinished(free_music);

	return 0;
}

void
cleanup_sfx(void) {
	int i;

	for (i = 0; i < SFX_COUNT; i++) {
		if (open_sfx[i]) {
			Mix_FreeChunk(open_sfx[i]);
			open_sfx[i] = NULL;
		}
		SDL_FreeRW(sfx[i]);
		sfx[i] = NULL;
	}
}

void
cleanup_music(void) {
	int i;

	for (i = 0; i < MUSIC_COUNT; i++) {
		SDL_FreeRW(music[i]);
		sfx[i] = NULL;
	}
	if (current_music) {
		free_music();
	}
}

/*
 * Private functions
 */

int
get_sfx_by_name(const char *name) {
	int i;

	for (i = 0; i < SFX_COUNT; i++) {
		if (SDL_strcmp(name, sfx_names[i]) == 0) return i;
	}
	return -1;
}

int
get_music_by_name(const char *name) {
	int i;

	for (i = 0; i < MUSIC_COUNT; i++) {
		if (SDL_strcmp(name, music_names[i]) == 0) return i;
	}
	return -1;
}

void
free_music(void) {
	Mix_FreeMusic(current_music);
	current_music = NULL;
}

#endif
