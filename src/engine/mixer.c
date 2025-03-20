#ifndef DISABLE_MIXER
#ifdef WIN32
#include <windows.h>
#endif

#include "engine/internal/mixer.h"
#include "engine/internal/ht_hash.h"

/* Prototypes for private functions */
static void Amphora_FreeMusic(void);

/* File-scoped variables */
static HT_HashTable sfx;
static HT_HashTable music;
static HT_HashTable open_sfx;
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
Amphora_PlaySFX(const char *name, const int channel, const int repeat) {
	if (!HT_GetValue(name, open_sfx)) {
		HT_StoreRef(name, Mix_LoadWAV_RW(HT_GetRef(name, SDL_RWops, sfx), 0), open_sfx);
	}
	if (channel > -1 && Mix_Playing(channel)) return;

	Mix_PlayChannel(channel, HT_GetRef(name, Mix_Chunk, open_sfx), repeat);
}

void
Amphora_SetMusic(const char *name) {
	SDL_RWops *mus_rw = HT_GetRef(name, SDL_RWops, music);

	if (Mix_PlayingMusic()) {
		Mix_HaltMusic();
		Mix_FreeMusic(current_music);
		current_music = NULL;
	}

	SDL_RWseek(mus_rw, 0, RW_SEEK_SET);
	current_music = Mix_LoadMUS_RW(mus_rw, 0);
}

void
Amphora_PlayMusic(int ms) {
	if (Mix_PlayingMusic()) return;

	Mix_FadeInMusic(current_music, -1, ms);
}

void
Amphora_PlayMusicN(int n, int ms) {
	if (Mix_PlayingMusic()) return;

	Mix_FadeInMusic(current_music, n, ms);
}

void
Amphora_PauseMusic(void) {
	if (!Mix_PlayingMusic()) return;

	Mix_PauseMusic();
}

void
Amphora_UnpauseMusic(void) {
	if (!Mix_PausedMusic()) return;

	Mix_ResumeMusic();
}

void
Amphora_StopMusic(void) {
	if (!Mix_PlayingMusic()) return;

	Mix_HaltMusic();
}

void
Amphora_FadeOutMusic(int ms) {
	if (!Mix_PlayingMusic()) return;

	Mix_FadeOutMusic(ms);
}

/*
 * Internal functions
 */

int
Amphora_InitSFX(void) {
	int i;
#ifdef WIN32
	HRSRC sfx_info;
	HGLOBAL sfx_resource;

	sfx = HT_NewTable();
	open_sfx = HT_NewTable();

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
		HT_StoreRef(sfx_names[i], SDL_RWFromConstMem(sfx_resource, SizeofResource(NULL, sfx_info)), sfx);
	}
#else
#define LOADSFX(name, path) extern char name##_sf[]; extern int name##_sf_size;
	SFX
#undef LOADSFX
	sfx = HT_NewTable();
	open_sfx = HT_NewTable();
#define LOADSFX(name, path) HT_StoreRef(#name, SDL_RWFromConstMem(name##_sf, name##_sf_size), sfx);
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
Amphora_InitMusic(void) {
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
		HT_StoreRef(music_names[i], SDL_RWFromConstMem(music_resource, SizeofResource(NULL, music_info)), music);
	}
#else
#define LOADMUSIC(name, path) extern char name##_mu[]; extern int name##_mu_size;
	MUSIC
#undef LOADMUSIC
	music = HT_NewTable();
#define LOADMUSIC(name, path) HT_StoreRef(#name, SDL_RWFromConstMem(name##_mu, name##_mu_size), music);
	MUSIC
#undef LOADMUSIC
#endif
#ifdef DEBUG
	for (i = 0; i < MUSIC_COUNT; i++) {
		SDL_Log("Found music %s\n", music_names[i]);
	}
#endif
	Mix_HookMusicFinished(Amphora_FreeMusic);

	return 0;
}

void
Amphora_CloseSFX(void) {
	int i;

	for (i = 0; i < SFX_COUNT; i++) {
		if (HT_GetValue(sfx_names[i], open_sfx)) {
			Mix_FreeChunk(HT_GetRef(sfx_names[i], Mix_Chunk, open_sfx));
		}
		SDL_FreeRW(HT_GetRef(sfx_names[i], SDL_RWops, sfx));;
	}
	HT_FreeTable(open_sfx);
	HT_FreeTable(sfx);
}

void
Amphora_CloseMusic(void) {
	int i;

	if (current_music && !Mix_FadingMusic()) {
		Amphora_FreeMusic();
	}
	for (i = 0; i < MUSIC_COUNT; i++) {
		SDL_FreeRW(HT_GetRef(music_names[i], SDL_RWops, music));;
	}
}

/*
 * Private functions
 */

static void
Amphora_FreeMusic(void) {
	Mix_FreeMusic(current_music);
	current_music = NULL;
}

#endif
