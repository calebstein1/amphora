/*
 * DO NOT TOUCH THIS FILE!
 *
 * The engine requires this file in this format to dynamically read game data
 */

#include "engine/internal/scenes.h"
#include "engine/input.h"
#include "config.h"
#include "resources.h"
#include "scene_list.h"

int Amphora_StartEngine(void);
void Amphora_RegisterGameData(const char *, const char *);
void Amphora_RegisterWindowTitle(const char *);
void Amphora_RegisterPrefs(const char *, const char *, int, int, unsigned int, int);
void Amphora_RegisterSceneData(const AmphoraScene *, const char **, int);
void Amphora_RegisterImageData(const char **, const char **, int);
void Amphora_RegisterFontData(const char **, const char **, int);
void Amphora_RegisterMapData(const char **, const char **, int);
void Amphora_RegisterSFXData(const char **, const char **, int);
void Amphora_RegisterMusicData(const char **, const char **, int);

/* Scene data */

enum scene_list_e {
#define SCENE(name) name,
	SCENES
#undef SCENE
	SCENES_COUNT
};

#define SCENE(name) extern void name##_Init(void); extern void name##_Update(Uint32, const InputState *); extern void name##_Destroy(void);
	SCENES
#undef SCENE

static const AmphoraScene scene_structs[] = {
#define SCENE(name) { .init_func = name##_Init, .update_func = name##_Update, .destroy_func = name##_Destroy },
	SCENES
#undef SCENE
};
static const char *scene_names[] = {
#define SCENE(name) #name,
	SCENES
#undef SCENE
};

/* Image data */

enum images_e {
#define LOADIMG(name, path) name##_im,
	IMAGES
#undef LOADIMG
	IMAGES_COUNT
};

static const char *img_names[] = {
#define LOADIMG(name, path) #name,
	IMAGES
#undef LOADIMG
};
static const char *img_paths[] = {
#define LOADIMG(name, path) #path,
	IMAGES
#undef LOADIMG
};

/* Font data */

enum fonts_e {
#define LOADFONT(name, path) name##_ft,
	FONTS
#undef LOADFONT
	FONTS_COUNT
};

static const char *font_names[] = {
#define LOADFONT(name, path) #name,
	FONTS
#undef LOADFONT
};
static const char *font_paths[] = {
#define LOADFONT(name, path) #path,
	FONTS
#undef LOADFONT
};

/* Map data */

enum tilemaps_e {
#define LOADMAP(name, path) name##_tm,
	MAPS
#undef LOADMAP
	MAPS_COUNT
};

static const char *map_names[] = {
#define LOADMAP(name, path) #name,
	MAPS
#undef LOADMAP
};
static const char *map_paths[] = {
#define LOADMAP(name, path) #path,
	MAPS
#undef LOADMAP
};

/* SFX data */

enum sfx_e {
#define LOADSFX(name, path) name##_sf,
	SFX
#undef LOADSFX
	SFX_COUNT
};

static const char *sfx_names[] = {
#define LOADSFX(name, path) #name,
	SFX
#undef LOADSFX
};
static const char *sfx_paths[] = {
#define LOADSFX(name, path) #path,
	SFX
#undef LOADSFX
};

/* Music data */

enum music_e {
#define LOADMUSIC(name, path) name##_mu,
	MUSIC
#undef LOADMUSIC
	MUSIC_COUNT
};

static const char *music_names[] = {
#define LOADMUSIC(name, path) #name,
	MUSIC
#undef LOADMUSIC
};
static const char *music_paths[] = {
#define LOADMUSIC(name, path) #path,
	MUSIC
#undef LOADMUSIC
};

int
main(int argc, char *argv[])
{
	(void)argc, (void)argv;

	Amphora_RegisterGameData(GAME_AUTHOR, GAME_TITLE);
	Amphora_RegisterWindowTitle(GAME_TITLE);
	Amphora_RegisterPrefs(GAME_AUTHOR, GAME_TITLE, WINDOW_X, WINDOW_Y, WINDOW_MODE, FRAMERATE);
	Amphora_RegisterSceneData(scene_structs, scene_names, SCENES_COUNT);
	Amphora_RegisterImageData(img_names, img_paths, IMAGES_COUNT);
	Amphora_RegisterFontData(font_names, font_paths, FONTS_COUNT);
	Amphora_RegisterMapData(map_names, map_paths, MAPS_COUNT);
	Amphora_RegisterSFXData(sfx_names, sfx_paths, SFX_COUNT);
	Amphora_RegisterMusicData(music_names, music_paths, MUSIC_COUNT);

	Amphora_StartEngine();
}
